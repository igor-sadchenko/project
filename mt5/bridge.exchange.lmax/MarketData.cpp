#include "stdafx.h"
#include "MarketData.h"

namespace lmax {

	MarketData::MarketData()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	MarketData::~MarketData()
	{
		UnInit();
	}

	int MarketData::Init() {
		/*m_pMDCacheQ = new shared::Memmap<shared::SystemID, shared::marketdata::MarketData>(Global::GetSetup()->MarketDataCachePathname(), sk::packet(__FUNCTION__).append(".marketdata.mapping"));*/
		sk::intptr out = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_tcp_client_setup>(shared::api_network, shared::api_network_tcp_client_setup)(Global::GetSetup()->MarketDataTcptoAddress().c_str(), out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}
	int MarketData::UnInit() {
		/*SK_DELETE_PTR(m_pMDCacheQ);*/
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}
	int MarketData::Open() {
		if (m_IsOpen.load()) return 0;

		auto FixconBuf = sk::Helper::LoadFile(Global::GetSetup()->MarketDataFixcon().c_str());
		if (!FixconBuf.size()) return -1;
		sk::intptr outPtr = 0;
		Global::GetApi()->GetInterFaceApi<shared::tf_api_libfix_setup>(shared::api_libfix, shared::api_libfix_setup)(&FixconBuf[0], FixconBuf.size(), outPtr);
		if (!outPtr) return -1;
		FixApi(outPtr);
		FixApi()->Open(this);

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
		NetApi<sk::network::INetworkApi>()->Start(this);

		m_IsOpen.store(true);
		return 0;
	}
	int MarketData::Close() {
		if (!m_IsOpen.load()) return 0;
		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();
		NetApi<sk::network::INetworkApi>()->Stop();
		FixApi()->Close();
		FixApi()->Release();
		m_IsOpen.store(false);
		return 0;
	}

	void MarketData::OnTcpClientConnect()
	{
		
	}

	void MarketData::OnTcpClientDisconnect()
	{
		
	}

	void MarketData::OnTcpClientWelcome(const sk::network::Header&, const sk::packet&)
	{
		NetLoginRequest(sk::packet((char*)Global::GetSetup()->LoginFieldMarketData(), shared::LENLOGINFIELD));
	}

	void MarketData::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::NetCmd::EN_NETCMD_100000070://!@ Bridge Login Result
		{
			OnNetLoginResult(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000030:
		{
			OnNetShutdown(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000010:
		{
			OnNetSystemTime(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000020:
		{
			OnNetSystemSetup(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000070:
		{
			OnNetShowWindow(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000110:
		{
			OnNetProductsRequest(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000120:
		{
			OnNetSystemProducts(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000320:
		{
			OnNetMDCacheReqeust(head, pak);
		}break;
		}///switch
	}


	void MarketData::OnNetLoginResult(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(head.Retcode))
		{
			/*NetStatus(sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED);*/
			NetSystemTimeRequest();
			NetSystemSetupRequest();
			NetSystemProductsRequest();
		}
	}
	void MarketData::OnNetShutdown(const sk::network::Header& head, const sk::packet& pak)
	{
		if (!Global::SystemExit())
		{
			Global::SystemExit(1);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", "System request close , closing . . .");
		}
	}
	void MarketData::OnNetShowWindow(const sk::network::Header& head, const sk::packet& pak)
	{
		if (head.DataExtern.ExternDataI == 0)
		{
			if (::IsWindowVisible(Global::GetHwnd()))
				::ShowWindow(Global::GetHwnd(), SW_HIDE);
		}
		else
		{
			if (!::IsWindowVisible(Global::GetHwnd()))
				::ShowWindow(Global::GetHwnd(), SW_SHOW);
		}
		//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
	}
	void MarketData::OnNetSystemSetup(const sk::network::Header& head, const sk::packet& pak)
	{
		shared::BridgeSystemSetup* setup = (shared::BridgeSystemSetup*)pak.data();
		if (setup)
		{
			Global::GetSetup()->BridgeSystemSetup(*setup);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
		}
	}
	void MarketData::OnNetSystemTime(const sk::network::Header& head, const sk::packet& pak)
	{
		std::int64_t* time = (std::int64_t*)pak.data();
		if (time)
		{
			Global::GetSetup()->BridgeSystemTime(*time);
		}
	}
	void MarketData::OnNetProductsRequest(const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet productsBuf;
		Global::GetSetup()->m_ProductQ.iterate(
			[&](const auto&, shared::LPProduct& product)
			{
				productsBuf.append((char*)&product, shared::LENLPPRODUCT);
			});
		NetProductsSend(productsBuf);
	}
	void MarketData::OnNetSystemProducts(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		auto products = sk::Helper::OneUnknownParseStruct<shared::LPProduct>(pak.data(), pak.size());
		if (!products)
			return;
		for (const auto& product : *products)
		{
			m_ProductQ.push(product.ID, product);
		}
	}
	void MarketData::OnNetMDCacheReqeust(const sk::network::Header& head, const sk::packet& pak)
	{
		//if (!MDCacheGet())
		//	return;
		sk::packet quotes;
		//MDCacheGet()->Iterate(
		//	[&](const auto&, shared::marketdata::MarketData& md)
		//	{
		//		quotes.append((char*)&md, shared::marketdata::LENMARKETDATA);
		//	});
		if (!quotes.empty())
		{
			NetMarketDataCacheSend(quotes);
		}
	}

	void MarketData::OnFixLogon()
	{
		FixStatus(libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED);
		FixMarketDataRequest();
	}

	void MarketData::OnFixLogout()
	{
		FixStatus(libfix::EnFixStatus::EN_FIXSTATUS_DISCONNECTED);
		Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", SKAPIRES_DESC(SKAPIRES::EXCHANGE_RET_ERROR_NETWORK_DISCONNECT));
	}

	void MarketData::OnFixResponse(const libfix::EnFixMessageType& FixMsgType, const sk::intptr& FixPak)
	{
		switch (FixMsgType)
		{
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_0:
		{
			FixHeartbeat(PtrToType(libfix::_0__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_3:
		{
			FixReject(PtrToType(libfix::_3__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_W:
		{
			FixMarketDataSnapshotFullRefresh(PtrToType(libfix::_W__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_Y:
		{
			FixMarketDataRequestReject(PtrToType(libfix::_Y__, FixPak));
		}break;
		}///switch
	}

	int MarketData::FixMarketDataRequest()
	{
		sk::stringa requests;
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			return -1;
		Global::GetSetup()->m_ProductQ.iterate(
			[&](const auto&, auto& inst)
			{
				libfix::_V__ v;
				v._262 = FixApi()->MakeReqID();
				v._263 = "1";
				v._264 = "0";
				v._265 = "0";
				v.__267.emplace_back(libfix::_V__::_267{ "0" });
				v.__267.emplace_back(libfix::_V__::_267{ "1" });
				v.__146.emplace_back(libfix::_V__::_146{ tostringa(inst.ID),"8" });
				sk::uint32 RefSeqNum = 0;
				if (0 == FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_V, TypeToPtr(&v), RefSeqNum))
				{
					requests.append(inst.Symbol.symbol).append(",").append(tostringa(inst.ID));
					m_MDRequestCacheQ.push(strtoull(v._262.c_str(), nullptr, 10), inst.ID);				
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			});
		if (!requests.empty())
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("请求报价({})", requests);
		}
#if 0
		libfix::_V__ v;
		v._262 = tostringa(shared::SystemIDType::IDMade(Global::GetSetup()->LoginFieldMarketData()->ID, time(0)));
		v._263 = "1";
		v._264 = "0";
		v._265 = "0";
		v.__267.emplace_back(libfix::_V__::_267{ "0" });
		v.__267.emplace_back(libfix::_V__::_267{ "1" });
		Global::GetSetup()->m_ProductQ.iterate(
			[&](const auto&, auto& inst)
			{
				v.__146.emplace_back(libfix::_V__::_146{ tostringa(inst.ID),"8" });
				m_MDRequestCacheQ.push(strtoull(v._262.c_str(), nullptr, 10), inst.ID);
			});
		sk::uint32 RefSeqNum = 0;
		if (0 == FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_V, TypeToPtr(&v), RefSeqNum))
		{
		}
#endif
		return 0;
	}
	void MarketData::FixReject(const libfix::_3__* tag3)
	{

	}
	void MarketData::FixMarketDataRequestReject(const libfix::_Y__* tagY)
	{
		if (!tagY) return;
		m_MDRequestCacheQ.pop(
			strtoull(tagY->_262.c_str(), nullptr, 10),
			[&](const auto&, auto& id)
			{
				//! 如果报价请求失败了,就从报价配置映影中KO掉该节点,
				//!	 因为交易定单与报价共用一块合约品种的缓冲映像.
				Global::GetSetup()->m_ProductQ.pop(
					id,
					[&](const auto&, auto& inst)
					{
#if 0
						Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
							"MarketDataRequestReject(MDReqID({}),InstID({}),Instrument({}),Reason({}))",
							tagY->_262,
							inst.ID,
							inst.Symbol.symbol,
							tagY->_58);

						Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn(
							"MarketDataRequest Failed, so ko at ({}.{}) by queue mapping.",
							inst.ID,
							inst.Symbol.symbol);
#endif
					});
			});
	}
	void MarketData::FixMarketDataSnapshotFullRefresh(const libfix::_W__* tagW)
	{
		if (!tagW || tagW->_48.empty()) return;
		
		shared::marketdata::MarketData md;
		auto found = Global::GetSetup()->m_ProductQ.search(
			std::stoll(tagW->_48),
			[&](const auto&, auto& instrument)
			{
				md.ID = instrument.ID;//!@ 交易所ID
				md.Symbol = instrument.SystemSymbol;
				md.GMTQuoteTimestamp = sk::Helper::TickCountGet<std::chrono::milliseconds>();
			});
		if (!found)
		{
			//Global::Log()->error("lmax not found instrument code({})", tagW->_48);
			return;
		}
		if (md.ID <= 0) {
			//Global::Log()->error("{}", "lmax not find instrument ID");
			return;
		}
		if (tagW->__268.empty()) {
			/*Global::Log()->error("{}", "lmax invalid quote");*/
			return;
		}
		int index_ask = 0;
		int index_bid = 0;
		for (const auto& quotes : tagW->__268) {
			if (!quotes._269.compare("0")) {//! Bid
				md.PriceBid[index_bid] = std::stod(quotes._270);
				md.VolumeBid[index_bid] = std::stod(quotes._271);
				++index_bid;
			}
			else if (!quotes._269.compare("1")) {//! Offer
				md.PriceOffer[index_ask] = std::stod(quotes._270);
				md.VolumeOffer[index_ask] = std::stod(quotes._271);
				++index_ask;
			}
			if (index_ask >= shared::marketdata::MD_QUOTE_LEVEL && index_bid >= shared::marketdata::MD_QUOTE_LEVEL)
				break;
		}

		//!@ Bid < Offer
		md.PriceBidFinal = md.PriceBid[0];
		md.PriceOfferFinal = md.PriceOffer[0];

		m_MDUpdateQ.push(md);
	}


	void MarketData::ProcessT()
	{
		sk::packet quotes;
		do {
			quotes.clear();
			do {
				auto md = m_MDUpdateQ.pop();
				if (!md) break;
				quotes.append((char*)md.get(), shared::marketdata::LENMARKETDATA);
				//MDCacheGet()->Push(md->ID/*交易所产品ID*/, *md);
			} while (1);
			if (!quotes.empty())
			{
				NetMarketDataSend(quotes);
			}
			if (!m_IsOpenT.load()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} while (1);
	}

}///namespace lmax