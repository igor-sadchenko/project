#include "stdafx.h"
#include "DataSource.h"

namespace feeder {

	DataSource::DataSource()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	DataSource::~DataSource()
	{
		UnInit();
	}

	int DataSource::Init()
	{
		sk::intptr out = 0;
		Feeder::ApiGet()->GetFunction<shared::tf_api_network_tcp_client_setup>(shared::api_network, shared::api_network_tcp_client_setup)(Feeder::SetupGet()->TcptoAddr().c_str(), out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}

	int DataSource::UnInit()
	{
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	bool DataSource::Ready() const
	{
		return
			m_IsOpen.load() &&
			NetApi<sk::network::INetworkApi>()->NetStatus() == sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED;
	}

	int DataSource::Open()
	{
		if (m_IsOpen.load()) return 0;

		NetApi<sk::network::INetworkApi>()->Start(this);

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });

		m_IsOpen.store(true);
		return 0;
	}

	int DataSource::Close()
	{
		if (!m_IsOpen.load()) return 0;

		NetApi<sk::network::INetworkApi>()->Stop();

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_IsOpen.store(false);
		return 0;
	}

	void DataSource::OnTcpClientConnect()
	{

	}

	void DataSource::OnTcpClientDisconnect()
	{

	}

	void DataSource::OnTcpClientWelcome(const sk::network::Header&, const sk::packet&)
	{
		NetLoginRequest();
		Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
	}

