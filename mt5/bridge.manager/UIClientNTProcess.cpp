#include "stdafx.h"
#include "UIClient.h"

namespace client {

	void UIClient::OnTcpClientConnect()
	{

	}

	void UIClient::OnTcpClientDisconnect()
	{

	}

	void UIClient::OnTcpClientWelcome(const sk::network::Header& head, const sk::packet& pak)
	{
		NetLoginRequest();
	}

	void UIClient::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::EnNetCmd::EN_NETCMD_100000050:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000060:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000070:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000010:
		{
			OnNetSystemTime(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000020:
		{
			OnNetSystemSetup(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000040:
		{
			OnNetSystemExchanges(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000060:
		{
			OnNetSystemAssignLoginsPostOrder(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000120:
		{
			OnNetSystemProducts(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000210:
		{
			OnNetSystemProductCons(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000310:
		{
			OnNetSystemMDUpdate(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000320:
		{
			OnNetSystemMDCache(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000420:
		{
			OnNetSystemExchangePositionUpdate(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000451:
		{
			OnNetSystemTradeRecordHead(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000452:
		{
			OnNetSystemTradeRecordUpdate(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000510:
		{
			OnNetSystemMTUsers(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000530:
		{
			OnNetSystemPosition(head, pak);
		}break;
		default:
		{
		}break;
		}///switch

		Global::SharedGet()->m_NetSync.Response(head.Cmd, head, pak);
	}

	void OnNetSystemProducts(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		auto Products = sk::Helper::OneUnknownParseStruct<shared::SystemProduct>(pak.data(), pak.size());
		if (!Products || Products->empty())
			return;
		for (const auto& prod : *Products)
		{
			Global::SharedGet()->m_SystemProductQ.push(shared::SystemIDType::SystemProductType(prod.ID), prod);
		}
	}
	void UIClient::OnNetSystemTime(const sk::network::Header& head, const sk::packet& pak)
	{
		auto pSystemTime = (__int64*)pak.data();
		if (pSystemTime)
		{
			Global::SetupGet()->BridgeSystemTime(*pSystemTime);
		}
		OnUISystemTimeUpdate();
	}
	void UIClient::OnNetSystemSetup(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		auto pSystemSetup = (shared::BridgeSystemSetup*)pak.data();
		Global::SetupGet()->BridgeSystemSetup(*pSystemSetup);
		OnUISystemSetupRefresh();
	}
	void OnNetSystemExchanges(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
	}
	void UIClient::OnNetSystemProducts(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		auto sysProds = sk::Helper::OneUnknownParseStruct<shared::SystemProduct>(pak.data(), pak.size());
		if (!sysProds || sysProds->empty())
			return;
		Global::SharedGet()->m_SystemProductQ.clear();
		for (const auto& prod : *sysProds)
		{
			Global::SharedGet()->m_SystemProductQ.push(shared::SystemIDType::SystemProductType(prod.ID), prod);
		}
		OnUISystemProductsUpdate();
	}

	void UIClient::OnNetSystemProductCons(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty() || head.DataExtern.ExternDataU64 <= 0)
			return;
		Global::SharedGet()->m_SystemSymbolConQ.push(head.DataExtern.ExternDataU64, pak);
		if (Global::SharedGet()->m_SystemSymbolConQ.size() == head.DataExtern.ExternDataI || head.DataExtern.ExternDataI == 1)
		{
			OnUISystemProductConRefresh();
		}
	}

	void UIClient::OnNetSystemTradeRecordHead(const sk::network::Header& head, const sk::packet& pak)
	{
		OnUISystemTradeRecordInit(pak);
	}

	void UIClient::OnNetSystemTradeRecordUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		OnUISystemTradeRecordUpdate(pak);
	}
	void UIClient::OnNetSystemExchanges(const sk::network::Header& head, const sk::packet& pak)
	{
		std::vector<shared::BridgeLoginField> exchanges;
		sk::Helper::OneUnknownParseStruct<shared::BridgeLoginField>(pak.data(), pak.size(), exchanges);
		if (exchanges.empty())
			return;
		for (const auto& node : exchanges)
		{
			Global::SharedGet()->m_ExchangeOnlineQ.push(node.ID, node);
		}
		OnUISystemExchangeOnlineUpdate();
	}
	void UIClient::OnNetSystemAssignLoginsPostOrder(const sk::network::Header& head, const sk::packet& pak)
	{
		if (head.DataExtern.ExternDataU64 <= 0)
			return;
		switch (head.DataProcessFlag)
		{
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
			[[fallthrough]];
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
		{
			if (pak.empty())
				break;
			auto pLogin = (UINT64*)pak.data();
			Global::SharedGet()->m_SystemAssignLoginsPostOrderQ.pushpush(
				head.DataExtern.ExternDataU64, 
				[&](auto& logins) 
				{
					logins.push(*pLogin);
				});
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
		{
			if (pak.empty())
				break;
			auto pLogin = (UINT64*)pak.data();
			Global::SharedGet()->m_SystemAssignLoginsPostOrderQ.iterate(
				[&](const auto&,auto& logins)
				{
					logins.pop(*pLogin);
				});
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
		{
			if (pak.empty())
			{
				Global::SharedGet()->m_SystemAssignLoginsPostOrderQ.pop(head.DataExtern.ExternDataU64, [](const auto&,auto&) {});
			}
			else
			{
				std::vector<UINT64> logins;
				sk::Helper::OneUnknownParseStruct<UINT64>(pak.data(), pak.size(), logins);
				sk::container::set<UINT64> at;
				for (const auto& login : logins)
				{
					at.push(login);
				}
				Global::SharedGet()->m_SystemAssignLoginsPostOrderQ.push(head.DataExtern.ExternDataU64, at);
			}
		}break;
		}///switch


		OnUISystemAssignLoginsPostOrderUpdate(head.DataExtern.ExternDataU64);
	}
	void UIClient::OnNetSystemExchangePositionUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		shared::tradeorder::ExchangePosition* target = nullptr;
		std::vector<shared::tradeorder::ExchangePosition> positions;
		switch (head.DataProcessFlag)
		{
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
		{
			sk::Helper::OneUnknownParseStruct<shared::tradeorder::ExchangePosition>(pak.data(), pak.size(), positions);
			if (positions.empty())
				return;
			Global::SharedGet()->m_ExchangePositionQ.clear();
			for (const auto& node : positions)
			{
				Global::SharedGet()->m_ExchangePositionQ.pushpush(head.DataExtern.ExternDataU64, [&](auto& q) {q.push(node.ID, node); });
			}
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
		{

			if (pak.empty())
				break;
			target = (shared::tradeorder::ExchangePosition*)pak.data();
			Global::SharedGet()->m_ExchangePositionQ.pushpush(head.DataExtern.ExternDataU64, [&](auto& q) {q.push(target->ID, *target); });
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
		{
			if (pak.empty())
				break;
			target = (shared::tradeorder::ExchangePosition*)pak.data();
			Global::SharedGet()->m_ExchangePositionQ.search(head.DataExtern.ExternDataU64,
				[&](const auto&, auto& obj) {
					obj.pop(target->ID, [&](const auto&, auto&) {});
				});
		}break;
		}

		if (!Global::GetCore()->PositionUpdateFlag())
		{
			Global::GetCore()->PositionUpdateFlag(true);
		}
	}

	void UIClient::OnNetSystemMDCache(const sk::network::Header& head, const sk::packet& pak)
	{
		std::vector<shared::marketdata::MarketData> mds;
		sk::Helper::OneUnknownParseStruct<shared::marketdata::MarketData>(pak.data(), pak.size(), mds);
		if (mds.empty())
			return;
		for (const auto& md : mds)
		{
			Global::SharedGet()->m_MarketDataQ.push(md.ID, md);
			Global::SharedGet()->m_MarketDataUpdateQ.push(md);
		}
	}
	void UIClient::OnNetSystemMTUsers(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;

		switch (head.DataProcessFlag)
		{
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
		{
			auto pUser = (shared::UserSimpleField*)pak.data();
			Global::SharedGet()->m_MTUserSimpleQ.pop(pUser->Login, [](const auto&, auto&) {});
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
		{
			std::vector<shared::UserSimpleField> users;
			sk::Helper::OneUnknownParseStruct<shared::UserSimpleField>(pak.data(), pak.size(), users);
			if (users.empty())
				return;

			for (const auto& user : users)
			{
				Global::SharedGet()->m_MTUserSimpleQ.push(user.Login, user);
			}
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
			[[fallthrough]];
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
		{
			auto pUser = (shared::UserSimpleField*)pak.data();
			Global::SharedGet()->m_MTUserSimpleQ.push(pUser->Login, *pUser);
		}break;
		}///switch

		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("({})(MTUsers({}))", __FUNCTION__, Global::SharedGet()->m_MTUserSimpleQ.size());
	}
	void UIClient::OnNetSystemPosition(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;

		shared::SystemID ExchangeID = shared::SystemIDType::ExchangeType(head.DataExtern.ExternDataU64);
		Global::SharedGet()->m_SystemPositionQ.pop(ExchangeID, [&](const auto&,auto&) {});
		std::vector<shared::SimplePositionField> Positions;
		sk::Helper::OneUnknownParseStruct<shared::SimplePositionField>(pak.data(), pak.size(), Positions);
		
		for (const auto& position : Positions)
		{
			Global::SharedGet()->m_SystemPositionQ.pushpush(
				ExchangeID,
				[&](auto& positions)
				{
					positions.pushpush(
						shared::SystemIDType::SystemProductType(position.ExtGatewayPID),
						[&](shared::SimplePositionField& field)
						{
							if (field.ExtTotalQty <= 0)
							{
								field = position;
							}
							else
							{
								if (shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY == position.Side)
								{
									field.ExtTotalQty += (INT64)position.Volume;
								}
								else
								{
									field.ExtTotalQty -= (INT64)position.Volume;
								}
							}
						});
				});
		}


		OnUISystemExchangePositionUpdate();
	}

	void UIClient::OnNetSystemMDUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		size_t pos = 0;
		size_t total = pak.size();
		size_t node = shared::marketdata::LENMARKETDATA;
		do {
			if (total < node) break;
			auto quote = (shared::marketdata::MarketData*)(pak.data() + pos);
			if (!quote) break;
			Global::SharedGet()->m_MarketDataUpdateQ.push(*quote);
			pos += node;
			total -= node;
		} while (1);
	}

	int UIClient::NetLoginRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_100000070;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&Global::SetupGet()->LoginField(), shared::LENLOGINFIELD), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemTimeRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000010;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemSetupUpdateRequest(const shared::BridgeSystemSetup& setup) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000020;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&setup, shared::LENBRIDGESYSTEMSETUP), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemProductsRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000120;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemProductConsRequest(
		shared::SystemID reqID /*= 0*/,
		sk::network::Header::EnDataProcessFlag ProcessFlag/*= sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT*/,
		sk::packet update /*= ""*/) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.DataExtern.ExternDataU64 = reqID;
		head.DataProcessFlag = ProcessFlag;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000210;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(update, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int UIClient::NetSystemTradeRecordHead() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000451;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int UIClient::NetSystemTradeRecordUpdate(int count) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.DataExtern.ExternDataI = count;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000452;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemMDCacheRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000320;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemExchangeOnline() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000040;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemExchangePositionRequest(const shared::SystemID& exchangeID) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.DataExtern.ExternDataU64 = exchangeID;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000420;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemPositionRequest(const shared::SystemID& exchangeID) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.DataExtern.ExternDataU64 = exchangeID;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000530;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemMTUsersRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000510;
		head.DataProcessFlag = sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemAssignLoginsPostOrder(const shared::SystemID& ExchangeID, const sk::network::HEADER::EnDataProcessFlag& flag, const sk::packet& pak) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_300000060;
		head.DataExtern.ExternDataU64 = ExchangeID;
		head.DataProcessFlag = flag;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(pak, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
}///namespace client