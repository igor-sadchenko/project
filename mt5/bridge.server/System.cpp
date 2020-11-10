#include "stdafx.h"
#include "System.h"

using namespace rapidxml;

namespace proxy {

	System::System() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	System::~System() {
		UnInit();
	}

	int System::Init() {
		m_pRecord = new shared::DiskLogEx(Global::GetSetup()->GetLogRecordPath().c_str(), { "时间", "MT帐户","系统品种", "MT品种","LP品种","交易所","系统单号","订单类型","订单量","成交量","订单价","成交均价","最后成交价","处理时间(毫秒)","循环抛单(次数)","MT状态码","处理结果" });
		//m_pHedgeCacheObj = new shared::Memmap<shared::tradeorder::AdjustPositionKey, shared::TradeTaskRouteBridge>(\
		//	Global::GetSetup()->HedgeCacheMapPathName().c_str(), "sk.bridge.hedgecache.map", false);
		m_pQuoteFinal = new QuoteFinal();
		m_pServer = new Server();
		m_pSymbolCon = new SymbolCon(Global::GetSetup()->GetSymbolsPath());
		m_pOrderDispatch = new OrderDispatch(this);
		return 0;
	}
	int System::UnInit() {
		Global::GetSetup()->m_LPProcesesMapQ.iterate_clear(
			[&](const auto&, LPProcess* process, bool&, bool& itclear)
			{
				if (process->Status())
				{
					GetServer()->m_ServiceObjQ.iterate(
						[&](const auto& id, IServiceObj* obj)
						{
							if (shared::SystemIDType::ExchangeType(id) == process->ID())
							{
								obj->SendShutdown();
							}
						});
					process->Close();
				}
				itclear = true;
				SK_DELETE_PTR(process);
			});
		SK_DELETE_PTR(m_pServer);
		SK_DELETE_PTR(m_pQuoteFinal);
		SK_DELETE_PTR(m_pSymbolCon);
		SK_DELETE_PTR(m_pRecord);
		SK_DELETE_PTR(m_pOrderDispatch);
		return 0;
	}
	int System::Open() {
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
		m_Threads.emplace_back([this]() {RecordT(); });
		m_Threads.emplace_back([this]() {TradeOrderT(); });
		m_Threads.emplace_back([this]() {MarketDataProcessT(); });

		GetServer()->Open();

		if (m_pSymbolCon)
		{
			Global::SharedGet()->m_GatewayProductFinalQ.iterate(
				[&](const auto& sysProdID, auto& gatewayProds) {
					gatewayProds.iterate(
						[&](auto& gatewayProdObj) {
							Global::SharedGet()->m_MTSymbolProdconQ.push(gatewayProdObj.Symbol, gatewayProdObj);
						});
				});
		}

		//!@ 打印系统参数
		auto SystemSetup = Global::GetSetup()->SystemSetupGet();

		sk::stringa info;
		SystemSetup.Print(info);
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", info);

		m_IsOpen.store(true);
		return 0;
	}
	int System::Close() {
		if (!m_IsOpen.load()) return 0;

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "Exiting the system module . . . " << std::endl;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		GetServer()->Close();

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << "System module exit is complete . " << std::endl;

		Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("{}", "系统业务已停止 .");

		m_IsOpen.store(false);
		return 0;
	}

	bool System::Ready() const
	{
		bool result = true;
		Global::GetSetup()->m_ReadyContextMapQ.iterate(
			[&](const auto&, auto& status, const auto&, auto& itbreak)
			{
				if (!status)
				{
					result = false;
					itbreak = true;
				}
			});
		return result;
	}