	void DataSource::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::NetCmd::EN_NETCMD_100000070://!@ Bridge Login Result
		{
			OnNetLoginResult(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000110:
		{
			OnNetProductsGet(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000120:
		{
			OnSystemProducts(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000310:
		{
			OnNetMarketDataUpdate(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000010:
		{
			OnNetSystemTimeUpdate(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000020:
		{
			OnNetSystemSetupUpdate(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000210:
		{

		}break;
		}///switch
	}

	void DataSource::OnNetLoginResult(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(head.Retcode))
		{
			/*NetStatus(sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED);*/
			NetSystemProductsRequest();
			FeederGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, "EN_NETSTATUS_SYNCHRONIZED");
			ReadyLoginVeirfy.store(true);
		}
	}
	void DataSource::OnNetSystemTimeUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		auto time = (__int64*)pak.data();
		if (time)
		{
			//m_pFeeder->SetupGet()->SystemServerTime(*time);
		}
	}
#if 0
	int DataSource::NetSymbolRemove(const sk::stringa& symbol) {
		if (!symbol.empty()) {
			auto netapi = NetApi<sk::network::INetworkApi>();
			if (netapi && NetStatus()) {
				std::vector<sk::packet> enpak;
				sk::network::Header head;
				head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100306);
				netapi->Packet(symbol, head, enpak);
				m_NetWriteQ.push(enpak);
				return 0;
			}
		}
		return -1;
	}
	int DataSource::NetSymbolAppend(const sk::stringa& symbol) {
		if (!symbol.empty()) {
			auto netapi = NetApi<sk::network::INetworkApi>();
			if (netapi && NetStatus()) {
				std::vector<sk::packet> enpak;
				sk::network::Header head;
				head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100305);
				netapi->Packet(symbol, head, enpak);
				m_NetWriteQ.push(enpak);
				return 0;
			}
		}
		return -1;
	}
#endif
	int DataSource::NetProductsSend() const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet products;
		Feeder::SharedGet()->m_MTSymbolQ.iterate(
			[&](const auto& symbol, shared::ConSymbolField& symbolField, const auto& count, auto&)
			{
				shared::MTFeederProduct product;
				product.ID = count;//!十进制
				product.Symbol = sk::StringConvert::WStringToMBytes(symbol).c_str();
				product.Point = symbolField.Point;
				product.Digits = symbolField.Digits;
				product.ContractSize = symbolField.ContractSize;
				products.append((char*)&product, shared::LENMTFEEDERPRODUCT);
			});
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000110);
		NetApi<sk::network::INetworkApi>()->Packet(products, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	void DataSource::OnNetProductsGet(const sk::network::Header& head, const sk::packet& pak) {
		NetProductsSend();
	}
	void DataSource::OnNetMarketDataUpdate(const sk::network::Header& head, const sk::packet& pak) {
		size_t pos = 0;
		size_t total = pak.size();
		size_t node = shared::marketdata::LENMARKETDATA;
		do {
			if (total < node) break;
			auto quote = (shared::marketdata::MarketData*)(pak.data() + pos);
			if (!quote) break;
			Feeder::SharedGet()->m_MarketDataUpdateQ.push(*quote);
			pos += node;
			total -= node;
		} while (1);
	}
	void DataSource::OnNetSystemSetupUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
		auto setup = (shared::BridgeSystemSetup*)pak.data();
		if (setup)
		{
			Feeder::SetupGet()->BridgeSystemSetup(*setup);
			Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
		}
	}
	void DataSource::ONetProductConUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		std::vector<shared::MTFeederProduct> vtData;
		sk::Helper::OneUnknownParseStruct<shared::MTFeederProduct>(pak.data(), pak.size(), vtData);
		for (const auto& prod : vtData)
		{
			Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("({})(FeederPID({:X}),Symbol({}))", __FUNCTION__, prod.ID, prod.Symbol.symbol);
		}


	}
	void DataSource::OnSystemProducts(const sk::network::Header& head, const sk::packet& pak) {

		Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);

		std::vector<shared::MTFeederProduct> vtData;
		sk::Helper::OneUnknownParseStruct<shared::MTFeederProduct>(pak.data(), pak.size(), vtData);
		for (const auto& prod : vtData)
		{
			Feeder::SharedGet()->m_ProdIDProdconQ.push(prod.ID, prod);
			Feeder::SharedGet()->m_MTSymbolProdconQ.push(prod.Symbol, prod);
			Feeder::SharedGet()->m_ProdIDMTSymbolQ.push(prod.ID, prod.Symbol);

			Feeder::SharedGet()->m_SymbolMappingQ.pushpush(
				shared::SystemIDType::SystemProductType(prod.ID),
				[&](auto& setObj)
				{
					setObj.push(prod);
				});
		}

		//! 打印映射地图
		auto targetFile = FeederGet()->PathObj() + "//sk.feeder.symbol.map";
		sk::Helper::CreateDirectoryA(targetFile.c_str());
		std::ofstream of(targetFile, std::ios::out | std::ios::trunc);
		if (of.is_open())
		{
			of << R"(新生联创（上海）)" << std::endl << std::endl;
			Feeder::SharedGet()->m_SymbolMappingQ.iterate(
				[&](const auto& sysProdID, auto& mtProds)
				{
					of << R"([)" << sk::Log::Format("{:X}", sysProdID) << R"(]->{)";
					mtProds.iterate(
						[&](auto& prod)
						{
							of << R"(()" << prod.Symbol.symbol << R"())" << R"(,)";
						});
					of << R"(})" << std::endl;
				});
			of.close();
		}

	}
	int DataSource::NetSystemProductsRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000120);
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int DataSource::NetLoginRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;

		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100000070);
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&Feeder::SetupGet()->BridgeLoginField(), sizeof(shared::BridgeLoginField)), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	void DataSource::ProcessT()
	{
		do
		{
			if (!Ready())
			{
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				if (!m_IsOpenT.load()) { break; }
				continue;
			}

			auto& SystemSetup = Feeder::SetupGet()->BridgeSystemSetup();

			auto pMD = Feeder::SharedGet()->m_MarketDataUpdateQ.pop();
			if (!pMD || \
				SystemSetup.FeederSpreadMode == shared::BridgeSystemSetup::EnFeederSpreadMode::EN_FEEDER_SPREAD_MODE_PLATFORM)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(10));
				if (!m_IsOpenT.load()) { break; }
				continue;
			}

			Feeder::SharedGet()->m_SymbolMappingQ.search(
				shared::SystemIDType::SystemProductType(pMD->ID),
				[&](const auto&, auto& setObj)
				{
					setObj.iterate(
						[&](shared::MTFeederProduct& prod)
						{
							if (prod.EnableQuote)
							{
								MTTick tick = { 0 };
								auto targetSymbol = sk::StringConvert::MBytesToWString(prod.Symbol.symbol);
								sk::SafeCopyW(tick.symbol, targetSymbol.c_str(), _countof(tick.symbol));
								tick.flags = MTTick::EnTickFlags::TICK_FLAG_ALL;
								tick.volume = static_cast<decltype(tick.volume)>(pMD->VolumeOffer[0]);
								tick.ask = pMD->PriceOfferFinal;
								tick.bid = pMD->PriceBidFinal;

								Feeder::SharedGet()->m_MTSymbolQ.search(
									targetSymbol,
									[&](const auto&, shared::ConSymbolField& symbolField)
									{
										auto totalOffset = prod.SpreadTotal * symbolField.Point;
										auto askSpread = prod.SpreadAsk * symbolField.Point;
										auto bidSpread = prod.SpreadBid * symbolField.Point;
										//! 买价
										tick.bid += (totalOffset + bidSpread);
										//! 卖价
										tick.ask += (totalOffset + askSpread);
									});


								//! 卖价要小于买价
								if (tick.bid > 0 && tick.ask > 0 /*&& tick.bid > tick.ask*/)
								{
									auto mtret = FeederGet()->ServerApi()->TickAdd(tick);
									if (MT_RET_OK != mtret)
									{
										Feeder::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->info(L"failed to SendTicks({})", mtret);
									}
								}
								else
								{
									Feeder::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->info(L"failed to parse tick, invalid bid/ask symbol({}),bid({:.5f}),ask({:.5f})", tick.symbol, tick.bid, tick.ask);
								}
							}
						});
				});

			std::this_thread::sleep_for(std::chrono::microseconds(10));
			if (!m_IsOpenT.load()) { break; }
		} while (1);
	}

}///namespace feeder