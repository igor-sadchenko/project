#include "stdafx.h"
#include "Exchange.h"

namespace dealer {

	void Exchange::OnNetLoginResult(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(head.Retcode))
		{
			/*NetStatus(sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED);*/
			NetSystemProductRequest();
			DealerGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, "EN_NETSTATUS_SYNCHRONIZED");
			ReadyLoginVeirfy.store(true);
		}
	}

	int Exchange::NetLoginRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;

		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100000070);
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&Dealer::SetupGet()->BridgeLoginField(), sizeof(shared::BridgeLoginField)), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int Exchange::NetMTUsersSend(const sk::network::Header::EnDataProcessFlag& ProcessFlag, const sk::packet& pak) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000510);
		head.DataProcessFlag = ProcessFlag;
		NetApi<sk::network::INetworkApi>()->Packet(pak, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int Exchange::NetMTSymbolsSend(const sk::network::Header::EnDataProcessFlag& ProcessFlag, const sk::packet& pak) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000520);
		head.DataProcessFlag = ProcessFlag;
		NetApi<sk::network::INetworkApi>()->Packet(pak, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int Exchange::NetSystemPositiionSend(const sk::network::Header::ExternData& externData, const sk::packet& pak) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000530);
		head.DataExtern = externData;
		NetApi<sk::network::INetworkApi>()->Packet(pak, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	void Exchange::OnNetMTUsersGet(const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet users;
		DealerGet()->SharedGet()->m_MTUserQ.iterate(
			[&](const auto&, auto& user)
			{
				users.append((char*)&user, shared::LENUSERFIELD);
			});
		if (!users.empty())
		{
			NetMTUsersSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT, users);
		}
	}
	void Exchange::OnNetSystemPositionsGet(const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet res;
		if (head.DataExtern.ExternDataU64 > 0)
		{
			Dealer::SharedGet()->m_MTPositionQ.iterate(
				[&](const auto&, shared::PositionField& position)
				{
					if (position.ExternalID[0])
					{
						auto GatewayProdcon = Dealer::SharedGet()->m_MTSymbolProdconQ.search(sk::StringConvert::WStringToMBytes(position.Symbol).c_str());
						if (GatewayProdcon)
						{
							shared::SimplePositionField Position;
							Position.ExtOrderID = shared::SystemIDType::WStrToSystemOrderID(&position.ExternalID[0]);
							Position.ExtExchangeID = shared::SystemIDType::SystemOrderIDMadeUn(Position.ExtOrderID, Position.ExtOrderTime);
							Position.ContractSize = position.ContractSize;
							Position.Digits = position.Digits;
							Position.ExtSignalLogin = position.ApiDataLogin;
							Position.ExtSignalPosition = position.ApiDataPosition;
							Position.ExtGatewayPID = GatewayProdcon->ID;
							Position.Login = position.Login;
							Position.Position = position.Position;
							Position.PriceOpen = position.PriceOpen;
							Position.Symbol = GatewayProdcon->Symbol;
							Position.Volume = position.Volume;
							Position.Side = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY;
							if (position.Action == IMTPosition::EnPositionAction::POSITION_SELL)
								Position.Side = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL;

							if (shared::SystemIDType::ExchangeType(head.DataExtern.ExternDataU64) == Position.ExtExchangeID)
							{
								res.append((char*)&Position, shared::LENSIMPLEPOSITIONFIELD);
							}
						}


					}
				});
		}
		else
		{
			Dealer::SharedGet()->m_MTPositionQ.iterate(
				[&](const auto&, shared::PositionField& position)
				{
					if (position.ExternalID[0])
					{
						auto GatewayProdcon = Dealer::SharedGet()->m_MTSymbolProdconQ.search(sk::StringConvert::WStringToMBytes(position.Symbol).c_str());
						if (GatewayProdcon)
						{
							shared::SimplePositionField Position;
							Position.ExtOrderID = shared::SystemIDType::WStrToSystemOrderID(&position.ExternalID[0]);
							Position.ExtExchangeID = shared::SystemIDType::SystemOrderIDMadeUn(Position.ExtOrderID, Position.ExtOrderTime);
							Position.ContractSize = position.ContractSize;
							Position.Digits = position.Digits;
							Position.ExtSignalLogin = position.ApiDataLogin;
							Position.ExtSignalPosition = position.ApiDataPosition;
							Position.ExtGatewayPID = GatewayProdcon->ID;
							Position.Login = position.Login;
							Position.Position = position.Position;
							Position.PriceOpen = position.PriceOpen;
							Position.Symbol = GatewayProdcon->Symbol;
							Position.Volume = position.Volume;
							Position.Side = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY;
							if (position.Action == IMTPosition::EnPositionAction::POSITION_SELL)
								Position.Side = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL;

							res.append((char*)&Position, shared::LENSIMPLEPOSITIONFIELD);
						}
				}
		});
	}

		NetSystemPositiionSend(head.DataExtern, res);
}
	void Exchange::OnNetMTSymbolsGet(const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet symbols;
		DealerGet()->SharedGet()->m_MTSymbolQ.iterate(
			[&](const auto&, auto& symbol)
			{
				symbols.append((char*)&symbol, shared::LENCONSYMBOLFIELD);
			});
		if (!symbols.empty())
		{
			NetMTSymbolsSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, symbols);
		}
	}
	void Exchange::OnNetSystemTimeUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		auto time = (__int64*)pak.data();
		if (time)
		{
			Dealer::SetupGet()->BridgeSystemTime(*time);
		}
	}

	void Exchange::OnNetSystemProducts(const sk::network::Header& head, const sk::packet& pak)
	{
		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);

		std::vector<shared::MTGatewayProduct> vtData;
		sk::Helper::OneUnknownParseStruct<shared::MTGatewayProduct>(pak.data(), pak.size(), vtData);
		for (const auto& prod : vtData) {
			Dealer::SharedGet()->m_ProdIDProdconQ.push(prod.ID, prod);
			Dealer::SharedGet()->m_MTSymbolProdconQ.push(prod.Symbol, prod);
			Dealer::SharedGet()->m_ProdIDMTSymbolQ.push(prod.ID, prod.Symbol);

			Dealer::SharedGet()->m_SymbolMappingQ.pushpush(
				shared::SystemIDType::SystemProductType(prod.ID),
				[&](auto& setObj)
				{
					setObj.push(prod);
				});
		}

		//! 打印映射地图
		auto targetFile = DealerGet()->PathObj() + "//sk.dealer.symbol.map";
		sk::Helper::CreateDirectoryA(targetFile.c_str());
		std::ofstream of(targetFile, std::ios::out | std::ios::trunc);
		if (of.is_open())
		{
			of << R"(新生联创（上海）)" << std::endl << std::endl;
			Dealer::SharedGet()->m_SymbolMappingQ.iterate(
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

		ReadyProductCon.store(true);
	}

	int Exchange::NetSystemProductRequest() const
	{
		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);

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


#if 0
	int Exchange::NetSymbolRemove(const sk::stringa& symbol) {
		if (!symbol.empty()) {
			auto netapi = NetApi<sk::network::INetworkApi>();
			if (netapi && NetStatus()) {
				std::vector<sk::packet> enpak;
				sk::network::Header head;
				head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100306);
				netapi->Packet(symbol, head, enpak);
				if (NetStatus())
				{
					NetApi<sk::network::INetworkApi>()->Write(enpak);
				}
				return 0;
			}
		}
		return -1;
	}
	int Exchange::NetSymbolAppend(const sk::stringa& symbol) {
		if (!symbol.empty()) {
			auto netapi = NetApi<sk::network::INetworkApi>();
			if (netapi && NetStatus()) {
				std::vector<sk::packet> enpak;
				sk::network::Header head;
				head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_100305);
				netapi->Packet(symbol, head, enpak);
				if (NetStatus())
				{
					NetApi<sk::network::INetworkApi>()->Write(enpak);
				}
				return 0;
	}
		}
		return -1;
}
#endif
	int Exchange::NetProductsSend() const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet products;
		Dealer::SharedGet()->m_MTSymbolQ.iterate(
			[&](const auto& symbol, shared::ConSymbolField& symbolField, const auto& count, auto&)
			{
				shared::MTGatewayProduct product;
				product.ID = count;//!十进制
				product.Symbol = sk::StringConvert::WStringToMBytes(symbol).c_str();
				product.Point = symbolField.Point;
				product.Digits = symbolField.Digits;
				product.ContractSize = symbolField.ContractSize;
				product.AutoAdjustPositionSpreadRange = 100;
				product.EnableTrade = true;
				products.append((char*)&product, shared::LENMTGATEWAYPRODUCT);
			});
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000110);
		NetApi<sk::network::INetworkApi>()->Packet(products, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	void Exchange::OnNetSystemSetupUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
		auto setup = (shared::BridgeSystemSetup*)pak.data();
		if (setup)
		{
			Dealer::SetupGet()->BridgeSystemSetup(*setup);
			Dealer::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
			ReadySystemSetup.store(true);
		}
	}
	void Exchange::OnNetProductsGet(const sk::network::Header& head, const sk::packet& pak)
	{
		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
		NetProductsSend();
	}

	void Exchange::OnNetSystemAssignLoginsPostOrder(const sk::network::Header& head, const sk::packet& pak)
	{
		if (head.DataExtern.ExternDataU64<=0)
			return;
		switch (head.DataProcessFlag)
		{
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
			[[fallthrough]];
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
		{
			if (pak.empty())
				break;
			auto pLogin = (UINT64*)pak.data();
			Dealer::SharedGet()->m_SystemAssignLoginsPostOrderQ.pushpush(head.DataExtern.ExternDataU64, [&](auto& logins) {logins.push(*pLogin); });
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
		{
			if (pak.empty())
				break;
			auto pLogin = (UINT64*)pak.data();
			Dealer::SharedGet()->m_SystemAssignLoginsPostOrderQ.iterate([&](const auto&, auto& logins) {logins.pop(*pLogin); });
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
		{
			if (pak.empty())
				break;
			std::vector<UINT64> logins;
			sk::Helper::OneUnknownParseStruct<UINT64>(pak.data(), pak.size(), logins);
			sk::container::set<UINT64> at;
			for (const auto& login : logins)
				at.push(login);
			Dealer::SharedGet()->m_SystemAssignLoginsPostOrderQ.push(head.DataExtern.ExternDataU64, at);
		}break;
		}///switch
	}

	void Exchange::OnNetTradeOrderRes(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		auto pOrderRes = (shared::SystemOrder*)pak.data();
		Dealer::SharedGet()->m_OrderResQ.push(*pOrderRes);
	}
	int Exchange::NetTradeOrderRequest(const sk::packet& orders) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000410);
		NetApi<sk::network::INetworkApi>()->Packet(orders, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}

	int Exchange::NetTradeOrderTimeoutSend(const shared::SystemOrder& timeout_order) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000415);
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&timeout_order, shared::LENSYSTEMORDER), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int Exchange::NetTradeOrderFailedSend(const shared::SystemOrder& failed_order) const
	{
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000414);
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&failed_order, shared::LENSYSTEMORDER), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int Exchange::NetSystemLogpakSend(const sk::packet& log) const
	{
		if (log.empty())
			return -1;
		if (!NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (sk::network::EnNetCmd::EN_NETCMD_300000050);
		NetApi<sk::network::INetworkApi>()->Packet(log, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
}///namespace dealer