	bool System::Suspend() const
	{
#if defined(_DEBUG)
		bool result = true;
		Global::GetSetup()->m_ReadyContextMapQ.iterate(
			[&](const auto& flag, auto& status, const auto&, auto& itbreak)
			{
				if (shared::SystemIDType::ServiceObjectType(flag) == (shared::SystemID)shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					if (status)
					{
						result = false;
						itbreak = true;
					}
				}
			});
#else
		bool result = true;
		Global::GetSetup()->m_ReadyContextMapQ.iterate(
			[&](const auto& flag, auto& status, const auto&, auto& itbreak)
			{
				if (shared::SystemIDType::ServiceObjectType(flag) == (shared::SystemID)shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER)
				{
					if (status)
					{
						result = false;
						itbreak = true;
					}
				}
			});
#endif
		return result;
	}

	void System::ProcessT()
	{
		const time_t ReminderInterval = 60000;
		time_t ReminderIntervalNext = 0;
		do {
			auto TImeCurrent = sk::Helper::TickCountGet<std::chrono::milliseconds>();
			if (ReminderIntervalNext == 0 || TImeCurrent - ReminderIntervalNext >= ReminderInterval && Suspend())
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}", "系统正在挂起状态...");
				ReminderIntervalNext = TImeCurrent;
			}
#if !defined(_DEBUG)
			if (!Suspend())
			{
				OpenLPProceses();
				//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}", "核心业务模块已接入，系统业务开始.");
			}
			else
			{
				CloseLPProceses();
				//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", "核心业务模块已退出，系统业务暂停.");
			}
#else
			OpenLPProceses();
#endif

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (!m_IsOpenT.load())
			{
				CloseLPProceses();
				break;
			}
		} while (1);
	}

	void System::RecordT()
	{
		do {
			auto record = Global::SharedGet()->m_RecordQ.pop();
			if (record)
			{
				RecordGet()->Write(*record);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) {
				break;
			}
		} while (1);
	}
	void System::TradeOrderT()
	{
		do
		{
			while (auto req = OrderDispatchGet()->m_RequestQ.pop())
			{
				bool result = false;
				GetServer()->m_ServiceObjQ.iterate(
					[&](const auto& ServiceObjID, auto& pServiceObj, auto& itbreak)
					{
						if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER == pServiceObj->ServiceObjFlagGet() && \
							shared::SystemIDType::ExchangeType(ServiceObjID) == shared::SystemIDType::ExchangeType(req->OrderRequest.ExchangePID))
						{
							itbreak = true;
							result = 0 == ServiceObjType<LPTradeOrderObj>(pServiceObj)->TradeOrderRequest(req->OrderRequest);
						}
					});
				if (!result)
				{
					req->OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_ORDER_REQUEST;
					OrderDispatchGet()->Ack(*req);
				}
			}

			while (auto ack = OrderDispatchGet()->m_ReplyQ.pop())
			{
				if (ack->OrderRequest.OrdRequestType & \
					static_cast<decltype(ack->OrderRequest.OrdRequestType)>(shared::SystemOrder::EnOrdRequestType::EN_ORD_REQUEST_TYPE_POSITION_ADJUST))
				{
					if (SKAPIRES_SUCCESS(ack->OrderRequest.SKRetcode))
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
							"自动调仓(完成)(TradeQty({}),TradePx({}),GatewayPID({:X}),ExchangePID({:X}))",
							ack->OrderRequest.LPTradeQty,
							ack->OrderRequest.TradeAvgPx,
							ack->OrderRequest.GatewayPID,
							ack->OrderRequest.ExchangePID
						);
					}
					else
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error(
							"自动调仓(失败)(TradeQty({}),TradePx({}),GatewayPID({:X}),ExchangePID({:X}))",
							ack->OrderRequest.LPTradeQty,
							ack->OrderRequest.TradeAvgPx,
							ack->OrderRequest.GatewayPID,
							ack->OrderRequest.ExchangePID
						);
					}

				}
				else
				{
					GetServer()->m_ServiceObjQ.search(
						ack->OrderRequest.ClientID,
						[&](const auto&, IServiceObj* pServiceObj)
						{
							if (pServiceObj->ServiceObjFlagGet() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER)
							{
								ServiceObjType<MTDealerObj>(pServiceObj)->TradeOrderResSend(ack->OrderRequest);
							}
						});
				}


				shared::SystemIDType::MTVolumeToLPVolume(
					ack->ExchangeProductCon.ContractSize,
					ack->GatewayProductCon.ContractSize,
					ack->ExchangeProductCon.MinOrderSize,
					ack->OrderRequest.MTTradeQty,
					ack->OrderRequest.LPTradeQty);

				//{ "时间", "MT帐户","系统品种", "MT品种","LP品种","交易所","系统单号","订单类型","订单量","成交量","订单价","成交均价","最后成交价","处理时间(毫秒)","循环抛单(次数)","MT状态码","处理结果" }
				CHAR szLogLine[2048] = { 0 };
				int nLogLine = sprintf_s(
					szLogLine,
					"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",
					sk::time::Time::GetSystimeStr(true).c_str(),
					tostringa(ack->OrderRequest.Key.MTLogin).c_str(),
					ack->SystemProductCon.Symbol.symbol,
					ack->GatewayProductCon.Symbol.symbol,
					ack->ExchangeProductCon.Symbol.symbol,
					sk::Log::Format("{:X}", shared::SystemIDType::ExchangeType(ack->ExchangeProductCon.ID)).c_str(),
					sk::Log::Format("{:X}", ack->OrderRequest.OrderID).c_str(),
					shared::tradeorder::Misc::StrOrdSideA(ack->OrderRequest.Key.OrdSide),
					tostringa(ack->OrderRequest.MTOrderQty).c_str(),
					tostringa(ack->OrderRequest.MTTradeQty).c_str(),
					sk::Helper::FloatToStrA(ack->OrderRequest.MTOrderPx, ack->GatewayProductCon.Digits).c_str(),
					sk::Helper::FloatToStrA(ack->OrderRequest.TradeAvgPx, ack->GatewayProductCon.Digits).c_str(),
					sk::Helper::FloatToStrA(ack->OrderRequest.TradeLastPx, ack->GatewayProductCon.Digits).c_str(),
					tostringa(sk::Helper::TickCountGet<std::chrono::milliseconds>() - ack->TimeCreate).c_str(),
					tostringa(ack->OrderRequest.LPProcessCount).c_str(),
					tostringa((UINT)ack->OrderRequest.MTRetcode).c_str(),
					SKAPIRES_DESC(ack->OrderRequest.SKRetcode));

				Global::SharedGet()->m_RecordQ.push(sk::packet(szLogLine, nLogLine));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) {
				break;
			}
		} while (1);
	}
	void System::MarketDataProcessT() {

		time_t TimeOld = 0;
		time_t TimeNow = sk::Helper::TickCountGet<std::chrono::milliseconds>();

		do {
			TimeNow = sk::Helper::TickCountGet<std::chrono::milliseconds>();
			if (!TimeOld)
			{
				TimeOld = TimeNow;
			}
			else if (TimeNow - TimeOld >= Global::GetSetup()->SystemSetupGet().TimeoutQuoteActive)
			{
				GetServer()->m_ServiceObjQ.iterate(
					[&](const auto&, IServiceObj* pServiceObj)
					{
						if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA == pServiceObj->ServiceObjFlagGet())
						{
							if (ServiceObjType<LPMarketDataObj>(pServiceObj)->MDUpdateTimestamp() > 0 && \
								TimeNow - ServiceObjType<LPMarketDataObj>(pServiceObj)->MDUpdateTimestamp() > Global::GetSetup()->SystemSetupGet().TimeoutQuoteActive)
							{
								Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("一定时间内,未接收到报价数据(({:X})", pServiceObj->ServiceObjFlagGet());
							}
						}
					});
				TimeOld = TimeNow;
			}

			if (!Global::SharedGet()->m_MarketDataUpdateQ.empty())
			{
				sk::packet quotes;
				do {
					auto pMD = Global::SharedGet()->m_MarketDataUpdateQ.pop();
					if (!pMD) break;
					//!@ 系统产品配置
					auto pSystemProdcon = SymbolConGet()->GetSystemProduct(shared::SystemIDType::SystemProductType(pMD->ID));
					if (!pSystemProdcon)
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
							"({})({})",
							__FUNCTION__,
							SKAPIRES_DESC(SKAPIRES::PROXY_RET_ERROR_PRODUCTCON_NOTFOUND_SYSTEM));
						continue;
					}
					//!@ 报价加入聚合因子
					FinalQuoteGet()->Push(*pMD);
					//!@ 聚合报价
					if (Global::GetSetup()->SystemSetupGet().AggregateQuoteSwitch == \
						shared::BridgeSystemSetup::EnAggregateQuoteSwitch::EN_AGGREGATE_QUOTE_FALG_ENABLE && \
						!pSystemProdcon->SetupEnable)
					{
						auto quote = FinalQuoteGet()->Get(pMD->ID);
						pMD->PriceBidFinal = quote.Bid.Price;
						pMD->PriceOfferFinal = quote.Ask.Price;
					}
					Global::SharedGet()->m_MarketDataFinalQ.push(shared::SystemIDType::SystemProductType(pMD->ID), *pMD);
					if (pSystemProdcon->SetupEnable)
					{
						sk::container::set<shared::SystemID> assigns;
						shared::ProductCon::UnMadeSystemSetup(pSystemProdcon->Setup, assigns);
						if (assigns.search(shared::SystemIDType::ExchangeType(pMD->ID)))
						{
							quotes.append((char*)pMD.get(), shared::marketdata::LENMARKETDATA);
						}
					}
					else
						quotes.append((char*)pMD.get(), shared::marketdata::LENMARKETDATA);

				} while (1);

				if (!quotes.empty()) {
					GetServer()->m_ServiceObjQ.iterate(
						[&](const auto& context, IServiceObj* pServiceObj)
						{
							switch (pServiceObj->ServiceObjFlagGet())
							{
							case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
							{
								pServiceObj->SendMarketDataUpdate(quotes);
							}break;
							case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
							{
								pServiceObj->SendMarketDataUpdate(quotes);
							}break;
							}
						});
				}
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) {
				break;
			}
		} while (1);
	}

	int System::ExecSystemCommand(const sk::stringa& cmdStr)
	{
		std::vector<sk::stringa> vtCmd;
		sk::Helper::OneUnknownParseA(cmdStr, ' ', vtCmd);
		if (vtCmd.empty()) return -1;

		__int64 cmdKey = 0;
		Global::GetSetup()->m_CommandsMapQ.iterate(
			[&](const auto& cmd, auto& cmdObj, const auto&, auto& iterate_break)
			{
				if (!memcmp(cmdObj.Input, cmdStr.c_str(), strlen(cmdObj.Input)))
				{
					cmdKey = cmd;
					iterate_break = true;
				}
			});
		if (cmdKey == 0) return -1;

		switch (cmdKey) {

		case 0x10000:
		{

		}break;

		case 0x10001:
		{//! 系统产品树初始化第一步,向LP请求产品 -->预初始化	
			ExecSystemProductsInit();
		}break;

		case 0x10002:
		{//! 打印系统运行时参数
			auto setup = Global::GetSetup()->SystemSetupGet();
			sk::stringa out;
			setup.Print(out);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}", out);
		}break;

		case 0x10003:
		{//! 系统产品树初始化第三步(Final),初始化系统产品树 此功能与 10001/10002合作使用
#if 0
			ExecSystemProductsInit();

			//! 踢除掉MT业务客户端，触发MT业务客户端重连初始化.
			GetServer()->m_NetClientQ.iterate(
				[&](const auto& pContext, auto& pClient)
				{
					switch (shared::EnClientContextType(pClient->ClientFieldGet()->ID))
					{
					case shared::EnClientContextType::EN_CLIENT_CONTEXT_TYPE_MTGATEWAY:
					case shared::EnClientContextType::EN_CLIENT_CONTEXT_TYPE_MTFEEDER:
					{

					}break;
					}

					pContext->Close();
					Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("Kick user at ([{}.{}]", pClient->ClientFieldGet()->Authcode, pContext->IPAddr());
				});
#endif
		}break;

		case 0x10004:
		{//! 推送报价启用聚合算法开关
			//if (m_EnableFinalPrice.load()) {
			//	m_EnableFinalPrice.store(false);
			//	Global::Log()->warn("{}", "关闭行情聚合推送");
			//}
			//else {
			//	m_EnableFinalPrice.store(true);
			//	Global::Log()->warn("{}", "开启行情聚合推送");
			//}
		}break;

		case 0x10005:
		{//! 查看/监视/检查/轮询 @对冲队列		
#if 0
			auto targetFile = Global::GetSetup()->HedgeCacheMapViewPathName();
			sk::Helper::CreateDirectoryA(targetFile.c_str());
			std::ofstream of(targetFile, std::ios::out | std::ios::trunc);
			if (of.is_open())
			{
				of << R"(新生联创（上海）)" << std::endl << std::endl;
				Global::SharedGet()->m_AutoAdjustPositionQ.iterate(
					[&](const shared::tradeorder::AdjustPositionKey& key, shared::TradeTaskRouteBridge& task)
					{
						auto text = sk::Log::Format(
							"Book At({:X}),OrderID({}),SpreadRange({}),CurrentSpreadRange({})",
							key.ExchangeID,
							key.OrderID.str(),
							task.HedgeSpreadRange,
							0
						);
						Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(text);
						of << text << std::endl;
					});
				of.close();
			}
#endif
		}break;

		default:
		{
			return -1;
		}
		}///switch
		return 0;
	}
	void System::ShowLPProcess()
	{
		Global::GetSetup()->m_LPProcesesMapQ.iterate(
			[&](const auto&, LPProcess* process)
			{
				if (process->Status())
				{
					GetServer()->m_ServiceObjQ.iterate(
						[&](const auto& id, IServiceObj* obj)
						{
							if (shared::SystemIDType::ExchangeType(id) == process->ID())
							{
								obj->SendShowWindow(m_LPProcessShowFlag.load() ? false : true);
							}
						});
				}
			});
		if (m_LPProcessShowFlag.load())
			m_LPProcessShowFlag.store(false);
		else
			m_LPProcessShowFlag.store(true);
	}
	int System::OpenLPProceses()
	{
		Global::GetSetup()->m_LPProcesesMapQ.iterate(
			[&](const auto&, auto& process)
			{
				if (!process->Status())
				{
					process->Open();
				}
			});
		return 0;
	}
	int System::CloseLPProceses()
	{
		Global::GetSetup()->m_LPProcesesMapQ.iterate(
			[&](const auto&, LPProcess* process)
			{
				if (process->Status())
				{
					GetServer()->m_ServiceObjQ.iterate(
						[&](const auto& id, IServiceObj* obj)
						{
							if (shared::SystemIDType::ExchangeType(id) == process->ID())
							{
								obj->SendShutdown();
							}
						});
					process->Close();
				}
			});
		return 0;
	}

	int System::ExecSystemProductsInit()
	{
		SymbolConGet()->Initialize();
		SymbolConGet()->Finalize();
		SymbolConGet()->Reload();

		Global::GetSetup()->SystemSetupSet(shared::BridgeSystemSetup());

		//! 踢除掉MT业务客户端，触发MT业务客户端重连初始化.
		GetServer()->m_ServiceObjQ.iterate(
			[&](const auto& ServiceObjID, IServiceObj* pServiceObj)
			{
				switch (pServiceObj->ServiceObjFlagGet())
				{
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
				{

				}break;
				}

				Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("Kick user at ([{:X}.{}]", pServiceObj->LoginFlag(), pServiceObj->Address());
				pServiceObj->Enable(false);
				pServiceObj->ContextGet()->Closed();
			});
		return 0;
	}

}///namespace proxy