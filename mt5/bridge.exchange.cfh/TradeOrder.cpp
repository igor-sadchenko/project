#include "stdafx.h"
#include "TradeOrder.h"

namespace cfh {

	TradeOrder::TradeOrder()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	TradeOrder::~TradeOrder()
	{
		UnInit();
	}

	int TradeOrder::Init()
	{
		//m_pTradeOrderCacheQ = new shared::Memmap<shared::SystemID, shared::tradeorder::ExchangePosition>(Global::GetSetup()->TradeOrderCachePathname(), sk::packet(__FUNCTION__).append(".tradeorder.mapping"));
		sk::intptr out = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_tcp_client_setup>(shared::api_network, shared::api_network_tcp_client_setup)(Global::GetSetup()->MarketDataTcptoAddress().c_str(), out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}

	int TradeOrder::UnInit()
	{
		//SK_DELETE_PTR(m_pTradeOrderCacheQ);
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	int TradeOrder::Open()
	{
		if (m_IsOpen.load()) return 0;

		auto FixconBuf = sk::Helper::LoadFile(Global::GetSetup()->TradeOrderFixcon().c_str());
		if (!FixconBuf.size()) return -1;
		sk::intptr outPtr = 0;
		Global::GetApi()->GetInterFaceApi<shared::tf_api_libfix_setup>(shared::api_libfix, shared::api_libfix_setup)(&FixconBuf[0], FixconBuf.size(), outPtr);
		if (!outPtr) return -1;
		FixApi(outPtr);
		FixApi()->Open(this);

		m_Timer.Open();
		m_Timer.TimerAppend(sk::ms_hour, this, OnTimer);
		m_Timer.TimerAppend(sk::ms_hour_half, this, OnTimer);
		m_Timer.TimerAppend(sk::ms_second, this, OnTimer);

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });

		NetApi<sk::network::INetworkApi>()->Start(this);


		m_IsOpen.store(true);
		return 0;
	}

