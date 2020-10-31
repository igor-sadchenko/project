#include "stdafx.h"
#include "Server.h"

namespace proxy {

	void Server::OnNetMTUsersProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
			[[fallthrough]];
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
		{
			if (pak.empty())
				break;
			switch (head.DataProcessFlag)
			{
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
				[[fallthrough]];
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
			{
				auto pUser = (shared::UserField*)pak.data();
				Global::SharedGet()->m_MTUserQ.push(pUser->Login, *pUser);

				shared::UserSimpleField simple;
				simple.Login = pUser->Login;
				sk::SafeCopyA(simple.Group, sk::StringConvert::WStringToMBytes(pUser->Group).c_str(), _countof(simple.Group));
				m_ServiceObjQ.search(pContext->Flag(),
					[&](const auto&, auto& pServiceObj)
					{
						pServiceObj->SendMTUsers(
							head.DataProcessFlag,
							sk::packet((char*)&simple, shared::LENUSERSIMPLEFIELD));
					});
			}break;
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
			{
				auto pUser = (shared::UserField*)pak.data();
				Global::SharedGet()->m_MTUserQ.pop(pUser->Login, [](const auto&, auto&) {});

				Global::GetSetup()->m_SystemAssignLoginsPostOrderQ.iterate(
					[&](const auto&, auto& _users, const auto&, auto& itbreak)
					{
						_users.pop(pUser->Login);
					});

				shared::UserSimpleField simple;
				simple.Login = pUser->Login;
				sk::SafeCopyA(simple.Group, sk::StringConvert::WStringToMBytes(pUser->Group).c_str(), _countof(simple.Group));
				m_ServiceObjQ.search(pContext->Flag(),
					[&](const auto&, auto& pServiceObj)
					{
						pServiceObj->SendMTUsers(
							head.DataProcessFlag,
							sk::packet((char*)&simple, shared::LENUSERSIMPLEFIELD));
					});
			}break;
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
			{
				std::vector<shared::UserField> users;
				sk::Helper::OneUnknownParseStruct<shared::UserField>(pak.data(), pak.size(), users);
				for (const auto& user : users)
				{
					Global::SharedGet()->m_MTUserQ.push(user.Login, user);
				}
			}break;
			}///switch

		}break;
		default:
		{
			sk::packet userspak;
			Global::SharedGet()->m_MTUserQ.iterate(
				[&](const auto&, auto& userField)
				{
					shared::UserSimpleField simple;
					simple.Login = userField.Login;
					sk::SafeCopyA(simple.Group, sk::StringConvert::WStringToMBytes(userField.Group).c_str(), _countof(simple.Group));
					userspak.append((char*)&simple, shared::LENUSERSIMPLEFIELD);
				});
			m_ServiceObjQ.search(pContext->Flag(), [&](const auto&, auto& pServiceObj) {pServiceObj->SendMTUsers(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT, userspak); });
		}break;
		}///switch
	}
	void Server::OnNetSystemAssignLoginsPostOrderProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.DataProcessFlag)
		{
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
			[[fallthrough]];
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
		{
			if (pak.empty() || head.DataExtern.ExternDataU64 <= 0)
				break;
			auto pLogin = (UINT64*)pak.data();
			auto result = Global::GetSetup()->SystemAssignLoginsPostOrderInsert(head.DataExtern.ExternDataU64, *pLogin);
			m_ServiceObjQ.iterate(
				[&](const auto&, auto& pServiceObj)
				{
					switch (pServiceObj->ServiceObjFlagGet())
					{
					case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
					{
						pServiceObj->SendSystemAssignLoginsPostOrder(\
							head.DataExtern.ExternDataU64, \
							head.DataProcessFlag, \
							result ? pak : "");
					}break;
					case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
					{
						pServiceObj->SendSystemAssignLoginsPostOrder();
					}break;
					default:
					{
						
					}break;
					}///switch			
				});
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
		{
			if (pak.empty() || head.DataExtern.ExternDataU64 <= 0)
				break;
			auto pLogin = (UINT64*)pak.data();
			auto result = Global::GetSetup()->SystemAssignLoginsPostOrderDelete(*pLogin);

			m_ServiceObjQ.iterate(
				[&](const auto&, auto& pServiceObj)
				{
					switch (pServiceObj->ServiceObjFlagGet())
					{
					case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
					{
						pServiceObj->SendSystemAssignLoginsPostOrder(\
							head.DataExtern.ExternDataU64, \
							head.DataProcessFlag, \
							result ? pak : "");
					}break;
					case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
					{
						pServiceObj->SendSystemAssignLoginsPostOrder();
					}break;
					default:
					{

					}break;
					}///switch			
				});
		}break;
		case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
		{
			switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
			{
			case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
			{
				std::vector<UINT64> logins;
				Global::GetSetup()->SystemAssignLoginsPostOrderSelect(head.DataExtern.ExternDataU64, logins);

				m_ServiceObjQ.search(
					pContext->Flag(),
					[&](const auto&, auto& pServiceObj)
					{
						pServiceObj->SendSystemAssignLoginsPostOrder(\
							head.DataExtern.ExternDataU64, \
							head.DataProcessFlag, \
							logins.empty() ? "" : sk::packet((char*)&logins[0], sizeof(UINT64) * logins.size()));
					});
			}break;
			default:
			{

			}break;
			}///switch
		}break;
		}///switch

		//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
	}
	void Server::OnNetSystemPositionProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
		{
			m_ServiceObjQ.iterate(
				[&](const auto&, auto& pServiceObj)
				{
					if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
					{
						ServiceObjType<AdminObj>(pServiceObj)->SendSystemPositions(head.DataExtern, pak);
					}
				});
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
		{
			m_ServiceObjQ.iterate(
				[&](const auto&, auto& pServiceObj, auto& itbreak)
				{
					if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER == pServiceObj->ServiceObjFlagGet())
					{
						ServiceObjType<MTDealerObj>(pServiceObj)->RequestSystemPositionGet(head.DataExtern);
						itbreak = true;
					}
				});
		}break;
		default:
		{

		}break;
		}
	}
	void Server::OnNetMTSymbolsProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
			[[fallthrough]];
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
		{
			if (pak.empty())
				break;
			switch (head.DataProcessFlag)
			{
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
				[[fallthrough]];
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
			{
				std::vector<shared::ConSymbolField> symbols;
				sk::Helper::OneUnknownParseStruct<shared::ConSymbolField>(pak.data(), pak.size(), symbols);
				for (const auto& symbol : symbols)
				{
					Global::SharedGet()->m_MTSymbolQ.push(symbol.Symbol, symbol);
				}
			}break;
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
			{
				std::vector<shared::ConSymbolField> symbols;
				sk::Helper::OneUnknownParseStruct<shared::ConSymbolField>(pak.data(), pak.size(), symbols);
				for (const auto& symbol : symbols)
				{
					Global::SharedGet()->m_MTSymbolQ.pop(
						symbol.Symbol,
						[&](const auto& symbol, auto&)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("删除 MT符号({})", sk::StringConvert::WStringToMBytes(symbol));
						});
				}
			}break;
			default:
			{
			}break;
			}///switch
		}break;
		default:
		{
			sk::packet symbols;
			Global::SharedGet()->m_MTSymbolQ.iterate([&](const auto&, auto& symbol) {symbols.append((char*)&symbol, shared::LENCONSYMBOLFIELD); });
			m_ServiceObjQ.search(
				pContext->Flag(),
				[&](const auto&, auto& pServiceObj)
				{
					pServiceObj->SendMTSymbols(symbols);
				});
		}break;
		}///switch
	}

	void Server::OnNetTradeRecordHeadProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		auto vhead = SystemGet()->RecordGet()->HeadStr();
		sk::packet shead;
		for (const auto& node : vhead)
		{
			shead.append(node).append("\t");
		}
		if (!shead.empty()) shead.pop_back();

		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, IServiceObj* pServiceObj)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordHead(shead);
				}
			});
	}

	void Server::OnNetExchangeOnlineProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet exchanges;
		m_ServiceObjQ.iterate(
			[&](const auto& flag, IServiceObj* pServiceObj)
			{
				if (pServiceObj->ServiceObjFlagGet() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER)
				{
					Global::GetSetup()->m_ClientsMapQ.search(
						flag,
						[&](const auto&, auto& filed)
						{
							exchanges.append((char*)&filed, shared::LENLOGINFIELD);
						});
				}
			});
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, IServiceObj* pServiceObj)
			{
				ServiceObjType<AdminObj>(pServiceObj)->SendExchangeOnline(exchanges);
			});
	}

	void Server::OnNetTradeRecordUpdateProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		auto logs = SystemGet()->RecordGet()->PopNewest();
		sk::packet records;
		if (head.DataExtern.ExternDataI > 0)
		{
			size_t count = 0;
			for (auto rit = logs.rbegin(); rit != logs.rend(); ++rit)
			{
				++count;
				records.append(*rit).append("@");
				if (count >= head.DataExtern.ExternDataI)
					break;
			}
		}

		if (!records.empty()) records.pop_back();

		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordUpdate(records);
				}
			});
	}

	void Server::OnNetProductConProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())) == \
			shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
		{
			switch (head.DataProcessFlag)
			{
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
				[[fallthrough]];
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
			{
				if (head.DataExtern.ExternDataU64 <= 0)
					break;

				shared::SystemProduct sysProd;
				sk::container::set<shared::LPProduct> lpProds;
				sk::container::set<shared::MTFeederProduct> feederProds;
				sk::container::set<shared::MTGatewayProduct> gatewayProds;

				bool update = false;
				if (!SymbolConGet()->Push(
					sk::packet(pak.data(), pak.size()),
					sysProd,
					lpProds,
					feederProds,
					gatewayProds))
				{
					if (!SymbolConGet()->Finalize(sysProd.ID))
					{
						update = true;
					}
				}

				if (!update)
				{
					sk::network::Header head_(head);
					head_.Retcode = static_cast<decltype(head_.Retcode)>(SKAPIRES::PROXY_RET_ERROR);
					NetApi<sk::network::INetworkApi>()->WriteTo(pContext, head_, "");
					Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("更新产品配置({:X},({}))失败", head.DataExtern.ExternDataU64, sysProd.Symbol.symbol);
					break;
				}
				else
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("更新产品配置({:X},({}))完成", head.DataExtern.ExternDataU64, sysProd.Symbol.symbol);
				}

				m_ServiceObjQ.iterate(
					[&](const auto& Login, IServiceObj* pServiceObj)
					{
						switch (pServiceObj->ServiceObjFlagGet())
						{
						case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
						{
							ServiceObjType<AdminObj>(pServiceObj)->SendProductCon(
								pak,
								head.DataExtern.ExternDataU64,
								1,
								SKAPIRES::PROXY_RET_SUCCESS);
						}break;
						case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
						{
							ServiceObjType<MTDataFeedObj>(pServiceObj)->SendProducts(feederProds.Vector());
						}break;
						case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
						{
							ServiceObjType<MTDealerObj>(pServiceObj)->SendProducts(gatewayProds.Vector());
						}break;
						case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
						{
							ServiceObjType<LPMarketDataObj>(pServiceObj)->SendProducts(lpProds.Vector());
						}break;
						case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
						{
							ServiceObjType<LPTradeOrderObj>(pServiceObj)->SendProducts(lpProds.Vector());
						}break;
						default:
							break;
						}///switch
					});
			}break;
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
			{
				auto pAdminObj = ServiceObjType<AdminObj>(*m_ServiceObjQ.search(pContext->Flag()));
				if (!pAdminObj)
					break;
				if (head.DataExtern.ExternDataU64 > 0)
				{
					auto pConBuf = SymbolConGet()->GetSystemSymbolConBuffer(shared::SystemIDType::SystemProductType(head.DataExtern.ExternDataU64));
					if (pConBuf && !pConBuf->empty())
					{
						pAdminObj->SendProductCon(
							*pConBuf,
							head.DataExtern.ExternDataU64,
							1,
							SKAPIRES::PROXY_RET_SUCCESS);
					}
					else
					{
						pAdminObj->SendProductCon(
							"",
							head.DataExtern.ExternDataU64,
							1,
							SKAPIRES::PROXY_RET_ERROR);
					}
				}
				else
				{
					std::vector<shared::SystemID> reqSysIDs;
					if (pak.empty())
					{
						auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetSystemProducts();
						for (const auto& prod : prods)
							reqSysIDs.emplace_back(prod.ID);
					}
					else
						sk::Helper::OneUnknownParseStruct<shared::SystemID>(pak.data(), pak.size(), reqSysIDs);
					if (reqSysIDs.empty())
						break;
					for (auto it = reqSysIDs.begin(); it != reqSysIDs.end(); ++it)
					{
						sk::packet repCon;
						auto pConBuf = SymbolConGet()->GetSystemSymbolConBuffer(shared::SystemIDType::SystemProductType(*it));
						if (pConBuf && !pConBuf->empty())
							repCon = *pConBuf;
						pAdminObj->SendProductCon(
							repCon,
							*it,
							INT(reqSysIDs.size()),
							repCon.empty() ? SKAPIRES::PROXY_RET_ERROR : SKAPIRES::PROXY_RET_SUCCESS);
					}
				}
			}break;
			case sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
			{

			}break;
			default:
			{
			}break;
			}///switch
		}
		else
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}({})->({})", __FUNCTION__, "产品配置只针对管理客户端的业务", pContext->IPAddr());
		}
	}
	void Server::OnNetProductsProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				switch (pServiceObj->ServiceObjFlagGet())
				{
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
				{
					auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetSystemProducts();
					ServiceObjType<AdminObj>(pServiceObj)->SendProducts(prods);
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
				{
					auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetLPProducts(pContext->Flag());
					ServiceObjType<LPTradeOrderObj>(pServiceObj)->SendProducts(prods);
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
				{
					auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetLPProducts(pContext->Flag());
					ServiceObjType<LPMarketDataObj>(pServiceObj)->SendProducts(prods);
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
				{
					auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetFeederProducts(pContext->Flag());
					ServiceObjType<MTDataFeedObj>(pServiceObj)->SendProducts(prods);
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
				{
					auto prods = Global::GetCore()->GetSystem()->SymbolConGet()->GetGatewayProducts(pContext->Flag());
					ServiceObjType<MTDealerObj>(pServiceObj)->SendProducts(prods);
				}break;
				default:
				{
				}break;
				}
			});
	}

	void Server::OnNetExchangePositionProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
		{
			std::vector<shared::tradeorder::ExchangePosition> positions;
			sk::Helper::OneUnknownParseStruct<shared::tradeorder::ExchangePosition>(pak.data(), pak.size(), positions);
			if (positions.empty()) break;

			LPTradeOrderObj* pTarget = nullptr;
			m_ServiceObjQ.search(pContext->Flag(), [&](const auto&, auto& pServiceObj) {pTarget = ServiceObjType<LPTradeOrderObj>(pServiceObj); });
			if (!pTarget)
				break;

			switch (head.DataProcessFlag)
			{
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
			{
				for (auto& position : positions)
				{
					pTarget->m_PositionQ.push(position.ID, position);
				}
				//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("载入交易所持仓({})来自({:X})", pTarget->m_PositionQ.size(), pContext->Flag());
			}break;
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
			{
				for (auto& position : positions)
				{
					pTarget->m_PositionQ.push(position.ID, position);
				}

				m_ServiceObjQ.iterate(
					[&](const auto&, auto& pServiceObj)
					{
						if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
						{
							ServiceObjType<AdminObj>(pServiceObj)->\
								SendExchangePositions(sk::packet((char*)&positions[0], shared::tradeorder::LENEXCHANGEPOSITION * positions.size()), pContext->Flag(), head.DataProcessFlag);
						}
					});

			}break;
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE:
			{
				for (auto& position : positions)
				{
					pTarget->m_PositionQ.pop(position.ID, [](const auto&, auto&) {});
				}

				m_ServiceObjQ.iterate(
					[&](const auto&, auto& pServiceObj)
					{
						if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
						{
							ServiceObjType<AdminObj>(pServiceObj)->\
								SendExchangePositions(sk::packet((char*)&positions[0], shared::tradeorder::LENEXCHANGEPOSITION * positions.size()), pContext->Flag(), head.DataProcessFlag);
						}
					});
			}break;
			}///switch

		}break;
		default:
		{
			sk::packet positions;
			m_ServiceObjQ.search(
				head.DataExtern.ExternDataU64,
				[&](const auto&, IServiceObj* pServiceObj)
				{
					if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER == pServiceObj->ServiceObjFlagGet())
					{
						ServiceObjType<LPTradeOrderObj>(pServiceObj)->m_PositionQ.iterate(
							[&](const auto&, auto& position)
							{
								positions.append((char*)&position, shared::tradeorder::LENEXCHANGEPOSITION);
							});
					}
				});

			m_ServiceObjQ.search(
				pContext->Flag(),
				[&](const auto&, auto& pServiceObj)
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendExchangePositions(positions, head.DataExtern.ExternDataU64, sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT);
				});
		}break;
		}///switch
	}

	void Server::OnNetMarketDataCache(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
		{
			auto quotes = sk::Helper::OneUnknownParseStruct<shared::marketdata::MarketData>(pak.data(), pak.size());
			if (!quotes || quotes->empty()) break;

			m_ServiceObjQ.search(
				pContext->Flag(),
				[&](const auto&, auto& pServiceObj)
				{
					for (auto& quote : *quotes)
					{
						ServiceObjType<LPMarketDataObj>(pServiceObj)->m_MarketDataQ.push(quote.ID, quote);

						auto sysProdID = SymbolConGet()->GetSystemProductID(quote.Symbol);
						if (sysProdID > 0)
						{
							quote.ID = sysProdID | quote.ID | shared::SystemIDType::ExchangeType(pContext->Flag());
							Global::SharedGet()->m_MarketDataQ.push(quote.ID, quote);
						}
					}

					//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("获取报价映像({})来自({:X})", ServiceObjType<LPMarketDataObj>(pServiceObj)->m_MarketDataQ.size(), pContext->Flag());
				});
		}break;
		default:
		{
			sk::packet mds;
			Global::SharedGet()->m_MarketDataQ.iterate(
				[&](const auto&, auto& md)
				{
					mds.append((char*)&md, shared::marketdata::LENMARKETDATA);
				});
			m_ServiceObjQ.search(
				pContext->Flag(),
				[&](const auto&, auto& pServiceObj)
				{
					pServiceObj->SendMarketDataCache(mds);
				});
		}break;
		}///switch
	}
	void Server::OnNetMarketDataUpdate(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		//!@ 只接受来自LP报价接口的报价更新
		if (shared::SystemIDType::ServiceObjectType(pContext->Flag()) != (shared::SystemID)shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA)
			return;
		std::vector<shared::marketdata::MarketData> quotes;
		sk::Helper::OneUnknownParseStruct<shared::marketdata::MarketData>(pak.data(), pak.size(), quotes);
		if (quotes.empty())
			return;

		LPMarketDataObj* pServiceTarget = nullptr;
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto& serviceObjID, IServiceObj* pServiceObj)
			{
				pServiceTarget = ServiceObjType<LPMarketDataObj>(pServiceObj);
			});

		if (pServiceTarget)
		{
			pServiceTarget->MDUpdateTimestamp(sk::Helper::TickCountGet<std::chrono::milliseconds>());
		}

		for (auto& quote : quotes)
		{
			if (pServiceTarget)
			{
				pServiceTarget->m_MarketDataQ.push(quote.ID, quote);
			}

			auto sysProdID = SymbolConGet()->GetSystemProductID(quote.Symbol);
			if (sysProdID > 0)
			{
				quote.ID = sysProdID | quote.ID | shared::SystemIDType::ExchangeType(pContext->Flag());
				Global::SharedGet()->m_MarketDataUpdateQ.push(quote);
				Global::SharedGet()->m_MarketDataQ.push(quote.ID, quote);
			}
		}
	}
	void Server::OnNetProductsGetProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto& serviceObjID, IServiceObj* pServiceObj)
			{
				switch (pServiceObj->ServiceObjFlagGet())
				{
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
				{
					if (SKAPIRES_FAILED((SKAPIRES)head.Retcode))
						break;
					auto products = sk::Helper::OneUnknownParseStruct<shared::LPProduct>(pak.data(), pak.size());
					if (!products)
						break;
					for (auto it = products->begin(); it != products->end(); ++it)
					{
						ServiceObjType<LPTradeOrderObj>(pServiceObj)->m_ProductQ.push(it->ID, *it);
						Global::SharedGet()->m_ExchangeProductQ.pushpush(
							shared::SystemIDType::ExchangeType(pContext->Flag()),
							[&](auto& obj)
							{
								obj.push(it->ID, *it);
							});
					}

					Global::SharedGet()->m_ExchangeProductQ.search(
						pContext->Flag(),
						[&](const auto&, auto& obj)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("加入LP合约产品({})来自(({:X}))", obj.size(), pContext->Flag());
						});
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
				{
					if (SKAPIRES_FAILED((SKAPIRES)head.Retcode))
						break;
					auto products = sk::Helper::OneUnknownParseStruct<shared::LPProduct>(pak.data(), pak.size());
					if (!products)
						break;
					for (auto it = products->begin(); it != products->end(); ++it)
					{
						ServiceObjType<LPMarketDataObj>(pServiceObj)->m_ProductQ.push(it->ID, *it);
						Global::SharedGet()->m_ExchangeProductQ.pushpush(
							shared::SystemIDType::ExchangeType(pContext->Flag()),
							[&](auto& obj)
							{
								obj.push(it->ID, *it);
							});
					}

					Global::SharedGet()->m_ExchangeProductQ.search(
						pContext->Flag(),
						[&](const auto&, auto& obj)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("加入LP合约产品({})来自(({:X}))", obj.size(), pContext->Flag());
						});
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
				{
					if (SKAPIRES_FAILED((SKAPIRES)head.Retcode))
						break;
					auto products = sk::Helper::OneUnknownParseStruct<shared::MTFeederProduct>(pak.data(), pak.size());
					if (!products)
						break;
					for (auto it = products->begin(); it != products->end(); ++it)
					{
						Global::SharedGet()->m_MTDataFeedProductsQ.pushpush(
							pContext->Flag(),
							[&](auto& obj)
							{
								obj.push(it->ID, *it);
							});
					}
					Global::SharedGet()->m_MTDataFeedProductsQ.search(
						pContext->Flag(),
						[&](const auto&, auto& obj)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("加入MT合约产品({})来自(({:X}))", obj.size(), pContext->Flag());
						});
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
				{
					if (SKAPIRES_FAILED((SKAPIRES)head.Retcode))
						break;
					auto products = sk::Helper::OneUnknownParseStruct<shared::MTGatewayProduct>(pak.data(), pak.size());
					if (!products)
						break;
					for (auto it = products->begin(); it != products->end(); ++it)
					{
						Global::SharedGet()->m_MTGatewayProductsQ.pushpush(
							pContext->Flag(),
							[&](auto& obj)
							{
								obj.push(it->ID, *it);
							});
					}
					Global::SharedGet()->m_MTGatewayProductsQ.search(
						pContext->Flag(),
						[&](const auto&, auto& obj)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("加入MT合约产品({})来自(({:X}))", obj.size(), pContext->Flag());
						});
				}break;
				}
			});
	}
	void Server::OnNetSystemTimeProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				pServiceObj->SendSystemTime();
			});
	}
	void Server::OnNetSystemSetupProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		volatile bool Update = false;
		m_ServiceObjQ.search(
			pContext->Flag(),
			[&](const auto&, IServiceObj* pServiceObj)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlagGet())
				{
					shared::BridgeSystemSetup* setup = (shared::BridgeSystemSetup*)pak.data();
					if (setup)
					{
						Global::GetSetup()->SystemSetupSet(*setup);
						Update = true;
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
					}
				}
			});
		if (!Update) return;
		m_ServiceObjQ.iterate(
			[&](const auto&, IServiceObj* clientObj)
			{
				clientObj->SendBridgeSystemSetup(Global::GetSetup()->SystemSetupGet());
			});
	}
	void Server::OnNetLoginProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
		{
			pContext->Closed();
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("Rogue Client Kick At ({})!", pContext->IPAddr());
			return;
		}

		auto pLoginField = (shared::BridgeLoginField*)pak.data();
		auto pServiceObj = std::make_shared<ServiceObj>(NetApi<sk::network::INetworkApi>(), pContext);

		SKAPIRES LoginResult = SKAPIRES::PROXY_RET_SUCCESS;

		Global::GetSetup()->m_ClientsMapQ.search(
			pLoginField->ID,
			[&](const auto&, shared::BridgeLoginField& clientField)
			{
				if (sk::SafeCmpA(clientField.Password, pLoginField->Password, _countof(clientField.Password)))
				{
					LoginResult = SKAPIRES::PROXY_RET_ERROR_LOGIN_PASSWORD;
				}

				if (SKAPIRES::PROXY_RET_SUCCESS == LoginResult)
				{
					m_ServiceObjQ.iterate(
						[&](const auto& login, auto& serviceObj, const auto&, auto& itbreak)
						{
							if (login == pLoginField->ID)
							{
								itbreak = true;
								LoginResult = SKAPIRES::PROXY_RET_ERROR_LOGIN_REPEAT;
							}
						});
				}
			});

		if (!SKAPIRES_SUCCESS(LoginResult))
		{
			pServiceObj->SendLoginResult(LoginResult);
			pContext->Closed();
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{:X}({})({})!", pLoginField->ID, pContext->IPAddr(), SKAPIRES_DESC(LoginResult));
			return;
		}

		pContext->Flag(pLoginField->ID);

		IServiceObj* pTargetClient = nullptr;
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pLoginField->ID)))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
		{
			pTargetClient = new LPMarketDataObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;

			m_ServiceObjQ.push(pTargetClient->LoginFlag(), pTargetClient);
			pTargetClient->ProductsRequest();
			//!@ 请求报价缓冲
			/*ServiceObjType<LPMarketDataObj>(pTargetClient)->MarketDataCacheRequest();*/
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER: {
			pTargetClient = new LPTradeOrderObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;

			m_ServiceObjQ.push(pTargetClient->LoginFlag(), pTargetClient);
			pTargetClient->ProductsRequest();
			//!@ 请求持仓缓冲
			/*ServiceObjType<LPTradeOrderObj>(pTargetClient)->PositionRequest();*/
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED: {
			pTargetClient = new MTDataFeedObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;

			m_ServiceObjQ.push(pTargetClient->LoginFlag(), pTargetClient);
			pTargetClient->ProductsRequest();
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER: {
			pTargetClient = new MTDealerObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;

			m_ServiceObjQ.push(pTargetClient->LoginFlag(), pTargetClient);
			pTargetClient->ProductsRequest();
			ServiceObjType<MTDealerObj>(pTargetClient)->RequestMTUserGet();
			ServiceObjType<MTDealerObj>(pTargetClient)->RequestMTSymbolGet();
			ServiceObjType<MTDealerObj>(pTargetClient)->SendSystemAssignLoginsPostOrder();
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN: {
			pTargetClient = new AdminObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;

			m_ServiceObjQ.push(pTargetClient->LoginFlag(), pTargetClient);
			ServiceObjType<AdminObj>(pTargetClient)->SendSystemAssignLoginsPostOrder();
		}break;
		default:
		{

		}break;
		}///switch

		if (SKAPIRES_SUCCESS(LoginResult) && pTargetClient)
		{
			pServiceObj->SendLoginResult(SKAPIRES::PROXY_RET_SUCCESS);
			pTargetClient->SendSystemTime();
			pTargetClient->SendBridgeSystemSetup(Global::GetSetup()->SystemSetupGet());
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("业务模块(({:X})({})({}))接入.", pTargetClient->LoginFlag(), pLoginField->Username, pContext->IPAddr());

			//!@ 准备队列--标记位
			Global::GetSetup()->m_ReadyContextMapQ.search(pContext->Flag(), 
				[&](const auto& id, auto& status) 
				{
					status = true; 
				});
		}
		else
		{
			pServiceObj->SendLoginResult(SKAPIRES::PROXY_RET_ERROR);
			pContext->Closed();
		}

	}
































	System* Server::SystemGet() const {
		if (GetGlobal()->GetCore()) {
			return GetGlobal()->GetCore()->GetSystem();
		}
		return nullptr;
	}
	SymbolCon* Server::SymbolConGet() const {
		if (Global::GetCore() && Global::GetCore()->GetSystem()) {
			return Global::GetCore()->GetSystem()->SymbolConGet();
		}
		return nullptr;
	}
	QuoteFinal* Server::QuoteFinalGet() const {
		if (Global::GetCore() && Global::GetCore()->GetSystem()) {
			return Global::GetCore()->GetSystem()->FinalQuoteGet();
		}
		return nullptr;
	}

	SKAPIRES Server::NetTradeOrderRequest(const shared::SystemOrder& OrderRequest)
	{
		SKAPIRES result = SKAPIRES::PROXY_RET_ERROR;
		m_ServiceObjQ.iterate(
			[&](const auto& ServiceObjID, IServiceObj* pServiceObj, const auto&, auto& itbreak)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER == pServiceObj->ServiceObjFlagGet() && \
					shared::SystemIDType::ExchangeType(ServiceObjID) == shared::SystemIDType::ExchangeType(OrderRequest.ExchangePID))
				{
					if (!ServiceObjType<LPTradeOrderObj>(pServiceObj)->TradeOrderRequest(OrderRequest))
					{
						result = SKAPIRES::PROXY_RET_SUCCESS;
					}
					itbreak = true;
				}
			});
		return result;
	}

	SKAPIRES Server::NetTradeOrderResSend(const shared::SystemOrder& OrderResult)
	{
		SKAPIRES result = SKAPIRES::PROXY_RET_ERROR;
		m_ServiceObjQ.search(
			OrderResult.ClientID,
			[&](const auto&, auto& pClient)
			{
				auto pTargetClient = (MTDealerObj*)pClient;
				if (0 == pTargetClient->TradeOrderResSend(OrderResult))
				{
					result = SKAPIRES::PROXY_RET_SUCCESS;
				}
			});
		return result;
	}

	void Server::OnNetTradeOrderProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
		{
			std::vector<shared::SystemOrder> orders;
			sk::Helper::OneUnknownParseStruct<shared::SystemOrder>(pak.data(), pak.size(), orders);
			if (orders.empty())
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("订单请求不合法的数据包({}.{})", pak.size(), pak.data());
				break;
			}
			//Global::SharedGet()->m_OrderReqQ.push(orders);
			SystemGet()->OrderDispatchGet()->Req(orders);
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
		{
			std::vector<shared::SystemOrder> orders;
			sk::Helper::OneUnknownParseStruct<shared::SystemOrder>(pak.data(), pak.size(), orders);
			if (orders.empty())
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("订单回执不合法的数据包({}.{})", pak.size(), pak.data());
				break;
			}
			SystemGet()->OrderDispatchGet()->Res(orders);
			//Global::SharedGet()->m_OrderRepQ.push(orders);
		}break;
		}///switch
	}


	void Server::OnNetLogpakProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;

		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("({:X})({})", pContext->Flag(), sk::packet(pak.data(), pak.size()));

		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
		{
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
		{
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
		{
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
		{
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
		{
		}break;
		default:
		{
		}break;
		}///switch
	}


	void Server::OnNetOrderFailedProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
		{
			//! this is failed order task...
			auto order = (shared::SystemOrder*)pak.data();
			



		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
		{


		}break;
		}///switch

	}
	void Server::OnNetOrderTimeoutProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
#if 0
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
		{
			auto pOrder = (shared::SystemOrder*)pak.data();
			Global::SharedGet()->m_GatewayOrderTimeoutQ.push(pOrder->MTRequestID, *pOrder);
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
		{
			auto pOrder = (shared::SystemOrder*)pak.data();
			Global::SharedGet()->m_GatewayOrderTimeoutQ.pop(
				pOrder->MTRequestID,
				[&](const auto&, shared::SystemOrder& timeout_order)
				{
					if (SKAPIRES_SUCCESS(timeout_order.SKRetcode))
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn(
							"网关超时超时定单移除完成(GatewayPID({:X}),ExchangePID({}),LPTradeQty({}))",
							pOrder->GatewayProductID,
							pOrder->ExchangeProdID,
							pOrder->LPTradeQty
						);
					}
					else
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->error(
							"网关超时超时定单移除失败(GatewayPID({:X}),ExchangePID({}),LPTradeQty({}),SKRetcode({}))",
							pOrder->GatewayProductID,
							pOrder->ExchangeProdID,
							pOrder->LPTradeQty,
							SKAPIRES_DESC(pOrder->SKRetcode)
						);
					}
				});
		}break;
		}///switch
#endif
	}

}///namespace proxy