#include "stdafx.h"
#include "Dealer.h"

namespace dealer {

	void Dealer::DealerLockT()
	{
		std::set<shared::Serial> triggers;
		auto Requests = ServerApi()->TradeRequestCreateArray();

		do {

			switch (sk::LicenseInfo::EnProductVersionType(LicenseGet()))
			{
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_BETA:
				[[fallthrough]];
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_DEMO:
				[[fallthrough]];
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_FINAL:
				[[fallthrough]];
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_TRIAL:
				[[fallthrough]];
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_OFFICIAL:
			{

			}break;
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_INVALID:
				[[fallthrough]];
			default:
			{
				Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(SKAPIRES_DESC(SKAPIRES::LICENSE_RET_ERROR_VERIFY_FAILED));
				return;
			}break;
			}///switch



			auto& SystemSetup = SetupGet()->BridgeSystemSetup();
			Requests->Clear();
			std::shared_ptr<DealerOrder> pDealerOrder;
			if (MT_RET_OK != ServerApi()->DealerRequestGetAll(m_DealerManagerLogin.load(), Requests) || Requests->Total() <= 0)
			{
				if (!m_PluginReady.load()) { break; }
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}

			for (UINT i = 0; i < Requests->Total(); ++i)
			{
				auto temp = Requests->Next(i);

				m_pRequest->Clear();
				if (MT_RET_OK != ServerApi()->DealerLock(m_DealerManagerLogin.load(), temp->ID(), m_pRequest))
				{
					continue;
				}

				shared::TradeTaskRouteGateway RouteCheck;
				RouteCheck.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();
				shared::Business::MakeRequestField(m_pRequest, RouteCheck.MTRequest);
				SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.push(m_pRequest->ID(), RouteCheck);


				if (!ExchangeGet() || !ExchangeGet()->Ready())
				{
					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info("{}", "交易所设备还没有准备好.");
					continue;
				}

				//!@ 根据定单的类型做相应处理
				if (m_pRequest->ResultOrder() <= 0)
				{
					QuickConfirm(m_pRequest, MT_RET_REQUEST_DONE);
					continue;
				}

				switch (m_pRequest->Type())
				{
				case IMTOrder::EnOrderType::OP_BUY:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_SELL:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_BUY_LIMIT:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_SELL_LIMIT:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_CLOSE_BY:
				{

				}break;
				case IMTOrder::EnOrderType::OP_BUY_STOP:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_SELL_STOP:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
					[[fallthrough]];
				case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
				{
					QuickConfirm(m_pRequest, MT_RET_REQUEST_DONE);
					continue;
				}break;
				default:
				{
					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}break;
				}

				if ((m_pRequest->Flags() & IMTRequest::EnTradeActionFlags::TA_FLAG_CLOSE || \
					m_pRequest->Flags() & IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET) && \
					(m_pRequest->Action() == IMTRequest::EnTradeActions::TA_ACTIVATE_SL || \
						m_pRequest->Action() == IMTRequest::EnTradeActions::TA_ACTIVATE_TP)
					)
				{
					Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn(
						"激活 | 触发 定单(MTReqID({}),Symbol({}),Order({}),Position({}),Volume({}),PositionExternalID({}),Flags({:X}),Action({}),PriceOrder({}),PriceTrigger({}),PriceSL({}),PriceTP({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()),
						m_pRequest->ResultOrder(),
						m_pRequest->Position(),
						m_pRequest->Volume(),
						m_pRequest->PositionExternalID() ? sk::StringConvert::WStringToMBytes(m_pRequest->PositionExternalID()) : "",
						m_pRequest->Flags(),
						m_pRequest->Action(),
						m_pRequest->PriceOrder(),
						m_pRequest->PriceTrigger(),
						m_pRequest->PriceSL(),
						m_pRequest->PriceTP());

					QuickConfirm(m_pRequest, MT_RET_REQUEST_DONE);
				}


				shared::Instrument Symbol = sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()).c_str();

				auto GatewayProdcon = SharedGet()->m_MTSymbolProdconQ.search(Symbol);
				if (!GatewayProdcon)
				{
					Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
						"未找到网关产品配置(MTReqID({}),Symbol({}),SKRetcode({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()),
						SKAPIRES_DESC(SKAPIRES::GATEWAY_RET_ERROR_PRODUCTCON_NOTFOUND));

					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}

				if (!GatewayProdcon->EnableTrade)
				{
					Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn(
						"系统配置禁止交易(MTReqID({}),Symbol({}),SKRetcode({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()),
						SKAPIRES_DESC(SKAPIRES::GATEWAY_RET_ERROR_ORDERTASK_DISABLE));

					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}

				if (m_pRequest->Type() != IMTOrder::EnOrderType::OP_BUY && m_pRequest->Type() != IMTOrder::EnOrderType::OP_SELL)
				{
					Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
						"不支持的定单请求类型(MTRequest::Type)(MTReqID({}),Symbol({}),Type({}),Position({}),PositionExternalID({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()),
						m_pRequest->Type(),
						m_pRequest->Position(),
						sk::StringConvert::WStringToMBytes(m_pRequest->PositionExternalID()));

					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}