	int TradeOrder::Close()
	{
		if (!m_IsOpen.load()) return 0;

		m_Timer.Close();

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

	void TradeOrder::OnTcpClientConnect()
	{

	}

	void TradeOrder::OnTcpClientDisconnect()
	{

	}

	void TradeOrder::OnTcpClientWelcome(const sk::network::Header&, const sk::packet&)
	{
		NetLoginRequest(sk::packet((char*)Global::GetSetup()->LoginFieldTradeOrder(), shared::LENLOGINFIELD));
	}

	void TradeOrder::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
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
		case sk::network::NetCmd::EN_NETCMD_300000410:
		{
			OnNetTradeOrderSignal(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000414:
		{
			OnNetOrderStatusRequest(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000415:
		{
			OnNetTradeOrderTimeoutSignal(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000420:
		{
			OnNetPositionRequest(head, pak);
		}break;
		}///switch
	}

	void TradeOrder::OnNetLoginResult(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(head.Retcode))
		{
			/*NetStatus(sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED);*/
			NetSystemTimeRequest();
			NetSystemSetupRequest();
			NetSystemProductsRequest();
		}
	}
	void TradeOrder::OnNetShowWindow(const sk::network::Header& head, const sk::packet& pak)
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
	void TradeOrder::OnNetShutdown(const sk::network::Header& head, const sk::packet& pak)
	{
		if (!Global::SystemExit())
		{
			Global::SystemExit(1);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", "System request close , closing . . .");
		}
	}

	void TradeOrder::OnNetSystemProducts(const sk::network::Header& head, const sk::packet& pak)
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
	void TradeOrder::OnNetSystemSetup(const sk::network::Header& head, const sk::packet& pak)
	{
		shared::BridgeSystemSetup* setup = (shared::BridgeSystemSetup*)pak.data();
		if (setup)
		{
			Global::GetSetup()->BridgeSystemSetup(*setup);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
		}
	}

	void TradeOrder::OnNetSystemTime(const sk::network::Header& head, const sk::packet& pak)
	{
		std::int64_t* time = (std::int64_t*)pak.data();
		if (time)
		{
			Global::GetSetup()->BridgeSystemTime(*time);
		}
	}

	void TradeOrder::OnNetProductsRequest(const sk::network::Header& head, const sk::packet& pak)
	{
		sk::packet productsBuf;
		Global::GetSetup()->m_ProductQ.iterate(
			[&](const auto&, shared::LPProduct& product)
			{
				productsBuf.append((char*)&product, shared::LENLPPRODUCT);
			});
		NetProductsSend(productsBuf);
	}

	void TradeOrder::OnFixLogon()
	{
		//TradeDataRecovery();	
		FixStatus(libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED);
		//TradeDataRecoverySafe();
		//Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", __FUNCTION__);
	}

	void TradeOrder::OnFixLogout()
	{
		FixStatus(libfix::EnFixStatus::EN_FIXSTATUS_DISCONNECTED);
#if 0
		m_OrderSignalRouteQ.iterate(
			[&](const auto&, auto& route)
			{
				route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_NETWORK_DISCONNECT;
				route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_CONNECTION;
				m_RequestAckQ.push(route.OrderRequest);
			});
		Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", SKAPIRES_DESC(SKAPIRES::EXCHANGE_RET_ERROR_NETWORK_DISCONNECT));
#endif
	}

	void TradeOrder::OnFixResponse(const libfix::EnFixMessageType& FixMsgType, const sk::intptr& FixPak)
	{
		switch (FixMsgType)
		{
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_3:
		{
			FixReject(PtrToType(libfix::_3__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_8:
		{
			FixExecutionReport(PtrToType(libfix::_8__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_AP:
		{
			FixPositionReport(PtrToType(libfix::_AP__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_AO:
		{
			FixRequestForPositionsAck(PtrToType(libfix::_AO__, FixPak));
		}break;
		case libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_AAB:
		{
			FixAccountInfo(PtrToType(libfix::_AAB__, FixPak));
		}break;
		}///switch
	}

	void TradeOrder::FixReject(const libfix::_3__* tag3)
	{
		if (!tag3) return;

		if (!tag3->_372.compare("D"))//! 订单信号请求被拒绝
		{
			m_SeqNumCIOrdIDRouteQ.pop(
				tag3->TargetSeqNum,
				[&](const auto&, auto& key)
				{
					m_OrderSignalRouteQ.search(
						key,
						[&](const auto& orderid, shared::TradeTaskRouteExchange& route)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
								"拒绝定单(OrderID({:X}),MTProdID({:X}),LPProdID({:X}),LPOrderQty({}),LPTradeQty({}),OrdSide({}),OrdRejReason({}.{}))",
								orderid,
								route.OrderRequest.GatewayPID,
								route.OrderRequest.ExchangePID,
								route.OrderRequest.LPOrderQty,
								route.OrderRequest.LPTradeQty,
								shared::tradeorder::Misc::StrOrdSideA(route.OrderRequest.Key.OrdSide),
								tag3->_373,
								tag3->_58);
#if 0
							route.OrderRequest.FixSessionRejectReason = \
								decltype(route.OrderRequest.FixSessionRejectReason)(strtol(tag3->_373.c_str(), nullptr, 10));
							sk::SafeCopyA(route.OrderRequest.FixReasonText, tag3->_58.c_str(), _countof(route.OrderRequest.FixReasonText));
#endif
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_REJECTED;
							route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
							m_RequestResQ.push(route);
						});
				});
		}

		else if (!tag3->_372.compare("H"))//! 订单状态请求被拒绝
		{
			m_SeqNumOrdStatusReqIDRouteQ.pop(
				tag3->TargetSeqNum,
				[&](const auto&, auto& reqID)
				{
					m_OrdStatusRouteQ.pop(
						reqID,
						[&](const auto&, auto& route)
						{
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_STATUS_REQUEST;
#if 0
							route.OrderRequest.FixSessionRejectReason = decltype(route.OrderRequest.FixSessionRejectReason)(strtol(tag3->_373.c_str(), nullptr, 10));
							sk::SafeCopyA(route.OrderRequest.FixReasonText, tag3->_58.c_str(), _countof(route.OrderRequest.FixReasonText));
#endif
							Global::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->error(
								"订单状态请求被拒绝(ID({}),RejectReason({}.{}))",
								reqID,
								tag3->_373,
								tag3->_58);
						});
				});
		}

		else if (!tag3->_372.compare("AD"))//! 订单记录请求被拒绝
		{
			m_SeqNumTradeRequestIDRouteQ.pop(
				tag3->TargetSeqNum,
				[&](const auto&, auto& reqID)
				{
					m_TradeRecordRouteQ.pop(
						reqID,
						[&](const auto&, auto& route)
						{
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_TRADE_RECORD_REQUEST;
#if 0
							route.OrderRequest.FixSessionRejectReason = decltype(route.OrderRequest.FixSessionRejectReason)(strtol(tag3->_373.c_str(), nullptr, 10));
							sk::SafeCopyA(route.OrderRequest.FixReasonText, tag3->_58.c_str(), _countof(route.OrderRequest.FixReasonText));
#endif
							Global::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->error(
								"订单记录请求被拒绝(ID({}),RejectReason({}.{}))",
								reqID,
								tag3->_373,
								tag3->_58);
						});
				});
		}

	}



	SKAPIRES TradeOrder::FixRequestForPositions()
	{
		SKAPIRES result = SKAPIRES::EXCHANGE_RET_ERROR;
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			return result;

		libfix::_AN__ req;
		req._710 = shared::SystemIDType::SystemOrderIDToStr(shared::SystemIDType::SystemOrderIDMade(0xB00000000));
		req._724 = "0";
		req._5003 = "-1";
		req._1 = FixApi()->Account();
		req._581 = "1";

		sk::uint32 SeqNum = 0;
		FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_AN, TypeToPtr(&req), SeqNum);
		return SKAPIRES::EXCHANGE_RET_SUCCESS;
	}
	SKAPIRES TradeOrder::FixAccountInfoRequest()
	{
		SKAPIRES result = SKAPIRES::EXCHANGE_RET_ERROR;
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			return result;

		libfix::_AAA__ req;
		req._1 = "3762";

		sk::uint32 SeqNum = 0;
		FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_AAA, TypeToPtr(&req), SeqNum);
		return SKAPIRES::EXCHANGE_RET_SUCCESS;
	}
	void TradeOrder::FixRequestForPositionsAck(const libfix::_AO__* tagAO)
	{
#if 0
		shared::tradeorder::LPPositionReportRequestAck reportAck;
		sk::SafeCopyA(reportAck.Authcode, Global::GetSetup()->LoginFieldTrade()->Authcode, _countof(reportAck.Authcode));
		reportAck.PosReqResult = decltype(reportAck.PosReqResult)(std::stoi(ack->_728));
		reportAck.PosReqStatus = decltype(reportAck.PosReqStatus)(std::stoi(ack->_729));
		reportAck.TotalNumPosReports = std::stoi(ack->_727);
		Global::SharedGet()->TotalNumPosReports.store(reportAck.TotalNumPosReports);
		reportAck.Retcode = SKAPIRES::BRIDGE_FAILED;
		if (reportAck.PosReqResult == shared::tradeorder::LPPositionReportRequestAck::EnPosReqResult::EN_POS_REQ_RESULT_ValidRequest &&
			reportAck.PosReqStatus == shared::tradeorder::LPPositionReportRequestAck::EnPosReqStatus::EN_POS_REQ_STATUS_Completed)
		{
			Global::SharedGet()->m_CombinedPositionsQ.clear();
			reportAck.Retcode = SKAPIRES::BRIDGE_SUCCESS;
		}
		/*sk::SafeCopyA(reportAck.Reason, res->_58.c_str(), _countof(reportAck.Reason));*/
		NetTradeOrderPositionReportRequestAck(reportAck);
#endif
	}
	void TradeOrder::FixPositionReport(const libfix::_AP__* tagAP)
	{
#if 0
		shared::tradeorder::MTPositionReport report;

		Global::SharedGet()->m_SymbolProductQ.search(postions->_55.c_str(),
			[&](const auto& sym, auto& pord)
			{
				report.ID = pord.ID;
				sprintf_s(report.Symbol.symbol, "%s.%s", pord.SystemSymbol.symbol, Global::GetSetup()->LoginFieldTrade()->Identify);
			});
		sk::SafeCopyA(report.Comment, Global::GetSetup()->LoginFieldTrade()->Identify, _countof(report.Comment));
		if (!postions->_730.empty())
		{
			report.AvgPrice = std::stod(postions->_730);
		}
		if (!postions->_231.empty())
		{
			report.ContractMultiplier = std::stoi(postions->_231);
		}
		if (!postions->__702.empty())
		{
			if (!postions->__702[0]._704.empty())
			{
				report.PositionAction = IMTPosition::EnPositionAction::POSITION_BUY;
				report.Volume = std::stod(postions->__702[0]._704);
			}
			else if (!postions->__702[0]._705.empty())
			{
				report.PositionAction = IMTPosition::EnPositionAction::POSITION_SELL;
				report.Volume = std::stod(postions->__702[0]._705);
			}
		}
		report.Digits = 5;
		Global::SharedGet()->m_CombinedPositionsQ.push(report.Symbol, report);
		auto view2 = &Global::SharedGet()->m_CombinedPositionsQ;

		if (Global::SharedGet()->TotalNumPosReports.load() <= Global::SharedGet()->m_CombinedPositionsQ.size())
		{
			NetTradeOrderPositionReportSend();
			Global::Log()->error("NetTradeOrderPositionReportSend total({})", Global::SharedGet()->TotalNumPosReports.load());
		}
#endif
	}
	void TradeOrder::FixAccountInfo(const libfix::_AAB__* tagAAB)
	{

	}
	void TradeOrder::FixExecutionReport(const libfix::_8__* tag8)
	{
		if (!tag8) return;

		//!@ 定单任务成交了,清理缓冲
		m_SeqNumCIOrdIDRouteQ.pop(tag8->TargetSeqNum, [&](const auto&, auto&) {});
		auto found = m_OrderSignalRouteQ.search_clear(
			shared::SystemIDType::StrToSystemOrderID(tag8->_11),
			[&](shared::TradeTaskRouteExchange& route, bool& itclear)
			{
				MadeExchangePosition(route.OrderRequest.ExchangePID, *tag8, route.TradePosition);

				route.TradeReport.Assign(*tag8);

				route.OrderRequest.LPTradeQty = route.TradeReport.CumQty;
				route.OrderRequest.TradeAvgPx = route.TradeReport.AvgPx;
				route.OrderRequest.TradeLastPx = route.TradeReport.LastPx;
				route.OrderRequest.OrdExecType = decltype(route.OrderRequest.OrdExecType)(route.TradeReport.ExecType);
				route.OrderRequest.OrdStatus = decltype(route.OrderRequest.OrdStatus)(route.TradeReport.OrdStatus);

				switch (route.OrderRequest.OrdExecType)
				{
				case shared::tradeorder::EnOrdExecType::EN_ORD_EXECTYPE_NEW:
				{
					route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS_ORDER_EXECTYPE_ACCEPTED;
					route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_ACCEPTED;
				}break;
				case shared::tradeorder::EnOrdExecType::EN_ORD_EXECTYPE_REPLACED:
				{
					route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS_ORDER_EXECTYPE_REPLACED;
					route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_DONE;
					m_RequestResQ.push(route);
				}break;
				case shared::tradeorder::EnOrdExecType::EN_ORD_EXECTYPE_CANCELLED:
				{
					if (route.OrderRequest.OrdRequestType & \
						static_cast<decltype(route.OrderRequest.OrdRequestType)>(shared::SystemOrder::EnOrdRequestType::EN_ORD_REQUEST_TYPE_POSITION_ADJUST))
					{
						if (route.OrderRequest.LPTradeQty <= 0)
						{
							if (route.AutoAdjust.TradeQtyOpen <= 0)
							{
								Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", "撤销订单[自动调仓](没有成交)");
								route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_EXECTYPE_REJECTED;
								route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
								m_RequestResQ.push(route);
							}
						}
						else
						{
							if (route.AutoAdjust.TradeQtyOpen <= 0)
							{
								route.AutoAdjust.TradeQtyOpen = route.OrderRequest.LPTradeQty;
								shared::tradeorder::Misc::OrdSideNot(route.OrderRequest.Key.OrdSide);
								route.OrderRequest.LPOrderQty = route.AutoAdjust.TradeQtyOpen;
							}
							else
							{
								route.AutoAdjust.TradeQtyClose += route.OrderRequest.LPTradeQty;
								route.OrderRequest.LPOrderQty = route.AutoAdjust.TradeQtyOpen - route.AutoAdjust.TradeQtyClose;
							}
						}

						itclear = true;
						route.UpdateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
						m_RerequestQ.push(route);
					}
					else
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("订单被撤销(CumQty({}))", route.OrderRequest.LPTradeQty);
						route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS_ORDER_EXECTYPE_CANCELLED;
						route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_CANCEL;
						m_RequestResQ.push(route);
					}
				}break;
				case shared::tradeorder::EnOrdExecType::EN_ORD_EXECTYPE_REJECTED:
				{

					//!@ 如果是自动调仓订单 不再继续处理
					if (route.OrderRequest.OrdRequestType & \
						static_cast<decltype(route.OrderRequest.OrdRequestType)>(shared::SystemOrder::EnOrdRequestType::EN_ORD_REQUEST_TYPE_POSITION_ADJUST))
					{
						if (route.AutoAdjust.TradeQtyOpen <= 0)
						{
							Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}","拒绝订单[自动调仓]");
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_REJECTED;
							route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
							m_RequestResQ.push(route);
						}
						else
						{
							route.UpdateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
							m_RerequestQ.push(route);
							itclear = true;
						}
					}
					else
					{
						sk::packet ReasonText = tag8->_58;
						Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("拒绝定单(OrdRejReason({}.{}))", route.TradeReport.OrdRejReason, ReasonText.empty() ? route.TradeReport.Text : ReasonText);
						//! 保证有成交
						if (shared::tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC == route.OrderRequest.OrdTimeInForce &&
							("CFH_" + ReasonText).find(SKAPIRES_DESC(SKAPIRES::EXHANGE_RET_ERROR_ORDER_CFH_QUANTITY_NOT_AVAILABLE_FOR_FOK_ORDER)) != sk::packet::npos)
						{
							route.UpdateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
							m_RerequestQ.push(route);
							itclear = true;
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}", "流动性原因拒绝的订单，再次请求");
						}
						else
						{
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_EXECTYPE_REJECTED;
							route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
							m_RequestResQ.push(route);
						}
					}
				}break;
				case shared::tradeorder::EnOrdExecType::EN_ORD_EXECTYPE_TRADE:
				{
					if (shared::tradeorder::EnOrdStatus::EN_ORD_STATUS_FILLED == route.OrderRequest.OrdStatus)
					{
						if (route.OrderRequest.OrdRequestType & \
							static_cast<decltype(route.OrderRequest.OrdRequestType)>(shared::SystemOrder::EnOrdRequestType::EN_ORD_REQUEST_TYPE_POSITION_ADJUST))
						{
							if (route.AutoAdjust.TradeQtyOpen <= 0)
							{
								route.AutoAdjust.TradeQtyOpen = route.OrderRequest.LPTradeQty;
								shared::tradeorder::Misc::OrdSideNot(route.OrderRequest.Key.OrdSide);
								route.OrderRequest.LPOrderQty = route.AutoAdjust.TradeQtyOpen;

								itclear = true;
								route.UpdateTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();
								m_RerequestQ.push(route);
							}
							else
							{
								route.AutoAdjust.TradeQtyClose += route.OrderRequest.LPTradeQty;
								route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS_ORDER_STATUS_FILLED;
								route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_DONE;
								m_RequestResQ.push(route);
							}
						}
						else
						{
							route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS_ORDER_STATUS_FILLED;
							route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_DONE;
							m_RequestResQ.push(route);
						}
					}
				}break;
				default:
				{
					route.OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR;
					route.OrderRequest.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_ERROR;
					m_RequestResQ.push(route);
				}break;
				}
			});

		if (!found)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->error(
				"来自CFH客户端的操作/或未找到定单路由数据(OrderID({:X}),LPTradeQty({}))",
				shared::SystemIDType::StrToSystemOrderID(tag8->_11),
				tag8->_14.empty() ? 0 : std::stod(tag8->_14));
		}
	}

	SKAPIRES TradeOrder::FixNewOrderSingle(shared::SystemOrder& OrderRequest, UINT& FixSeqNum)
	{

		OrderRequest.OrderID = shared::SystemIDType::SystemOrderIDMade(shared::SystemIDType::ExchangeType(OrderRequest.ExchangePID));

		SKAPIRES result = SKAPIRES::EXCHANGE_RET_ERROR;
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			return result;


		auto view = &Global::GetSetup()->m_ProductQ;

		Global::GetSetup()->m_ProductQ.search(
			shared::SystemIDType::ExchangeProductType(OrderRequest.ExchangePID),
			[&](const auto&, shared::LPProduct& lpProd)
			{
				libfix::_D__ v;
				v.__453.emplace_back(libfix::_D__::_453{ "skstu","D","3" });
				v._11 = shared::SystemIDType::SystemOrderIDToStr(OrderRequest.OrderID);
				v._15 = lpProd.QuotedCCY.currency;
				v._21 = "T";
				v._55 = lpProd.Symbol.symbol;

				v._38 = tostringa(OrderRequest.LPOrderQty);
				v._54 = (CHAR)OrderRequest.Key.OrdSide;
				v._40 = (CHAR)(OrderRequest.OrdType);
				switch (OrderRequest.OrdType)
				{
				case shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
				{
				}break;
				default:
				{
					v._44 = tostringa(OrderRequest.MTOrderPx);
				}break;
				}
				v._59 = (CHAR)(OrderRequest.OrdTimeInForce);
				v._60 = FixApi()->TransactTimeNow();
				v._77 = (CHAR)OrderRequest.PositionEffect;
				if (!FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_D, TypeToPtr(&v), FixSeqNum))
				{
					result = SKAPIRES::EXCHANGE_RET_SUCCESS;
#if 0
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("NewOrderSignal(ProdID({:X}),Symbol({}),OrderQty({}),OrderID({:X}))",
						lpProd.ID,
						lpProd.Symbol.symbol,
						OrderRequest.OrderQty,
						OrderRequest.OrderID
					);
#endif
				}
			});
		return result;
	}

	SKAPIRES TradeOrder::FixOrderStatusRequest(const shared::SystemOrder& order)
	{
		SKAPIRES result = SKAPIRES::EXCHANGE_RET_ERROR;
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
			return result;
		Global::GetSetup()->m_ProductQ.search(
			shared::SystemIDType::ExchangeProductType(order.ExchangePID),
			[&](const auto&, shared::LPProduct& lpProd)
			{
				libfix::_H__ v;
				v._11 = shared::SystemIDType::SystemOrderIDToStr(order.OrderID);
				v._48 = tostringa(lpProd.ID);
				v._22 = "8";
				v._54 = (CHAR)order.Key.OrdSide;
				v._790 = tostringa(FixApi()->MakeReqIDFromShort());
				FixSeqNum SeqNum = 0;
				if (0 == FixApi()->FixRequest(libfix::EnFixMessageType::EN_FIX_MESSAGE_TYPE_H, TypeToPtr(&v), SeqNum))
				{
					m_SeqNumOrdStatusReqIDRouteQ.push(SeqNum, std::stol(v._790));
					shared::TradeTaskRouteExchange route;
					route.OrderRequest = order;
					m_OrdStatusRouteQ.push(std::stol(v._790), route);
					result = SKAPIRES::EXCHANGE_RET_SUCCESS;
				}
			});
		return result;
	}
	void TradeOrder::OnNetOrderStatusRequest(const sk::network::Header& head, const sk::packet& pak)
	{

	}
	void TradeOrder::OnNetPositionRequest(const sk::network::Header& head, const sk::packet& pak)
	{
		//if (!m_pTradeOrderCacheQ)
		//	return;
		sk::packet positions;
		//m_pTradeOrderCacheQ->Iterate(
		//	[&](const auto&, shared::tradeorder::ExchangePosition& position)
		//	{
		//		positions.append((char*)&position, shared::tradeorder::LENEXCHANGEPOSITION);
		//	});
		if (!positions.empty())
		{
			NetPositionSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, positions);
		}
	}
	void TradeOrder::OnNetTradeOrderTimeoutSignal(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;

		auto order = (shared::SystemOrder*)pak.data();
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
		{
			order->SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_REJECTED;
			order->MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
			NetTradeOrderRequestRes(*order);
			return;
		}

		shared::TradeTaskRouteExchange route;
		route.OrderRequest = *order;
		m_RequestQ.push(route);
	}
	void TradeOrder::OnNetTradeOrderSignal(const sk::network::Header& head, const sk::packet& pak)
	{
#if 0
		if (Global::GetSetup()->BridgeSystemSetup().DevelopDebugSwitch == shared::BridgeSystemSetup::EnDevelopDebugSwitch::EN_DEVELOP_DEBUG_MODE_ENABLE)
		{
			std::vector<shared::SystemOrder> orders;
			sk::Helper::OneUnknownParseStruct<shared::SystemOrder>(pak.data(), pak.size(), orders);
			if (orders.empty())
				return;
			for (auto& order : orders)
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
					"{}(ID({}),Login({}),GatewayID({}),PositionID({}),OrderQty({}),OrderPx({}))",
					__FUNCTION__,
					order.MTRequestID,
					order.MTLogin,
					order.GatewayProductID,
					order.PositionID,
					order.LPOrderQty,
					order.MTOrderPx
				);

				//OrderResponse.TradeQty = SMTMath::VolumeToDouble(UINT64(OrderResponse.OrderQty / 10));
				order.LPTradeQty = 1.0;
				order.TradeAvgPx = order.MTOrderPx;
				order.TradeLastPx = order.MTOrderPx;

				order.MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_DONE_PARTIAL;
				order.SKRetcode = SKAPIRES::EXCHANGE_RET_SUCCESS;

				NetTradeOrderRequestRes(order);
			}

			return;
		}
#endif
		if (pak.empty())
			return;
		auto order = (shared::SystemOrder*)pak.data();
		if (!FixApi() || FixStatus() < libfix::EnFixStatus::EN_FIXSTATUS_CONNECTED)
		{
			order->SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_REJECTED;
			order->MTRetcode = shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_REJECT;
			NetTradeOrderRequestRes(*order);
			return;
		}

		shared::TradeTaskRouteExchange route;
		route.OrderRequest = *order;
		m_RequestQ.push(route);
	}

	void TradeOrder::ProcessT()
	{
		do {
			auto currentTime = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			auto route_rereq = m_RerequestQ.pop();
			if (route_rereq)
			{
				if (route_rereq->UpdateTime > 0 && currentTime - route_rereq->UpdateTime > 500)
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn(
						"延迟继续请求(SystemProdID({:X}),LPOrderQty({}))",
						route_rereq->OrderRequest.Key.SystemPID,
						route_rereq->OrderRequest.LPOrderQty);
					++route_rereq->OrderRequest.LPProcessCount;
					m_RequestQ.push(*route_rereq);
				}
				else
				{
					m_RerequestQ.push(*route_rereq);
				}
			}

			auto route_req = m_RequestQ.pop();
			if (route_req)
			{
				bool check = true;
				m_OrderSignalRouteQ.iterate(
					[&](const auto&, const shared::TradeTaskRouteExchange& route, auto& itbreak)
					{
						if (route.OrderRequest.Key.SystemPID == route_req->OrderRequest.Key.SystemPID)
						{
							m_RequestQ.push(*route_req);
							Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("定单任务繁忙(SystemProdID({:X}))", route.OrderRequest.Key.SystemPID);
							check = false;
							itbreak = true;
						}
					});
				if (check)
				{
					UINT FixSeqNum = 0;
					if (SKAPIRES_SUCCESS(FixNewOrderSingle(route_req->OrderRequest, FixSeqNum)))
					{
						m_SeqNumCIOrdIDRouteQ.push(FixSeqNum, route_req->OrderRequest.OrderID);
						m_OrderSignalRouteQ.push(route_req->OrderRequest.OrderID, *route_req);
					}
					else
					{
						route_req->OrderRequest.SKRetcode = SKAPIRES::EXCHANGE_RET_ERROR_ORDER_REJECTED;
						NetTradeOrderRequestRes(route_req->OrderRequest);
					}
				}
			}


			auto route_res = m_RequestResQ.pop();
			if (route_res)
			{
				m_OrderSignalRouteQ.pop(route_res->OrderRequest.OrderID, [&](const auto&, auto&) {});

				NetTradeOrderRequestRes(route_res->OrderRequest);

#if 0
				if (route_res->OrderRequest.LPTradeQty > 0)
				{
					Global::GetSetup()->m_ProductQ.search(
						strtoull(route_res->TradePosition.SecurityID, nullptr, 10),
						[&](const auto&, auto& instrument)
						{
							sk::SafeCopyA(route_res->TradePosition.Symbol, instrument.Symbol.symbol, _countof(route_res->TradePosition.Symbol));
						});

					auto found = m_pTradeOrderCacheQ->Search(
						route_res->OrderRequest.ExchangePID,
						[&](shared::tradeorder::ExchangePosition& position, shared::EnNodeChangedFlag& flag)
						{
							auto bak = position.TotalQty;
							position = route_res->TradePosition;
							position.TotalQty = bak;
							if (route_res->OrderRequest.Key.OrdSide == shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY)
							{
								position.TotalQty += static_cast<decltype(position.TotalQty)>(route_res->TradePosition.CumQty * shared::tradeorder::LPVOLUMEDIV);
							}
							else if (route_res->OrderRequest.Key.OrdSide == shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL)
							{
								position.TotalQty -= static_cast<decltype(position.TotalQty)>(route_res->TradePosition.CumQty * shared::tradeorder::LPVOLUMEDIV);
							}

							if (position.TotalQty > 0)
							{
								position.Side = CHAR(shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY);
								flag = shared::EnNodeChangedFlag::EN_NODE_CHANGED_FLAG_PUSH;
								NetPositionSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, sk::packet((char*)&position, shared::tradeorder::LENEXCHANGEPOSITION));
							}
							else if (position.TotalQty < 0)
							{
								position.Side = CHAR(shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL);
								flag = shared::EnNodeChangedFlag::EN_NODE_CHANGED_FLAG_PUSH;
								NetPositionSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, sk::packet((char*)&position, shared::tradeorder::LENEXCHANGEPOSITION));
							}
							else
							{
								flag = shared::EnNodeChangedFlag::EN_NODE_CHANGED_FLAG_POP;
								NetPositionSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE, sk::packet((char*)&position, shared::tradeorder::LENEXCHANGEPOSITION));
							}
						});
					if (!found)
					{
						if (route_res->OrderRequest.Key.OrdSide == shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL)
							route_res->TradePosition.TotalQty = 0 - static_cast<decltype(route_res->TradePosition.TotalQty)>(route_res->TradePosition.CumQty * shared::tradeorder::LPVOLUMEDIV);
						else
							route_res->TradePosition.TotalQty = static_cast<decltype(route_res->TradePosition.TotalQty)>(route_res->TradePosition.CumQty * shared::tradeorder::LPVOLUMEDIV);
						m_pTradeOrderCacheQ->Push(route_res->TradePosition.ID, route_res->TradePosition);
						NetPositionSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, sk::packet((char*)&route_res->TradePosition, shared::tradeorder::LENEXCHANGEPOSITION));
					}
				}
#endif
			}

			if (!m_IsOpenT.load()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (1);
	}

	int TradeOrder::TestOrderRequest(const shared::tradeorder::ExchangeOrder& OrderRequest)
	{
		return 0;
	}

	int TradeOrder::OnTimer(const sk::int64& interval, const sk::intptr& coreptr, const sk::int64& currtime)
	{
		auto pTradeOrder = reinterpret_cast<TradeOrder*>(coreptr);
		if (interval == sk::ms_second)//!@ 每隔一秒检测超时
		{

		}
		else if (interval == sk::ms_hour)
		{

		}
		else if (interval == sk::ms_hour_half)
		{

		}

		return 0;
	}


}///namespace cfh