				shared::ConSymbolField* pMTSymbol = nullptr;
				SharedGet()->m_MTSymbolQ.search(m_pRequest->Symbol(), [&](const auto&, auto& symbol) {pMTSymbol = &symbol; });
				if (!pMTSymbol)
				{
					Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
						"未找到订单品种的Symbol配置(MTReqID({}),Symbol({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()));

					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}

				if (SharedGet()->m_RerequestCacheQ.pop(m_pRequest->ID(), [&](const auto&, auto& route) {}))
				{
					if (SystemSetup.DealerContinueToRequestSwitch == shared::BridgeSystemSetup::EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_DISABLE)
					{
						QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					}
					continue;
				}

				if (!pDealerOrder)
					pDealerOrder = std::make_shared<DealerOrder>(SharedGet()->OrderSerial());

				shared::TradeTaskRouteGateway Route;
				Route.LPRequest = shared::SystemOrder();
				Route.MTSymbol = *pMTSymbol;
				Route.GatewayProdcon = *GatewayProdcon;
				shared::Business::MakeRequestField(m_pRequest, Route.MTRequest);
				Route.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();

				switch (SystemSetup.SystemOrderMode)
				{
				case shared::BridgeSystemSetup::EnSystemOrderMode::EN_SYSTEM_ORDER_MODE_SINGLE:
				{
					if (!SystemSetup.OneByOneOrderAssignLoginCheck(m_pRequest->Login()))
					{
						Route.LPRequest.Key.MTLogin = m_pRequest->Login();
					}
				}break;
				case shared::BridgeSystemSetup::EnSystemOrderMode::EN_SYSTEM_ORDER_MODE_MULTIPLE:
				{
					if (SystemSetup.OneByOneOrderAssignLoginCheck(m_pRequest->Login()))
					{
						Route.LPRequest.Key.MTLogin = m_pRequest->Login();
					}
				}break;
				default:
				{
					Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
						"不支持的系统定单模式(MTReqID({}),Symbol({}))",
						m_pRequest->ID(),
						sk::StringConvert::WStringToMBytes(m_pRequest->Symbol()));

					QuickConfirm(m_pRequest, MT_RET_REQUEST_REJECT);
					continue;
				}break;
				}///switch

				Route.LPRequest.Key.GatewaySerial = pDealerOrder->Serial();
				Route.LPRequest.OrderID = shared::SystemIDType::WStrToSystemOrderID(m_pRequest->PositionExternalID() ? m_pRequest->PositionExternalID() : L"0");
				Route.LPRequest.Key.ExchangeID = shared::SystemIDType::SystemOrderIDMadeUn(Route.LPRequest.OrderID);
				Route.LPRequest.Key.OrdSide = (m_pRequest->Type() == IMTOrder::EnOrderType::OP_BUY ? shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY : shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL);
				Route.LPRequest.Key.SystemPID = shared::SystemIDType::SystemProductType(GatewayProdcon->ID);
				if (Route.LPRequest.Key.ExchangeID <= 0)
				{
					//!@ 指定交易所抛单
					SharedGet()->m_SystemAssignLoginsPostOrderQ.iterate(
						[&](const auto& exchange, auto& logins, const auto&, auto& itbreak)
						{
							if (logins.search(m_pRequest->Login()))
							{
								Route.LPRequest.Key.MTLogin = m_pRequest->Login();
								Route.LPRequest.Key.ExchangeID = exchange;
								itbreak = true;
							}
						});
				}

				Route.LPRequest.GatewayPID = GatewayProdcon->ID;
				Route.LPRequest.ClientID = SetupGet()->BridgeLoginField().ID;
				Route.LPRequest.OrdTimeInForce = SystemSetup.DealerOrdTimeInForce;
				Route.LPRequest.OrdType = SystemSetup.DealerOrdType;
				Route.LPRequest.MTOrderPx = m_pRequest->PriceOrder();
				Route.LPRequest.MTOrderQty = m_pRequest->Volume();
				Route.LPRequest.PositionEffect = shared::tradeorder::EnPositionEffect::EN_POSITION_EFFECT_OPEN;
				pDealerOrder->Push(Route);
			}///for


			if (pDealerOrder)
			{
				triggers.insert(pDealerOrder->Serial());
				SharedGet()->m_DealerOrderQ.push(pDealerOrder->Serial(), pDealerOrder);
			}


			for (const auto& trigger : triggers)
			{
				SharedGet()->m_DealerOrderQ.search(
					trigger,
					[&](const auto&, auto& pDealerOrder)
					{
						std::vector<shared::SystemOrder> orders;
						pDealerOrder->Packet(orders);
						if (!orders.empty())
						{
							ExchangeGet()->NetTradeOrderRequest(sk::packet((char*)&orders[0], shared::LENSYSTEMORDER * orders.size()));
						}
					});
			}

			triggers.clear();

			if (!m_PluginReady.load()) { break; }
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} while (1);

		MTRelease(Requests);
	}

	void Dealer::ProcessT()
	{
		do {
			auto& SystemSetup = SetupGet()->BridgeSystemSetup();
			auto currentTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			auto pOrderRes = SharedGet()->m_OrderResQ.pop();
			if (pOrderRes)
			{
				SharedGet()->m_DealerOrderQ.search_clear(
					pOrderRes->Key.GatewaySerial,
					[&](auto& pDealerOrder, auto& itclear)
					{
						std::vector<shared::ConfirmField> confirms;
						std::vector<shared::TradeTaskRouteGateway> routes;
						itclear = pDealerOrder->Response(*pOrderRes, confirms, routes);
						for (const auto& confirm : confirms)
						{
							QuickConfirm(confirm, confirm.Retcode);

							SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.pop(confirm.ID, [&](const auto&,auto&) {});
						}
						//!@ 开关
						if (SystemSetup.DealerContinueToRequestSwitch == \
							shared::BridgeSystemSetup::EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_DISABLE)
						{
							for (const auto& route : routes)
							{
								QuickConfirm(route.MTConfirm, route.MTConfirm.Retcode);

								SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.pop(route.MTConfirm.ID, [&](const auto&, auto&) {});
							}
						}
						else
						{
							std::shared_ptr<DealerOrder> pNewDealerOrder;
							for (auto& route : routes)
							{
								if (!pNewDealerOrder)
								{
									pNewDealerOrder = std::make_shared<DealerOrder>(SharedGet()->OrderSerial());
									pNewDealerOrder->UpdateTime(sk::Helper::TickCountGet<std::chrono::milliseconds>());
								}
								route.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();
								route.LPRequest.MTOrderQty = route.MTRequest.Volume;
								route.LPRequest.Key.GatewaySerial = pNewDealerOrder->Serial();
								pNewDealerOrder->Push(route);
							}
							if (pNewDealerOrder)
							{
								SharedGet()->m_DelayDealerOrderQ.push(pNewDealerOrder->Serial(), pNewDealerOrder);
							}
						}
					});

			}

			if (SystemSetup.DealerContinueToRequestSwitch == \
				shared::BridgeSystemSetup::EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_ENABLE)
			{
				SharedGet()->m_DelayDealerOrderQ.iterate_clear(
					[&](const auto& serial, auto& pDealerOrder, auto&, auto& itclear)
					{
						if (currentTime - pDealerOrder->UpdateTime() > SystemSetup.DealerContinueToRequestDelay)
						{
							SharedGet()->m_DealerOrderQ.push(pDealerOrder->Serial(), pDealerOrder);
							SharedGet()->m_DealerOrderQ.search(
								pDealerOrder->Serial(),
								[&](const auto&, auto& pDealerOrder)
								{
									std::vector<shared::SystemOrder> orders;
									pDealerOrder->Packet(orders);
									if (!orders.empty())
									{
										ExchangeGet()->NetTradeOrderRequest(sk::packet((char*)&orders[0], shared::LENSYSTEMORDER * orders.size()));
									}
								});
							itclear = true;
						}
					});
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_PluginReady.load()) break;
		} while (1);
	}

	void Dealer::ProcessCheckT()
	{
		time_t ProxyTimeOld = 0;
		do {
			time_t TimeCurrent = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.iterate_clear(
				[&](const auto& reqid, shared::TradeTaskRouteGateway& route, auto& itbreak, auto& itclear)
				{
					if (TimeCurrent - route.TimeCreate >= SetupGet()->BridgeSystemSetup().TimeoutDealerOrder)
					{
						Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
							"检测到超时订单任务(ID({}),Symbol({}),Volume({}),Order({}),Position({}),Action({}),Flags({:X}))",
							route.MTRequest.ID,
							sk::StringConvert::WStringToMBytes(route.MTRequest.Symbol),
							route.MTRequest.Volume,
							route.MTRequest.ResultOrder,
							route.MTRequest.Position,
							route.MTRequest.Action,
							route.MTRequest.Flags
						);
						itclear = true;
					}
				});

			if (ProxyTimeOld == 0 || (TimeCurrent - ProxyTimeOld >= SetupGet()->ProxyInterval()))
			{
				if (SetupGet()->ProxyEnable() && !SetupGet()->ProxyPathname().empty())
				{
					auto ObjProcessName = sk::Helper::GetFileNameByFullPath(SetupGet()->ProxyPathname());
					if (!ObjProcessName.empty())
					{
						if (!sk::Helper::GetProcessidFromName(sk::StringConvert::MBytesToWString(ObjProcessName).c_str()))
						{
							PROCESS_INFORMATION ProcessInfo;
							if (!sk::Helper::OpenProcess(SetupGet()->ProxyPathname(), SetupGet()->ProxyPathname(), ProcessInfo))
							{
								Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info("Start Proxy Process Success(({}),({}))", ObjProcessName, SetupGet()->ProxyPathname());
							}
							else
							{
								Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error("Start Proxy Process Failed(({}),({}))", ObjProcessName, SetupGet()->ProxyPathname());
							}
						}
					}
				}
				ProxyTimeOld = TimeCurrent;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (!m_PluginReady.load()) break;
		} while (1);
	}



	MTAPIRES Dealer::MTSymbolSync()
	{
		MTAPIRES result = MT_RET_OK;
		SharedGet()->m_MTSymbolQ.clear();
		SharedGet()->m_OnConSymbolFieldQ.clear();

		UINT nTotalConSymbol = ServerApi()->SymbolTotal();
		for (UINT i = 0; i < nTotalConSymbol; ++i)
		{
			auto pSymbol = ServerApi()->SymbolCreate();
			if (MT_RET_OK == ServerApi()->SymbolNext(i, pSymbol))
			{
				shared::ConSymbolField Symbol;
				shared::Business::MakeConSymbolField(pSymbol, Symbol);
				SharedGet()->m_OnConSymbolFieldQ.push(pSymbol->Symbol(), Symbol);
				SharedGet()->m_MTSymbolQ.push(pSymbol->Symbol(), Symbol);
			}
			pSymbol->Release();
		}
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Symbol total({})", SharedGet()->m_OnConSymbolFieldQ.size());
		return result;
	}

	MTAPIRES Dealer::MTServerTimeSync()
	{
		MTAPIRES result = MT_RET_OK;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		auto conTime = ServerApi()->TimeCreate();
		result = ServerApi()->TimeGet(conTime);
		if (MT_RET_OK == result)
		{
			Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info(
				"系统时间: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(conTime->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		conTime->Release();
		License(ServerApi()->TimeCurrent());
		return result;
	}

	void Dealer::ConfirmPriceProcess(__in const shared::SystemOrder& in, __out shared::TradeTaskRouteGateway& out)
	{
		//!@ 价格
		switch (Dealer::SetupGet()->BridgeSystemSetup().DealerTradePxType)
		{
		case shared::BridgeSystemSetup::EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_AVGPX:
		{
			out.MTConfirm.Price = in.TradeAvgPx;
			out.MTConfirm.PriceGateway = in.TradeAvgPx;
		}break;
		case shared::BridgeSystemSetup::EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_LASTPX:
		{
			out.MTConfirm.Price = in.TradeLastPx;
			out.MTConfirm.PriceGateway = in.TradeLastPx;
		}break;
		default:
		{
			out.MTConfirm.Price = in.MTOrderPx;
			out.MTConfirm.PriceGateway = in.MTOrderPx;
		}break;
		}///switch
		if (out.MTConfirm.Price <= 0) { out.MTConfirm.Price = in.MTOrderPx; }
		if (out.MTConfirm.Price <= 0) { out.MTConfirm.Price = out.MTRequest.PriceOrder; }
		if (out.MTConfirm.Price <= 0) { out.MTConfirm.Price = in.TradeAvgPx; }
		if (out.MTConfirm.Price <= 0) { out.MTConfirm.Price = in.TradeLastPx; }

		if (in.Key.OrdSide == shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY)
		{
			out.MTConfirm.Price += (out.GatewayProdcon.SpreadTotal * out.GatewayProdcon.Point + out.GatewayProdcon.SpreadBid * out.GatewayProdcon.Point);
		}
		else
		{
			out.MTConfirm.Price += (out.GatewayProdcon.SpreadTotal * out.GatewayProdcon.Point + out.GatewayProdcon.SpreadAsk * out.GatewayProdcon.Point);
		}

		if (out.MTConfirm.Price <= 0)
			Dealer::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("ID({}),Volume({}),Price({})", out.MTConfirm.ID, out.MTConfirm.Volume, out.MTConfirm.Price);
	}

	bool Dealer::BridgeSystemGeneralSetupVerify()
	{
		bool result = true;
		//!@[桥系统交易任务工作时间段]
		__int64 SystemTime = SetupGet()->BridgeSystemTime();
		shared::BridgeSystemSetup SystemSetup = SetupGet()->BridgeSystemSetup();
		struct tm tmInfo;
		gmtime_s(&tmInfo, &SystemTime);
		UINT currTime = sk::time::Time::TimePeriodMade(tmInfo.tm_hour, tmInfo.tm_min, tmInfo.tm_sec);
		if (SystemSetup.SystemOperatingTimePeriod[tmInfo.tm_wday].TimeBegin < SystemSetup.SystemOperatingTimePeriod[tmInfo.tm_wday].TimeEnd)
		{
			if (!(currTime >= SystemSetup.SystemOperatingTimePeriod[tmInfo.tm_wday].TimeBegin &&
				currTime <= SystemSetup.SystemOperatingTimePeriod[tmInfo.tm_wday].TimeEnd))
			{
				result = false;
			}
		}

		return result;
	}

	void Dealer::GetOrderActionByRequestAction(const UINT& mt_request_action, shared::tradeorder::EnOrderAction& trade_order_action)
	{
		switch (mt_request_action)
		{
		case IMTRequest::TA_PRICE:
		case IMTRequest::TA_MARKET:
		case IMTRequest::TA_INSTANT:
		case IMTRequest::TA_PENDING:
		case IMTRequest::TA_ACTIVATE:
		case IMTRequest::TA_EXCHANGE:
		case IMTRequest::TA_ACTIVATE_SL:
		case IMTRequest::TA_ACTIVATE_TP:
		case IMTRequest::TA_STOPOUT_POSITION:
		case IMTRequest::TA_ACTIVATE_STOPLIMIT:
		case IMTRequest::TA_DEALER_POS_EXECUTE:
		case IMTRequest::TA_DEALER_ORD_PENDING:
		{
			trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_NEW;
		}break;
		case IMTRequest::TA_MODIFY:
		case IMTRequest::TA_DEALER_ORD_MODIFY:
		{
			trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_MODIFY;
		}break;
		case IMTRequest::TA_EXPIRATION:
		case IMTRequest::TA_REMOVE:
		case IMTRequest::TA_STOPOUT_ORDER:
		case IMTRequest::TA_DEALER_ORD_REMOVE:
		{
			trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_CANCEL;
		}break;
		}///switch
	}

	bool Dealer::GatewayProcessAuto(const IMTRequest* request, bool continueRequest)
	{
		bool Stop = true;
		switch (request->Type())
		{
		case IMTOrder::EnOrderType::OP_BUY:
		case IMTOrder::EnOrderType::OP_SELL:
		case IMTOrder::EnOrderType::OP_BUY_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_LIMIT:
		case IMTOrder::EnOrderType::OP_CLOSE_BY:
		{
			Stop = false;
		}break;
		case IMTOrder::EnOrderType::OP_BUY_STOP:
		case IMTOrder::EnOrderType::OP_SELL_STOP:
		case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
		{
			Stop = true;
		}break;
		}

		bool Atuo = Stop;

		if (request->Action() >= IMTRequest::EnTradeActions::TA_CLIENT_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_CLIENT_LAST)
		{
			switch (IMTRequest::EnTradeActions(request->Action()))
			{
			case IMTRequest::EnTradeActions::TA_PRICE:
			case IMTRequest::EnTradeActions::TA_REQUEST:
			case IMTRequest::EnTradeActions::TA_INSTANT:
			case IMTRequest::EnTradeActions::TA_MARKET:
			case IMTRequest::EnTradeActions::TA_EXCHANGE:
			case IMTRequest::EnTradeActions::TA_PENDING:
			case IMTRequest::EnTradeActions::TA_SLTP:
			case IMTRequest::EnTradeActions::TA_MODIFY:
			case IMTRequest::EnTradeActions::TA_REMOVE:
			case IMTRequest::EnTradeActions::TA_TRANSFER:
			case IMTRequest::EnTradeActions::TA_CLOSE_BY:
			{

			}break;
			}///switch
		}
		else if (request->Action() >= IMTRequest::EnTradeActions::TA_SERVER_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_SERVER_LAST)
		{
			switch (IMTRequest::EnTradeActions(request->Action()))
			{
			case IMTRequest::EnTradeActions::TA_ACTIVATE:
			case IMTRequest::EnTradeActions::TA_EXPIRATION:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_SL:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_TP:
			case IMTRequest::EnTradeActions::TA_STOPOUT_ORDER:
			case IMTRequest::EnTradeActions::TA_STOPOUT_POSITION:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_STOPLIMIT:
			{

			}break;
			}///switch
		}
		else if (request->Action() >= IMTRequest::EnTradeActions::TA_DEALER_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_DEALER_LAST)
		{
			switch (IMTRequest::EnTradeActions(request->Action()))
			{
			case IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_PENDING:
			case IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_MODIFY:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_REMOVE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_ACTIVATE:
			case IMTRequest::EnTradeActions::TA_DEALER_BALANCE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_SLIMIT:
			case IMTRequest::EnTradeActions::TA_DEALER_CLOSE_BY:
			{

			}break;
			}///switch
		}
		return Atuo;
	}

	bool Dealer::GatewayProcessAuto(const shared::RequestField& request)
	{
#if 0
		bRequest = false;
		bool StopOrder = false;
		switch (reqField.Type)
		{
		case IMTOrder::EnOrderType::OP_BUY:
		case IMTOrder::EnOrderType::OP_SELL:
		case IMTOrder::EnOrderType::OP_BUY_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_LIMIT:
		case IMTOrder::EnOrderType::OP_CLOSE_BY:
		{
			StopOrder = false;
		}break;
		case IMTOrder::EnOrderType::OP_BUY_STOP:
		case IMTOrder::EnOrderType::OP_SELL_STOP:
		case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
		{
			StopOrder = true;
		}break;
		}
		if (StopOrder) { return StopOrder; }


		bool AutoConfirm = true;
		switch (reqField.Action)
		{
		case IMTRequest::EnTradeActions::TA_PRICE:
		case IMTRequest::EnTradeActions::TA_MARKET:
		case IMTRequest::EnTradeActions::TA_EXCHANGE:
		case IMTRequest::EnTradeActions::TA_REQUEST:
		case IMTRequest::EnTradeActions::TA_INSTANT:
		case IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE:
		{
			AutoConfirm = false;
		}break;
		case IMTRequest::EnTradeActions::TA_PENDING:
		case IMTRequest::EnTradeActions::TA_SLTP:
		case IMTRequest::EnTradeActions::TA_TRANSFER:
		case IMTRequest::EnTradeActions::TA_EXPIRATION:
		case IMTRequest::EnTradeActions::TA_DEALER_ORD_PENDING:
		case IMTRequest::EnTradeActions::TA_DEALER_BALANCE:
		case IMTRequest::EnTradeActions::TA_CLOSE_BY:
		case IMTRequest::EnTradeActions::TA_DEALER_CLOSE_BY:
		case IMTRequest::EnTradeActions::TA_ACTIVATE:
		case IMTRequest::EnTradeActions::TA_ACTIVATE_SL:
		case IMTRequest::EnTradeActions::TA_ACTIVATE_TP:
		case IMTRequest::EnTradeActions::TA_DEALER_ORD_SLIMIT:
		case IMTRequest::EnTradeActions::TA_ACTIVATE_STOPLIMIT:
		case IMTRequest::EnTradeActions::TA_DEALER_ORD_ACTIVATE:
		case IMTRequest::EnTradeActions::TA_STOPOUT_ORDER:
		case IMTRequest::EnTradeActions::TA_STOPOUT_POSITION:
		case IMTRequest::EnTradeActions::TA_REMOVE:
		case IMTRequest::EnTradeActions::TA_DEALER_ORD_REMOVE:
		case IMTRequest::EnTradeActions::TA_MODIFY:
		case IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY:
		case IMTRequest::EnTradeActions::TA_DEALER_ORD_MODIFY:
		{
			switch (reqField.Flags)
			{
			case IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_PRICE:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_TRIGGER:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_EXP_TYPE:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_EXP_TIME:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_EXPERT:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_SIGNAL:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_SKIP_MARGIN_CHECK:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_SL:
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CHANGED_TP:
			case IMTRequest::TA_FLAG_CHANGED_SL | IMTRequest::TA_FLAG_CHANGED_TP:
			{
			}break;
			case IMTRequest::EnTradeActionFlags::TA_FLAG_CLOSE:
			{
				if (reqField.Action == IMTRequest::EnTradeActions::TA_ACTIVATE_TP ||
					reqField.Action == IMTRequest::EnTradeActions::TA_ACTIVATE_SL)
				{
					bRequest = true;
				}
			}break;
			}///switch
		}break;
		}///switch
		return AutoConfirm;
#else
		bool Stop = true;
		switch (request.Type)
		{
		case IMTOrder::EnOrderType::OP_BUY:
		case IMTOrder::EnOrderType::OP_SELL:
		case IMTOrder::EnOrderType::OP_BUY_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_LIMIT:
		case IMTOrder::EnOrderType::OP_CLOSE_BY:
		{
			Stop = false;
		}break;
		case IMTOrder::EnOrderType::OP_BUY_STOP:
		case IMTOrder::EnOrderType::OP_SELL_STOP:
		case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
		case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
		{
			Stop = true;
		}break;
		}

		bool Atuo = Stop;

		if (request.Action >= IMTRequest::EnTradeActions::TA_CLIENT_FIRST && request.Action <= IMTRequest::EnTradeActions::TA_CLIENT_LAST)
		{
			switch (IMTRequest::EnTradeActions(request.Action))
			{
			case IMTRequest::EnTradeActions::TA_PRICE:
			case IMTRequest::EnTradeActions::TA_REQUEST:
			case IMTRequest::EnTradeActions::TA_INSTANT:
			case IMTRequest::EnTradeActions::TA_MARKET:
			case IMTRequest::EnTradeActions::TA_EXCHANGE:
			case IMTRequest::EnTradeActions::TA_PENDING:
			case IMTRequest::EnTradeActions::TA_SLTP:
			case IMTRequest::EnTradeActions::TA_MODIFY:
			case IMTRequest::EnTradeActions::TA_REMOVE:
			case IMTRequest::EnTradeActions::TA_TRANSFER:
			case IMTRequest::EnTradeActions::TA_CLOSE_BY:
			{

			}break;
			}///switch
		}
		else if (request.Action >= IMTRequest::EnTradeActions::TA_SERVER_FIRST && request.Action <= IMTRequest::EnTradeActions::TA_SERVER_LAST)
		{
			switch (IMTRequest::EnTradeActions(request.Action))
			{
			case IMTRequest::EnTradeActions::TA_ACTIVATE:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_SL:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_TP:
			case IMTRequest::EnTradeActions::TA_ACTIVATE_STOPLIMIT:
			case IMTRequest::EnTradeActions::TA_STOPOUT_ORDER:
			case IMTRequest::EnTradeActions::TA_STOPOUT_POSITION:
			case IMTRequest::EnTradeActions::TA_EXPIRATION:
			{

			}break;
			}///switch
		}
		else if (request.Action >= IMTRequest::EnTradeActions::TA_DEALER_FIRST && request.Action <= IMTRequest::EnTradeActions::TA_DEALER_LAST)
		{
			switch (IMTRequest::EnTradeActions(request.Action))
			{
			case IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_PENDING:
			case IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_MODIFY:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_REMOVE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_ACTIVATE:
			case IMTRequest::EnTradeActions::TA_DEALER_BALANCE:
			case IMTRequest::EnTradeActions::TA_DEALER_ORD_SLIMIT:
			case IMTRequest::EnTradeActions::TA_DEALER_CLOSE_BY:
			{

			}break;
			}///switch
		}
		return Atuo;
#endif
	}

	MTAPIRES Dealer::QuickConfirm(const shared::ConfirmField& confirm, const UINT& retcode)
	{
		if (!m_pConfirm) return MT_RET_ERR_PARAMS;
		m_pConfirm->Clear();
		m_pConfirm->ID(confirm.ID);
		m_pConfirm->Retcode(retcode);
		m_pConfirm->Price(confirm.Price);
		m_pConfirm->Volume(confirm.Volume);
		m_pConfirm->OrderID(confirm.OrderID);
		m_pConfirm->Comment(confirm.Comment);
		m_pConfirm->PriceGateway(confirm.PriceGateway);
		m_pConfirm->ExternalRetcode(confirm.ExternalRetcode);
		m_pConfirm->PositionExternalID(confirm.PositionExternalID);

		auto result = ServerApi()->DealerAnswer(m_DealerManagerLogin.load(), m_pConfirm);
		if (MT_RET_OK == result)
		{
			if (!SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.pop(confirm.ID, [](const auto&, auto&) {}))
			{
				Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info(
					"确认订单超时(ID({}),SystemOrder({}),Volume({}),Price({:.5f}),PriceGateway({:.5f}))",
					confirm.ID,
					confirm.PositionExternalID[0] ? shared::SystemIDType::WStrToSystemOrderID(confirm.PositionExternalID) : 0,
					confirm.Volume,
					confirm.Price,
					confirm.PriceGateway
				);
			}
		}
		else
		{
			Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
				"确认订单失败(ID({}),SystemOrder({}),Volume({}),Price({:.5f}),PriceGateway({:.5f}),Retcode({}))",
				confirm.ID,
				confirm.PositionExternalID[0] ? shared::SystemIDType::WStrToSystemOrderID(confirm.PositionExternalID) : 0,
				confirm.Volume,
				confirm.Price,
				confirm.PriceGateway,
				result
			);
		}
		return result;
	}

	MTAPIRES Dealer::QuickConfirm(const IMTRequest* request, const UINT& retcode)
	{
		if (!m_pConfirm) return MT_RET_ERR_PARAMS;
		m_pConfirm->Clear();
		m_pConfirm->ID(request->ID());
		m_pConfirm->Retcode(retcode);
		m_pConfirm->Volume(request->Volume());
		m_pConfirm->Price(request->PriceOrder());
		m_pConfirm->OrderID(tostringw(request->ResultOrder()).c_str());
		auto result = ServerApi()->DealerAnswer(m_DealerManagerLogin.load(), m_pConfirm);
		if (MT_RET_OK == result)
		{
			if (!SharedGet()->m_GatewayOrderTaskRouteTimeoutCacheQ.pop(request->ID(), [](const auto&, auto&) {}))
			{
				Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info(
					"确认订单超时(ID({}),Symbol({}),MTPosition({}),MTOrder({}),SystemOrder({}),Volume({}),Action({}),Flags({:X}))",
					request->ID(),
					sk::StringConvert::WStringToMBytes(request->Symbol()),
					request->Position(),
					request->ResultOrder(),
					request->PositionExternalID() ? shared::SystemIDType::WStrToSystemOrderID(request->PositionExternalID()) : 0,
					request->Volume(),
					request->Action(),
					request->Flags()
				);
			}
		}
		else
		{
			Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
				"确认订单失败(ID({}),Symbol({}),MTPosition({}),MTOrder({}),SystemOrder({}),Volume({}),Action({}),Flags({:X}),Retcode({}))",
				request->ID(),
				sk::StringConvert::WStringToMBytes(request->Symbol()),
				request->Position(),
				request->ResultOrder(),
				request->PositionExternalID() ? shared::SystemIDType::WStrToSystemOrderID(request->PositionExternalID()) : 0,
				request->Volume(),
				request->Action(),
				request->Flags(),
				result
			);
		}
		return result;
	}

	MTAPIRES Dealer::ParamentersGet()
	{
		MTAPIRES result = MT_RET_OK;
		IMTConPlugin* pCurrPlugin = ServerApi()->PluginCreate();
		if (!pCurrPlugin)
		{
			return MT_RET_ERR_MEM;
		}
		result = ServerApi()->PluginCurrent(pCurrPlugin);
		if (MT_RET_OK != result)
		{
			pCurrPlugin->Release();
			return result;
		}
		auto PathObj = shared::Business::MTParamenterGet<IMTServerAPI, IMTConPlugin>(m_pServerApi, pCurrPlugin, PLUGIN_PARAM_KEY_PATH_OBJ);
		auto DealerManagerLogin = shared::Business::MTParamenterGet<IMTServerAPI, IMTConPlugin>(m_pServerApi, pCurrPlugin, PLUGIN_PARAM_KEY_DEALER_MANAGER_LOGIN);
		m_RouteRuleName = shared::Business::MTParamenterGet<IMTServerAPI, IMTConPlugin>(m_pServerApi, pCurrPlugin, PLUGIN_PARAM_KEY_ROUTE_RULE_NAME);
		m_PathObj = sk::StringConvert::WStringToMBytes(PathObj);
		m_DealerManagerLogin.store(wcstoull(DealerManagerLogin.c_str(), nullptr, 10));

		pCurrPlugin->Release();
		if (!m_PathObj.empty() && m_DealerManagerLogin.load() > 0 && !m_RouteRuleName.empty())
		{
			return MT_RET_OK;
		}
		return MT_RET_ERR_PARAMS;
	}

	MTAPIRES Dealer::RouteCreate()
	{
		MTAPIRES result = MT_RET_OK;

		auto pRoute = ServerApi()->RouteCreate();
		//!说明 : 是否已经存在的路上关系
		//!注意 : 
		//!日期 : Fri Jun 12 16:01:45 UTC+0800 2020
		for (UINT i = 0; i < ServerApi()->RouteTotal(); ++i)
		{
			if (MT_RET_OK != ServerApi()->RouteNext(i, pRoute))
			{
				pRoute->Release();
				return MT_RET_ERROR;
			}
			if (!m_RouteRuleName.compare(pRoute->Name()))
			{
				pRoute->Release();
				return MT_RET_OK;
			}
		}


		auto pDealerManager = ServerApi()->ManagerCreate();
		if ((result = ServerApi()->ManagerGet(m_DealerManagerLogin.load(), pDealerManager)) != MT_RET_OK)
		{
			pDealerManager->Release();
			pRoute->Release();
			return result;
		}

		pRoute->Clear();
		auto pRouteDealer = ServerApi()->RouteDealerCreate();
		pRouteDealer->Clear();
		pRouteDealer->Login(pDealerManager->Login());
		if ((result = pRoute->DealerAdd(pRouteDealer)) != MT_RET_OK)
		{
			pRoute->Release();
			pRouteDealer->Release();
			pDealerManager->Release();
			return result;
		}
		pRouteDealer->Release();
		pDealerManager->Release();

		pRoute->Name(m_RouteRuleName.c_str());
		pRoute->Mode(IMTConRoute::EnRouteMode::MODE_DISABLED);
		pRoute->Action(IMTConRoute::EnRouteAction::ACTION_DEALER_ONLINE);
		pRoute->Request(IMTConRoute::EnRouteFlags::REQUEST_ALL);
		pRoute->Type(IMTConRoute::EnTypeFlags::TYPE_ALL);
		result = ServerApi()->RouteAdd(pRoute);
		pRoute->Release();
		return result;
	}

}///namespace gateway