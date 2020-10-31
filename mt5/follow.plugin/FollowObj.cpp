#include "stdafx.h"
#include "FollowObj.h"

namespace follow {
	FollowObj::FollowObj() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	FollowObj::~FollowObj() {
		UnInit();
	}

	int FollowObj::Init() {
		m_pDiskLog = new shared::DiskLogEx(Follow::SetupGet()->FollowRecordPath().c_str(), \
		{ "时间", "任务类型", "帐户", "数量", "价格", "持仓", "品种", "多空", "开平", "SL", "TP", "信号源", "信号持仓", "信号SL", "信号TP", "运行时间", "返回代码", "原因" });
		m_pFollowconObj = new shared::Memmap<shared::FollowKey, shared::FollowField>(Follow::SetupGet()->FollowconPathname().c_str(), "sk_followcon_mapping_obj", false);
		return 0;
	}

	int FollowObj::UnInit() {
		SK_DELETE_PTR(m_pDiskLog);
		SK_DELETE_PTR(m_pFollowconObj);
		return 0;
	}

	int FollowObj::Open() {
		if (m_IsOpen.load()) return 0;
		m_IsOpenT.store(true);
		if (Follow::SetupGet()->EnableLog())
		{
			Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("载入跟单配置({})", FollowconObj()->Size());
		}
		m_Threads.emplace_back([this]() {FollowLogT(); });
		/*m_Threads.emplace_back([this]() {FollowModifyT(); });*/
		m_Threads.emplace_back([this]() {FollowExecuteT(); });

		m_Timer.Open();
		m_Timer.TimerAppend(5000, this, OnTimer);
		m_Timer.TimerAppend(Follow::SetupGet()->SystemSetupGet().TimeAutoPositionCloseCheck, this, OnTimer);
		m_Timer.TimerAppend(Follow::SetupGet()->SystemSetupGet().TimeSLTPCorrectionInterval, this, OnTimer);
		m_Timer.TimerAppend(1000 * 60 * 60, this, OnTimer);

		m_IsOpen.store(true);
		return 0;
	}

	int FollowObj::Close() {
		if (!m_IsOpen.load()) return 0;
		m_IsOpenT.store(false);

		m_Timer.Close();

		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_IsOpen.store(false);
		return 0;
	}
	std::shared_ptr<shared::ConGroupField>\
		FollowObj::GroupConfigGet(__in const sk::uint64& login)
	{
		std::shared_ptr<shared::ConGroupField> result;
		Follow::SharedGet()->m_LoginGroupQ.search(
			login,
			[&](const auto& userLogin, auto& group)
			{
				Follow::SharedGet()->m_OnConGroupFieldQ.search(
					group,
					[&](const auto&, auto& conGroupObj)
					{
						result = std::make_shared<shared::ConGroupField>(conGroupObj);
					});
			});
		return result;
	}
	std::shared_ptr<shared::ConGroupSymbolField>\
		FollowObj::GroupConfigGet(
			__in const sk::uint64& login,
			__in const sk::stringw& symbol)
	{
		std::shared_ptr<shared::ConGroupSymbolField> result;
		//! 查询Login组
		Follow::SharedGet()->m_LoginGroupQ.search(
			login,
			[&](const auto& userLogin, auto& group)
			{
				//! 查询symbol路径
				auto symbolPath = Follow::SharedGet()->m_SymbolPathQ.search(symbol);
				if (symbolPath)
				{
					//! 查询组配置
					auto groupSymbolCons = Follow::SharedGet()->m_ConGroupGroupSymbolQ.search(group);
					if (groupSymbolCons)
					{
						//! 查询symbol路径组配置
						groupSymbolCons->iterate(
							[&](const auto& groupSymbolPath, auto& groupSymbolCon, const auto&, auto& itbreak)
							{
								if (groupSymbolPath == L"*") {
									result = std::make_shared<shared::ConGroupSymbolField>(groupSymbolCon);
									itbreak = true;
								}
								else if (groupSymbolPath.find(symbolPath->c_str()) != sk::stringw::npos) {
									result = std::make_shared<shared::ConGroupSymbolField>(groupSymbolCon);
									itbreak = true;
								}
							});
					}
				}
			});
		return result;
	}

	SKAPIRES
		FollowObj::FollowMadeIn
		(
			__inout shared::SignalField& signalObj,
			__in const shared::FollowField& followObj,
			__inout shared::RequestField& OutRequest
		)
	{
		//!@ 获取组配置/货币队配置
		auto result = shared::TradeFollowFix::FollowInFixFinal(
			signalObj,
			followObj,
			[&](auto& _ToAdjustSignalObj)
			{
				_ToAdjustSignalObj.FollowPositionTotal = \
					Follow::SharedGet()->m_OnPositionFieldQ.count(
						[&](const shared::PositionField& _obj)->bool
						{
							if (_obj.Login != followObj.LoginFollow)
							{
								return false;
							}
							return true;
						});
			},
			[&](const auto& _ObjSignalFinal, const auto& _ObjFollowFinal, auto& _ToAdjustRequestObj)->SKAPIRES
			{
				//! 检查帐户Login
				shared::UserField userObj;
				if (!Follow::SharedGet()->m_OnUserFieldQ.search(
					_ToAdjustRequestObj.Login,
					[&](const auto&, auto& UserObj) {
						userObj = UserObj;
					})) {
					//! 跟单帐号Login已经不存在
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_LOGIN_NULL;
				}

				//!说明 : 初始化请求数据
				//!注意 : 注意后面如果条件满足则修复该数据
				//!日期 : Wed Jul 1 01:16:41 UTC+0800 2020
				OutRequest = _ToAdjustRequestObj;

				//! 获取货币队的通用配置
				auto pConSymbolcon = Follow::SharedGet()->m_OnConSymbolFieldQ.search(_ToAdjustRequestObj.Symbol);
				//! 获取组配置
				auto pConGroupSymbolcon = FollowObj::GroupConfigGet(_ToAdjustRequestObj.Login, _ToAdjustRequestObj.Symbol);
#if 0
				INT SpreadDiff = 0;
				sk::real64 Point = 0;
				sk::real64 Multiply = 0;
				sk::real64 SpreadDiffBalance = 0;
				UINT64 VolumeStep = 0;
				UINT64 VolumeMax = 0;
				UINT64 VolumeMin = 0;
				UINT64 VolumeLimit = 0;
				if (IMTOrder::EnOrderType::OP_BUY_LIMIT == _ToAdjustRequestObj.Type) {
					if (m_pManager->GetDispatch()->IsReady()) {
						MTTickShort tick;
						if (MT_RET_OK == m_pManager->GetDispatch()->GetManagerApi()->TickLast(_ToAdjustRequestObj.Symbol, tick)) {
							_ToAdjustRequestObj.PriceOrder = tick.ask + Point * (SpreadDiff / 2 + SpreadDiffBalance);
						}
					}
				}
				else if (IMTOrder::EnOrderType::OP_SELL_LIMIT == _ToAdjustRequestObj.Type) {
					if (m_pManager->GetDispatch()->IsReady()) {
						MTTickShort tick;
						if (MT_RET_OK == m_pManager->GetDispatch()->GetManagerApi()->TickLast(_ToAdjustRequestObj.Symbol, tick)) {
							_ToAdjustRequestObj.PriceOrder = tick.bid - Point * ((SpreadDiff + 1) / 2 - SpreadDiffBalance);
						}
					}
				}
#endif

				SKAPIRES VolFixRet = SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
				if (pConGroupSymbolcon &&
					(pConGroupSymbolcon->VolumeStep > 0 && pConGroupSymbolcon->VolumeStep < MAXUINT64) &&
					(pConGroupSymbolcon->VolumeMax > 0 && pConGroupSymbolcon->VolumeMax < MAXUINT64) &&
					(pConGroupSymbolcon->VolumeMin > 0 && pConGroupSymbolcon->VolumeMin < MAXUINT64))
				{
					VolFixRet = shared::TradeFollowFix::FollowInVolumeFixFinal(
						_ToAdjustRequestObj.Volume,
						pConGroupSymbolcon->VolumeStep,
						pConGroupSymbolcon->VolumeMax,
						pConGroupSymbolcon->VolumeMin);
				}
				else if (pConSymbolcon) {
					VolFixRet = shared::TradeFollowFix::FollowInVolumeFixFinal(
						_ToAdjustRequestObj.Volume,
						pConSymbolcon->VolumeStep,
						pConSymbolcon->VolumeMax,
						pConSymbolcon->VolumeMin);
				}
				else {
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", SKAPIRES_DESC(SKAPIRES::TRADE_FOLLOWOBJ_ERROR_DATA_MISSING));
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_DATA_MISSING;
				}
				if (VolFixRet != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", SKAPIRES_DESC(VolFixRet));
					return VolFixRet;
				}

				_ToAdjustRequestObj.TypeTime = IMTOrder::ORDER_TIME_GTC;
				OutRequest = _ToAdjustRequestObj;
				return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
			});
		return result;
	}

	SKAPIRES
		FollowObj::FollowMadeOut
		(
			__inout shared::SignalField& signalObj,
			__in const shared::FollowField& followCon,
			__in const shared::PositionField& followObj,
			__inout shared::RequestField& requestOut
		)
	{
		auto result = shared::TradeFollowFix::FollowOutFixFinal(signalObj, followCon, followObj,
			[&](const auto& _ObjSignalFinal, const auto& _ObjFollowCon, const auto& _ObjFollowFinal, auto& _ToAdjustRequestObj)->SKAPIRES
			{
				//! 检查帐户Login
				shared::UserField userObj;
				if (!Follow::SharedGet()->m_OnUserFieldQ.search(
					_ToAdjustRequestObj.Login,
					[&](const auto&, auto& UserObj) {
						userObj = UserObj;
					})) {
					//! 跟单帐号Login已经不存在
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_LOGIN_NULL;
				}

				//! 获取货币队的通用配置
				auto pConSymbolcon = Follow::SharedGet()->m_OnConSymbolFieldQ.search(_ToAdjustRequestObj.Symbol);
				//! 获取组配置
				auto pConGroupSymbolcon = FollowObj::GroupConfigGet(_ToAdjustRequestObj.Login, _ToAdjustRequestObj.Symbol);

				SKAPIRES VolFixRet = SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
				if (pConGroupSymbolcon) {
					VolFixRet = shared::TradeFollowFix::FollowOutVolumeFixFinal(
						_ToAdjustRequestObj.Volume,
						pConGroupSymbolcon->VolumeStep,
						pConGroupSymbolcon->VolumeMax,
						pConGroupSymbolcon->VolumeMin);
				}
				else if (pConSymbolcon) {
					VolFixRet = shared::TradeFollowFix::FollowOutVolumeFixFinal(
						_ToAdjustRequestObj.Volume,
						pConSymbolcon->VolumeStep,
						pConSymbolcon->VolumeMax,
						pConSymbolcon->VolumeMin);
				}
				else {
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_DATA_MISSING;
				}
				if (VolFixRet != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
				{
					return VolFixRet;
				}
				_ToAdjustRequestObj.TypeTime = IMTOrder::ORDER_TIME_GTC;
				requestOut = _ToAdjustRequestObj;
				return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
			});
		return result;
	}


	void FollowObj::FollowExecuteT()
	{
		do {
			do {

				auto& SystemSetup = Follow::SetupGet()->SystemSetupGet();

				auto taskFollow = m_TradeFollowSignalQ.pop();
				if (!taskFollow || \
					Follow::SetupGet()->SystemSetupGet().SystemSwitch == shared::FollowSystemSetup::EnSystemSwitch::EN_SYSTEM_SWITCH_DISABLE)
				{
					break;
				}

				//!@ 对冲分解
				if (IMTDeal::EnDealEntry::ENTRY_INOUT == taskFollow->SignalDeal.Entry &&
					taskFollow->SignalType == (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW)
				{
					FollowGet()->PositionApiDataAt(
						taskFollow->SignalPosition.Position,
						[&](IMTPosition* positionObj)->MTAPIRES
						{
							auto PositionPrev = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION_PREV);

							if (PositionPrev > 0)
							{
								shared::serverplugin::OnTradeSignalField signalOut(*taskFollow), signalIn(*taskFollow);
								signalOut.SignalPosition.Position = PositionPrev;
								signalOut.SignalPosition.Volume = 0;
								signalOut.SignalDeal.Entry = IMTDeal::EnDealEntry::ENTRY_OUT;
								signalOut.SignalDeal.Volume = signalOut.SignalDeal.VolumeClosed;
								signalOut.SignalDeal.PositionID = PositionPrev;
								signalOut.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW;
								m_TradeFollowSignalQ.push(signalOut);

								signalIn.SignalPosition.Volume = 0;
								signalIn.SignalDeal.Entry = IMTDeal::EnDealEntry::ENTRY_IN;
								signalIn.SignalDeal.Volume = taskFollow->SignalPosition.Volume;
								signalIn.SignalDeal.PositionID = taskFollow->SignalPosition.Position;
								signalIn.SignalDeal.VolumeClosed = 0;
								signalIn.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW;
								m_TradeFollowSignalQ.push(signalIn);
							}
							return Follow::ApiDataSet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION_PREV, taskFollow->SignalPosition.Position);
						});
					continue;
				}

				RouteObj* pRouteTask = nullptr;

				switch (RouteObj::EnProcessTaskType(taskFollow->SignalType))
				{
				case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
				{
					m_FollowProcessRouteTaskReqIDQ.pop(
						shared::serverplugin::OnTradeSignalKey(taskFollow->InitRequest.Position, taskFollow->InitRequest.Type),
						[&](const auto&, RouteObj* routeObj)
						{
							routeObj->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_INTERNAL);
							pRouteTask = routeObj;
						},
						[&]()
						{
							pRouteTask = new RouteObj(Follow::SharedGet()->Serial(), *taskFollow);
							if (pRouteTask)
							{
								pRouteTask->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_EXTERNAL);
							}
						});
				}break;
				case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
				{
					m_FollowProcessRouteTaskReqIDQ.pop(
						shared::serverplugin::OnTradeSignalKey(taskFollow->SignalRequest.ID, taskFollow->SignalRequest.Type),
						[&](const auto&, RouteObj* routeObj)
						{
							routeObj->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_INTERNAL);
							pRouteTask = routeObj;
						},
						[&]()
						{
							pRouteTask = new RouteObj(Follow::SharedGet()->Serial(), *taskFollow);
							if (pRouteTask)
							{
								pRouteTask->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_EXTERNAL);
							}
						});
				}break;
				default:
				{
					m_FollowProcessRouteTaskReqIDQ.pop(
						shared::serverplugin::OnTradeSignalKey(taskFollow->SignalRequest.ID, taskFollow->SignalRequest.Type),
						[&](const auto&, RouteObj* routeObj)
						{
							routeObj->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_INTERNAL);
							pRouteTask = routeObj;
						},
						[&]()
						{
							pRouteTask = new RouteObj(Follow::SharedGet()->Serial(), *taskFollow);
							if (pRouteTask)
							{
								pRouteTask->SignalSourceType(RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_EXTERNAL);
							}
						});
				}break;
				}///switch


				if (!pRouteTask)
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("({})({})", __FUNCTION__, SKAPIRES_DESC(SKAPIRES::SYSTEM_ERROR_POINTER_NULL));
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					continue;
				}

				std::vector<shared::serverplugin::OnTradeSignalField> RequestFields;
				FollowProcessFollow(pRouteTask, *taskFollow, RequestFields);

				for (auto it = RequestFields.begin(); it != RequestFields.end();)
				{
					switch (pRouteTask->ProcessTaskType())
					{
					case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
					{
						m_PositionUpdateRouteQ.push(it->InitRequest.Position, *it);

						auto updateResult = FollowGet()->PositionApiDataAt(
							it->InitRequest.Position,
							[&](IMTPosition* positionAt)->MTAPIRES
							{
								if (it->PositionUpdateFlag & (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_SL)
								{
									positionAt->PriceSL(it->InitRequest.PriceSL);
								}
								if (it->PositionUpdateFlag & (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_TP)
								{
									positionAt->PriceTP(it->InitRequest.PriceTP);
								}
								return MT_RET_OK;
							});
						if (MT_RET_OK == updateResult)
						{
							m_FollowProcessRouteTaskReqIDQ.push(shared::serverplugin::OnTradeSignalKey(it->InitRequest.Position, it->InitRequest.Type), pRouteTask);
							++it;
							continue;
						}
						else
						{
							m_PositionUpdateRouteQ.pop(it->InitRequest.Position, [](const auto&, auto&) {});
						}
						it = RequestFields.erase(it);
					}break;
					case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
					{
						if ((it->InitRequest.Position > 0 && !pRouteTask->FollowPathOutAdd(it->InitRequest.Position)) ||
							it->InitRequest.Position == 0)
						{
							if (MT_RET_OK == FollowGet()->TradeRequest(it->InitRequest))
							{
								m_FollowProcessRouteTaskReqIDQ.push(shared::serverplugin::OnTradeSignalKey(it->InitRequest.ID, it->InitRequest.Type), pRouteTask);
								++it;
								continue;
							}
						}
						it = RequestFields.erase(it);
					}break;
					default:
					{
						if ((it->InitRequest.Position > 0 && !pRouteTask->FollowPathOutAdd(it->InitRequest.Position)) ||
							it->InitRequest.Position == 0)
						{
							if (MT_RET_OK == FollowGet()->TradeRequest(it->InitRequest))
							{
								m_FollowProcessRouteTaskReqIDQ.push(shared::serverplugin::OnTradeSignalKey(it->InitRequest.ID, it->InitRequest.Type), pRouteTask);
								++it;
								continue;
							}
						}
						it = RequestFields.erase(it);
					}break;
					}///switch
				}

				//!说明 : 确认批次路由任务未完成
				//!注意 : 返回继续 等待/执行
				//!日期 : Mon Sep 7 14:34:58 UTC+0800 2020
				if (!pRouteTask->PerformTask(*taskFollow, RequestFields))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					continue;
				}

				//!@ 打印
				auto logs = pRouteTask->Print();
				Follow::SharedGet()->m_FollowLogQ.push(logs);

				SK_DELETE_PTR(pRouteTask);
			} while (1);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) { break; }
		} while (1);
	}

	void FollowObj::FollowModifyT() {
		do
		{
			do
			{
				auto taskFollow = m_PositionFollowSignalQ.pop();
				if (!taskFollow) break;

				auto result = FollowProcessModify(*taskFollow);
				if (SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS != result)
				{

				}
			} while (1);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) { break; }
		} while (1);
	}

	void FollowObj::FollowLogT()
	{
		do
		{
			do
			{
				auto	record = Follow::SharedGet()->m_FollowLogQ.pop();
				if (!record)
				{
					break;
				}
				DiskLogGet()->Write(*record);
			} while (1);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) { break; }
		} while (1);
	}

	SKAPIRES
		FollowObj::FollowProcessRefollowIn
		(
			__in const shared::FollowField& followCon
		)
	{
		SKAPIRES result = SKAPIRES::FOLLOW_RET_ERROR;

		//!@ 清理/平仓 当前信号所有的持仓关系,并重新触发该信号
		sk::container::map<UINT64/*signal position id*/, shared::PositionField> SignalPositions;
		sk::container::multimap<UINT64/*signal position id*/, shared::PositionField> FollowPositions;

		Follow::SharedGet()->m_OnPositionPtrQ.iterate(
			[&](const auto& positionID, IMTPosition* positionObj)
			{
				shared::PositionField positionField;
				shared::Business::MakePositionField(positionObj, positionField);
				//! 信号源的持仓
				if (positionObj->Login() == followCon.LoginSignal)
				{
					SignalPositions.push(positionField.Position, positionField);
				}

				//! 清理(平仓)所有与该信号有关的所有持仓
				UINT64 signalLogin = 0;
				UINT64 signalPosition = 0;
				Follow::SharedGet()->m_PositionRelationQ.iterate(
					[&](const auto& signalPositionID, auto& followPositionIDs, const auto&, auto& itbreak)
					{
						if (followPositionIDs.search(positionObj->Position(), [](const auto&, auto&) {}))
						{
							Follow::SharedGet()->m_OnPositionFieldQ.search(
								signalPositionID,
								[&](const auto&, auto& signalPositionField)
								{
									signalLogin = signalPositionField.Login;
									signalPosition = signalPositionField.Position;
								});
							itbreak = true;
						}
					});
				if (signalLogin == followCon.LoginSignal)
				{
					FollowPositions.push(signalPosition, positionField);
				}
			});

		//!@ 追加跟单 --- 
		//!@ 先平掉跟单者跟随追加信号的现有持仓
		//!@ 重新按信号的单量下单

		FollowPositions.iterate(
			[&](const auto& signalPositionID, auto& followPositionObj)
			{
				shared::RequestField reqField;
				reqField.Login = followPositionObj.Login;
				reqField.Position = followPositionObj.Position;
				reqField.Volume = followPositionObj.Volume;
				sk::SafeCopyW(reqField.Symbol, followPositionObj.Symbol, _countof(reqField.Symbol));
				reqField.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE;
				reqField.Type = followPositionObj.Action == UINT(IMTPosition::EnPositionAction::POSITION_BUY) ? \
					UINT(IMTPosition::EnPositionAction::POSITION_SELL) : UINT(IMTPosition::EnPositionAction::POSITION_BUY);

				if (MT_RET_OK == FollowGet()->TradeRequest(reqField))
				{
					result = SKAPIRES::FOLLOW_RET_SUCCESS_POSITION_PATCH;
					if (Follow::SetupGet()->EnableLog())
					{
						Follow::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info("补仓-平仓({}->{})", followPositionObj.Position, signalPositionID);
					}
				}
				else
				{
					result = SKAPIRES::FOLLOW_RET_ERROR_POSITION_PATCH;
					if (Follow::SetupGet()->EnableLog())
					{
						Follow::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->info("补仓-平仓({}->{})", followPositionObj.Position, signalPositionID);
					}
				}
			});

		SignalPositions.iterate(
			[&](const auto& signalPositionID, shared::PositionField& signalPositionObj)
			{
				shared::serverplugin::OnTradeSignalField signalIn;
				signalIn.SignalPosition = signalPositionObj;
				signalIn.SignalDeal.Entry = IMTDeal::EnDealEntry::ENTRY_IN;
				signalIn.SignalDeal.Order = signalPositionObj.Position;
				signalIn.SignalDeal.Volume = signalPositionObj.Volume;
				signalIn.SignalDeal.PositionID = signalPositionObj.Position;
				signalIn.SignalDeal.Action = signalPositionObj.Action;
				signalIn.SignalDeal.Digits = signalPositionObj.Digits;
				signalIn.SignalDeal.Login = signalPositionObj.Login;
				signalIn.SignalDeal.PriceSL = signalPositionObj.PriceSL;
				signalIn.SignalDeal.PriceTP = signalPositionObj.PriceTP;
				signalIn.SignalDeal.Price = signalPositionObj.PriceCurrent;

				signalIn.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN;
				signalIn.Followcon = followCon;

				sk::SafeCopyW(signalIn.SignalDeal.Symbol, signalPositionObj.Symbol, _countof(signalIn.SignalDeal.Symbol));
				if (Follow::SetupGet()->EnableLog())
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("补仓-跟单(login({})->(login({}),position({}))", followCon.LoginFollow, signalPositionObj.Login, signalPositionObj.Position);
				}
				m_TradeFollowSignalQ.push(signalIn);
			});
		return result;
	}


	SKAPIRES FollowObj::FollowProcessPatchIn()
	{
		SKAPIRES result = SKAPIRES::FOLLOW_RET_SUCCESS;
		if (FollowconObj()->Empty())
			return SKAPIRES::FOLLOW_RET_ERROR_FOLLOWCON_EMPTY;
		FollowconObj()->Iterate(
			[&](const auto&, shared::FollowField& followCon)
			{
				if (shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE == followCon.FollowAddToFlag)
				{
					Follow::SharedGet()->m_OnPositionPtrQ.iterate(
						[&](const auto& positionID, IMTPosition* positionObj)
						{
							if (positionObj->Login() == followCon.LoginSignal)
							{
								shared::serverplugin::OnTradeSignalField TradeSignalSignal;
								TradeSignalSignal.Followcon = followCon;
								TradeSignalSignal.SignalDeal.PositionID = positionID;
								TradeSignalSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN;
								m_TradeFollowSignalQ.push(TradeSignalSignal);
							}
						});
				}
			});
		return result;
	}

	SKAPIRES FollowObj::FollowProcessPatchIn(__in const shared::FollowField& followCon)
	{
		SKAPIRES result = SKAPIRES::FOLLOW_RET_SUCCESS;
		if (shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE == followCon.FollowAddToFlag)
		{
			Follow::SharedGet()->m_OnPositionPtrQ.iterate(
				[&](const auto& positionID, IMTPosition* positionObj)
				{
					if (positionObj->Login() == followCon.LoginSignal)
					{
						shared::serverplugin::OnTradeSignalField TradeSignalSignal;
						TradeSignalSignal.Followcon = followCon;
						TradeSignalSignal.SignalDeal.PositionID = positionID;
						TradeSignalSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN;
						m_TradeFollowSignalQ.push(TradeSignalSignal);
					}
				});
		}
		return result;
	}

	SKAPIRES FollowObj::FollowProcessPathSLTP()
	{
		//!获取开始SLTP跟单功能的跟单关系
		sk::container::map<shared::FollowKey, shared::FollowField> TargetFollowcons;
		FollowconObj()->Iterate(
			[&](const auto& followKey, shared::FollowField& followCon)
			{
				if (followCon.PointSLTPFlag == shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE)
				{
					TargetFollowcons.push(followKey, followCon);
				}
			});

		if (TargetFollowcons.empty())
			return SKAPIRES::FOLLOW_RET_ERROR_FOLLOWCON_EMPTY;

		Follow::SharedGet()->m_OnPositionPtrQ.iterate(
			[&](const auto& positionID, auto& positionObj)
			{
				auto signalPositionID = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION);
				Follow::SharedGet()->m_OnPositionFieldQ.search(
					signalPositionID,
					[&](const auto&, shared::PositionField& signalPositionField)
					{
						shared::FollowKey followKey(tostringa(signalPositionField.Login).append("@").append(tostringa(positionObj->Login())).c_str());
						TargetFollowcons.search(
							followKey,
							[&](const auto&, shared::FollowField& followCon)
							{
								shared::serverplugin::OnTradeSignalField TradeSignalSignal;
								TradeSignalSignal.InitRequest.Position = positionObj->Position();
								TradeSignalSignal.InitRequest.Login = positionObj->Login();
								TradeSignalSignal.InitRequest.Volume = positionObj->Volume();
								TradeSignalSignal.InitRequest.Type = positionObj->Action();
								TradeSignalSignal.InitRequest.PriceOrder = signalPositionField.PriceOpen;
								sk::SafeCopyW(TradeSignalSignal.InitRequest.Symbol, positionObj->Symbol(), _countof(TradeSignalSignal.InitRequest.Symbol));

								if (positionObj->PriceSL() != signalPositionField.PriceSL || positionObj->PriceTP() != signalPositionField.PriceTP)
								{
									UINT64 Point = followCon.PointSLTP;
									UINT64 FollowSL = static_cast<UINT64>(positionObj->PriceSL() * pow(10, positionObj->Digits()));
									UINT64 FollowTP = static_cast<UINT64>(positionObj->PriceTP() * pow(10, positionObj->Digits()));
									UINT64 SignalSL = static_cast<UINT64>(signalPositionField.PriceSL * pow(10, signalPositionField.Digits));
									UINT64 SignalTP = static_cast<UINT64>(signalPositionField.PriceTP * pow(10, signalPositionField.Digits));


									if (SignalSL != FollowSL)
									{
										if (SignalSL == 0)
										{
											TradeSignalSignal.PositionUpdateFlag |= (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_SL;
											TradeSignalSignal.InitRequest.PriceSL = 0;
										}
										else if (SignalSL + Point != FollowSL)
										{
											TradeSignalSignal.PositionUpdateFlag |= (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_SL;
											TradeSignalSignal.InitRequest.PriceSL = signalPositionField.PriceSL + followCon.PointSLTP / pow(10, positionObj->Digits());
										}
									}

									if (SignalTP != FollowTP)
									{
										if (SignalTP == 0)
										{
											TradeSignalSignal.PositionUpdateFlag |= (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_TP;
											TradeSignalSignal.InitRequest.PriceSL = 0;
										}
										else if (SignalTP + Point != FollowTP)
										{
											TradeSignalSignal.PositionUpdateFlag |= (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_TP;
											TradeSignalSignal.InitRequest.PriceTP = signalPositionField.PriceTP + followCon.PointSLTP / pow(10, positionObj->Digits());
										}
									}

									if (TradeSignalSignal.PositionUpdateFlag & (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_TP ||
										TradeSignalSignal.PositionUpdateFlag & (UINT64)shared::serverplugin::OnTradeSignalField::EnPositionUpdateFlag::EN_POSITION_UPDATE_PRICE_SL)
									{
										TradeSignalSignal.SignalPosition = signalPositionField;
										TradeSignalSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY;
										m_TradeFollowSignalQ.push(TradeSignalSignal);
									}
								}
							});
					});
			});


		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	SKAPIRES FollowObj::FollowProcessPatchOut()
	{
		m_FollowPatchOutQ.iterate_clear(
			[&](const auto& followPositionID, auto& signalPositionField, auto& itbreak, auto& itclear)
			{
				if (!Follow::SharedGet()->m_OnPositionPtrQ.search(followPositionID, [](const auto&, auto&) {}))
				{
					itclear = true;
				}
			});

		Follow::SharedGet()->m_OnPositionPtrQ.iterate(
			[&](const auto& positionID, auto& positionObj)
			{
				shared::PositionField signalPositionField;
				signalPositionField.ApiDataPosition = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION);
				signalPositionField.ApiDataLogin = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN);
				signalPositionField.ApiDataDeal = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL);
				signalPositionField.ApiDataOrder = Follow::ApiDataGet(positionObj, shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER);
				signalPositionField.Position = signalPositionField.ApiDataPosition;
				signalPositionField.Login = signalPositionField.ApiDataLogin;
				if (signalPositionField.Position > 0)
				{
					if (!Follow::SharedGet()->m_OnPositionFieldQ.search(signalPositionField.Position, [](const auto&, auto&) {}))
					{
						m_FollowPatchOutQ.push(positionID, signalPositionField);
					}
				}
			});

		m_FollowPatchOutQ.iterate(
			[&](const auto& followPositionID, auto& signalPositionField)
			{
				Follow::SharedGet()->m_OnPositionFieldQ.search(
					followPositionID,
					[&](const auto&, auto& positionField)
					{
						shared::serverplugin::OnTradeSignalField TradeSignalSignal;
						TradeSignalSignal.SignalPosition = signalPositionField;

						TradeSignalSignal.InitRequest.Login = positionField.Login;
						TradeSignalSignal.InitRequest.Position = positionField.Position;
						TradeSignalSignal.InitRequest.Volume = positionField.Volume;
						sk::SafeCopyW(TradeSignalSignal.InitRequest.Symbol, positionField.Symbol, _countof(TradeSignalSignal.InitRequest.Symbol));
						TradeSignalSignal.InitRequest.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE;
						TradeSignalSignal.InitRequest.Type = positionField.Action == UINT(IMTPosition::EnPositionAction::POSITION_BUY) ? \
							UINT(IMTPosition::EnPositionAction::POSITION_SELL) : UINT(IMTPosition::EnPositionAction::POSITION_BUY);
						TradeSignalSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE;
						m_TradeFollowSignalQ.push(TradeSignalSignal);
					});
			});

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	SKAPIRES FollowObj::FollowProcessModify
	(
		__in const shared::PositionField& signalPosition/*接收到的信号是修改之前的持仓快照*/
	)
	{
		//!@
		//!@ 新生联创(R) (上海)  -----  [ SL、TP、***跟单系统 www.skstu.com ]
		//!@

		shared::PositionField Signal;

		UINT UpdateFlag = UINT(shared::FollowRouteField::EnPositionUpdateFlag::EN_POSITION_MODIFY_FLAG_NULL);

		Follow::SharedGet()->m_OnPositionFieldQ.search(
			signalPosition.Position,
			[&](const auto&, auto& positionField)
			{
				if (signalPosition.PriceSL != positionField.PriceSL)
				{
					UpdateFlag |= UINT(shared::FollowRouteField::EnPositionUpdateFlag::EN_POSITION_MODIFY_FALG_PRICE_SL);
				}
				if (signalPosition.PriceTP != positionField.PriceTP)
				{
					UpdateFlag |= UINT(shared::FollowRouteField::EnPositionUpdateFlag::EN_POSITION_MODIFY_FALG_PRICE_TP);
				}
				if (UpdateFlag != UINT(shared::FollowRouteField::EnPositionUpdateFlag::EN_POSITION_MODIFY_FLAG_NULL))
				{
					Signal = positionField;
				}
			});

		if (UpdateFlag == UINT(shared::FollowRouteField::EnPositionUpdateFlag::EN_POSITION_MODIFY_FLAG_NULL))
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
		}

		sk::container::map<UINT64/*follow position id*/, shared::PositionField/*follow position field*/> FollowMods;

		Follow::SharedGet()->m_PositionRelationQ.search(
			signalPosition.Position,
			[&](const auto& dealID, auto& deals)
			{
				deals.iterate(
					[&](const auto& followPositionID, auto& deal)
					{
						Follow::SharedGet()->m_OnPositionFieldQ.search(
							followPositionID,
							[&](const auto& positionID, auto& positionField)
							{
								FollowMods.push(positionID, positionField);
							});
					});
			});

		FollowMods.iterate(
			[&](const UINT64& followPositionID, shared::PositionField& followPositionField)
			{
				auto Followkey = shared::FollowField::Key(Signal.Login, followPositionField.Login);
				auto pFollowcon = FollowconObj()->Search(Followkey);
				if (pFollowcon)
				{
					shared::FollowRouteField route;

					auto result = shared::TradeFollowFix::FollowUpdatePositionFixFinal(Signal, followPositionField, *pFollowcon);

					{//!@ 跟单日志 -- 创建(初始化)
						route.FollowLog.SignalObj.Action = signalPosition.Action;
						route.FollowLog.SignalObj.Login = signalPosition.Login;
						route.FollowLog.SignalObj.Position = signalPosition.Position;
						route.FollowLog.SignalObj.Price = signalPosition.PriceCurrent;
						route.FollowLog.SignalObj.PriceSL = signalPosition.PriceSL;
						route.FollowLog.SignalObj.PriceTP = signalPosition.PriceTP;
						route.FollowLog.SignalObj.Volume = signalPosition.Volume;
						sk::SafeCopyW(route.FollowLog.SignalObj.Symbol, signalPosition.Symbol, _countof(route.FollowLog.SignalObj.Symbol));
						route.FollowLog.FollowObj.Login = followPositionField.Login;
						route.FollowLog.FollowObj.Volume = followPositionField.Volume;
						route.FollowLog.FollowObj.Action = followPositionField.Action;
						sk::SafeCopyW(route.FollowLog.FollowObj.Symbol, followPositionField.Symbol, _countof(route.FollowLog.FollowObj.Symbol));
						route.FollowLog.TimeUpdate = ::time(0);
					}

					if (SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS == result)
					{
						route.Follow = *pFollowcon;
						route.PositionFollow = followPositionField;
						route.PositionSignal = Signal;
						m_FollowProcessPosIDRouteQ.push(followPositionField.Position, route);
						if (Follow::SetupGet()->EnableLog())
						{
							Follow::Log(sk::EnLogType::EN_LOG_TYPE_SIGNAL)->info("更新持仓任务:[position({}->{}),SL({}->{}),TP({}->{})]",
								followPositionID,
								Signal.Position,
								followPositionField.PriceSL,
								Signal.PriceSL,
								followPositionField.PriceTP,
								Signal.PriceTP);
						}

						auto updateResult = FollowGet()->PositionApiDataAt(
							followPositionField.Position,
							[&](IMTPosition* positionAt)->MTAPIRES
							{
								positionAt->PriceSL(followPositionField.PriceSL);
								positionAt->PriceTP(followPositionField.PriceTP);
								return MT_RET_OK;
							});

						if (MT_RET_OK != updateResult)
						{
							//!@"时间","任务类型","帐户","数量","价格","持仓","品种","多空","开平","SL","TP","信号源","信号持仓","信号SL","信号TP","运行时间","返回代码","原因"
							auto record = sk::Log::Format(
								"{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
								sk::time::Time::TimestampToTimeStr(sk::Helper::TickCountGet<std::chrono::milliseconds>(), true),
								"编辑定单",
								followPositionField.Login,
								followPositionField.Volume,
								sk::Helper::FloatToStrA(followPositionField.PriceOpen, 5),
								followPositionField.Position,
								sk::StringConvert::WStringToMBytes(followPositionField.Symbol),
								"-",
								"-",
								sk::Helper::FloatToStrA(followPositionField.PriceSL, 5),
								sk::Helper::FloatToStrA(followPositionField.PriceTP, 5),
								Signal.Login,
								Signal.Position,
								sk::Helper::FloatToStrA(Signal.PriceSL, 5),
								sk::Helper::FloatToStrA(Signal.PriceTP, 5),
								"-",
								updateResult,
								sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(updateResult))
							);
							Follow::SharedGet()->m_FollowLogQ.push(record);


							if (Follow::SetupGet()->EnableLog())
							{
								Follow::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->info(
									"更新持仓失败(followcon({}),关系({}->{})) desc({}.{})",
									Followkey.Get(),
									followPositionID,
									Signal.Position,
									updateResult,
									sk::StringConvert::WStringToMBytes(SMTFormat::FormatError(updateResult)));
							}

						}
					}
					else
					{
						Follow::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->info("{}", SKAPIRES_DESC(result));
					}
				}
				else
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->info("更新持仓 未查询到跟单配置(followcon({}),关系({}->{}))", Followkey.Get(), followPositionID, Signal.Position);
				}
			});

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	SKAPIRES FollowObj::FollowProcessFollow
	(
		__in RouteObj* pRouteObj,
		__inout shared::serverplugin::OnTradeSignalField& nowTradeSignalObj,
		__out std::vector<shared::serverplugin::OnTradeSignalField>& reqFields
	)
	{
		reqFields.clear();
		SKAPIRES result = SKAPIRES::FOLLOW_RET_SUCCESS;
		if (!pRouteObj)
		{
			Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("({})({})", __FUNCTION__, "pRouteObj==nullptr");
			return SKAPIRES::FOLLOW_RET_ERROR;
		}

		switch (pRouteObj->ProcessTaskType())
		{
		case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW:
		{
			shared::SignalField Signal = nowTradeSignalObj.toSignalField(FollowGet()->ServerTimeDiff());

			switch (Signal.Entry)
			{
			case IMTDeal::EnDealEntry::ENTRY_IN:
			{
				Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Signal({})", Signal.Login);

				if (pRouteObj->ProcessTaskSwicth() == RouteObj::EnProcessTaskSwicth::EN_PROCESS_TASK_SWITCH_DISABLE)
					break;

				if (pRouteObj->SignalSourceType() == RouteObj::EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_INTERNAL && \
					pRouteObj->InitialSignal().SignalDeal.Login == nowTradeSignalObj.SignalDeal.Login)
				{
					pRouteObj->ProcessTaskSwicth(RouteObj::EnProcessTaskSwicth::EN_PROCESS_TASK_SWITCH_DISABLE);
				}


				bool IsFollowContinue = true;
				auto FollowIns = FollowconObj()->Search(
					[&](auto& followcon)->bool
					{
						return followcon.LoginSignal == Signal.Login;
					});

				if (!FollowIns)
					break;

				FollowIns->iterate(
					[&](const auto&, auto& followcon)
					{
						auto result = FollowMadeIn(Signal, followcon, nowTradeSignalObj.InitRequest);
						if (Follow::SetupGet()->SystemSetupGet().FollowToSignalExchangeSwitch == \
							shared::FollowSystemSetup::EnFollowToSignalExchangeSwitch::EN_FOLLOWTO_SIGNAL_EXCHANGE_SWITCH_ENABLE && \
							Signal.ExternalID[0])
						{
							sk::SafeCmpW(nowTradeSignalObj.InitRequest.PositionExternalID, Signal.ExternalID, _countof(nowTradeSignalObj.InitRequest.PositionExternalID));
						}
						if (result == SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
						{
							reqFields.emplace_back(nowTradeSignalObj);
						}
					});			
			}break;
			case IMTDeal::EnDealEntry::ENTRY_OUT:
			{
				sk::container::map<UINT64/*follow order id*/, shared::PositionField/*follow position field*/> FollowOuts;
				Follow::SharedGet()->m_PositionRelationQ.search(
					nowTradeSignalObj.SignalPosition.Position,
					[&](const auto& dealID, auto& deals)
					{
						deals.iterate(
							[&](const auto& followPositionID, auto& deal)
							{
								Follow::SharedGet()->m_OnPositionFieldQ.search(
									followPositionID,
									[&](const auto& positionID, auto& positionField)
									{
										FollowOuts.push(positionID, positionField);
									});
							});
					});
				FollowOuts.iterate(
					[&](const auto& followPositionKey, auto& followPositionObj)
					{
						auto pFollowCon = FollowconObj()->Search(shared::FollowField::Key(Signal.Login, followPositionObj.Login));
						shared::FollowField followCon;
						if (pFollowCon)
						{
							followCon = *pFollowCon;
						}
						auto result = FollowMadeOut(Signal, followCon, followPositionObj, nowTradeSignalObj.InitRequest);
						if (result == SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
						{
							reqFields.emplace_back(nowTradeSignalObj);
						}
					});
			}break;
			}///switch
		}break;
		case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
		{
			if (pRouteObj->ProcessTaskSwicth() == RouteObj::EnProcessTaskSwicth::EN_PROCESS_TASK_SWITCH_DISABLE)
				break;

			//!@ 循环跟单检测
			if (pRouteObj->InitialSignal().Followcon.Equal(nowTradeSignalObj.Followcon) && nowTradeSignalObj.SignalDeal.Login > 0)
			{
				pRouteObj->ProcessTaskSwicth(RouteObj::EnProcessTaskSwicth::EN_PROCESS_TASK_SWITCH_DISABLE);
			}
			sk::container::map<shared::FollowKey, shared::FollowField> FollowCons;
			//!@ 信号任务
			if (pRouteObj->InitialSignal().Followcon.Equal(nowTradeSignalObj.Followcon))
			{
				FollowCons.push(pRouteObj->InitialSignal().Followcon.GetKey(), pRouteObj->InitialSignal().Followcon);
			}
			else if (nowTradeSignalObj.SignalRequest.ID > 0)
			{
				auto pFollowCons = FollowconObj()->Search(
					[&](auto& followcon)->bool
					{
						return followcon.LoginSignal == nowTradeSignalObj.SignalDeal.Login;
					});
				if (pFollowCons && !pFollowCons->empty())
					FollowCons = *pFollowCons;
			}

			FollowCons.iterate(
				[&](const auto& followKey, auto& followCon)
				{
					std::vector<shared::FollowField> signals;
					FollowPatchGo(followCon, nowTradeSignalObj.SignalDeal.PositionID, signals, reqFields);
					for (const auto& signal : signals)
					{
						if (shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE == signal.FollowAddToFlag)
						{
							shared::serverplugin::OnTradeSignalField TradeSignalSignal;
							TradeSignalSignal.Followcon = signal;
							TradeSignalSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN;
							m_TradeFollowSignalQ.push(TradeSignalSignal);
						}
					}
				});
		}break;
		case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE:
		{
			if (!Follow::SharedGet()->m_OnPositionPtrQ.search(
				nowTradeSignalObj.InitRequest.Position,
				[&](const auto&, auto& position)
				{
					reqFields.emplace_back(nowTradeSignalObj);
				}))
			{
				m_FollowPatchOutQ.pop(nowTradeSignalObj.InitRequest.Position, [](const auto&, auto&) {});
			}
		}break;
		case RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
		{
			if (!m_PositionUpdateRouteQ.pop(nowTradeSignalObj.InitRequest.Position, [&](const auto&, auto&) {}))
			{
				reqFields.emplace_back(nowTradeSignalObj);
			}
		}break;
		default:
		{
			result = SKAPIRES::FOLLOW_RET_ERROR_SIGNAL_TYPE;
			Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("({})({})", SKAPIRES_DESC(result), (UINT)nowTradeSignalObj.SignalType);
		}break;
		}

		return result;
	}

	SKAPIRES FollowObj::FollowPatchGo(const shared::FollowField& signal, const UINT64& position, std::vector<shared::FollowField>& signals, std::vector<shared::serverplugin::OnTradeSignalField>& patchs)
	{
		//!@ 当前信号所有的持仓
		sk::container::map<UINT64/*signal position id*/, shared::PositionField> SignalPositions;
		//!@ 信号源持仓对应跟单者的持仓
		sk::container::map<UINT64/*signal position id*/, shared::PositionField> FollowPositions;

		Follow::SharedGet()->m_OnPositionPtrQ.iterate(
			[&](const auto& positionID, IMTPosition* positionObj)
			{
				shared::PositionField positionField;
				shared::Business::MakePositionField(positionObj, positionField);
				//! 信号源持仓
				if (positionObj->Login() == signal.LoginSignal && positionObj->Position() == position)
				{
					SignalPositions.push(positionField.Position, positionField);
				}
				//! 跟单者持仓
				else if (positionObj->Login() == signal.LoginFollow)
				{
					FollowPositions.push(positionField.Position, positionField);
				}
			});

		SignalPositions.iterate_clear(
			[&](const auto& signalPosition, auto& signalPositionField, bool&, bool& itclear)
			{
				Follow::SharedGet()->m_PositionRelationQ.search(
					signalPosition,
					[&](const auto&, auto& followPositionIDs)
					{
						followPositionIDs.iterate(
							[&](const auto& followPositionID, auto& deal, auto& _itbreak)
							{
								FollowPositions.search(
									followPositionID,
									[&](const auto&, auto& followPositionField)
									{
										itclear = true;
										_itbreak = true;
										auto pFollowcons = FollowconObj()->Search(
											[&](auto& followcon)->bool
											{
												return followcon.LoginSignal == followPositionField.Login;
											});
										if (pFollowcons)
										{
											pFollowcons->iterate(
												[&](const auto&,auto& obj) 
												{
													signals.emplace_back(obj);
												});
										}
									});
							});
					});
			});


		SignalPositions.iterate(
			[&](const auto& signalPositionID, shared::PositionField& signalPositionObj)
			{
				shared::serverplugin::OnTradeSignalField tradeSignal;
				tradeSignal.Followcon = signal;
				tradeSignal.SignalPosition = signalPositionObj;
				tradeSignal.SignalDeal.Entry = IMTDeal::EnDealEntry::ENTRY_IN;
				tradeSignal.SignalDeal.Order = signalPositionObj.Position;
				tradeSignal.SignalDeal.Volume = signalPositionObj.Volume;
				tradeSignal.SignalDeal.PositionID = signalPositionObj.Position;
				tradeSignal.SignalDeal.Action = signalPositionObj.Action;
				tradeSignal.SignalDeal.Digits = signalPositionObj.Digits;
				tradeSignal.SignalDeal.Login = signalPositionObj.Login;
				tradeSignal.SignalDeal.PriceSL = signalPositionObj.PriceSL;
				tradeSignal.SignalDeal.PriceTP = signalPositionObj.PriceTP;
				tradeSignal.SignalDeal.Price = signalPositionObj.PriceCurrent;
				Follow::SharedGet()->m_PositionDealQ.search(
					signalPositionID,
					[&](const auto&, auto& deal)
					{
						tradeSignal.SignalDeal.Deal = deal;
					});
				sk::SafeCopyW(tradeSignal.SignalDeal.Symbol, signalPositionObj.Symbol, _countof(tradeSignal.SignalDeal.Symbol));
				auto result = FollowMadeIn(tradeSignal.toSignalField(), tradeSignal.Followcon, tradeSignal.InitRequest);
				if (result == SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
				{
					patchs.emplace_back(tradeSignal);
				}
			});

		return SKAPIRES::FOLLOW_RET_SUCCESS;

	}

	int FollowObj::OnTimer(const sk::int64& _interval, const sk::intptr& _core_ptr, const sk::int64& _cur_time) {
		auto pFollowObj = reinterpret_cast<FollowObj*>(_core_ptr);

		if (_interval == 5000)
		{
			do
			{
				auto	pTask = pFollowObj->m_FollowPatchInQ.pop();
				if (!pTask) {
					break;
				}

				auto result = pFollowObj->FollowProcessPatchIn(*pTask);
				if (!SKAPIRES_SUCCESS(result))
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_FAILED)->info("{}", SKAPIRES_DESC(result));
				}
			} while (1);


			//!说明 : 推送日志记录
			//!注意 : 
			//!日期 : Mon Sep 7 19:44:24 UTC+0800 2020
			auto logs = pFollowObj->DiskLogGet()->PopNewestUpdate();
			if (!logs.empty())
			{
				sk::packet updates;
				for (auto& log : logs)
				{
					updates.append(log).append("@");
				}
				if (!updates.empty())
					updates.pop_back();

				if (!updates.empty())
				{
					Follow::ServerGet()->m_NetClientQ.iterate(
						[&](const auto& flag, IServiceObj* pServiceObj)
						{
							if (pServiceObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
							{
								ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordUpdate(updates);
							}
						});
				}
			}

		}///if (_interval == 5000)

		if (_interval == Follow::SetupGet()->SystemSetupGet().TimeAutoPositionCloseCheck &&
			shared::FollowSystemSetup::EnAutoPositionCloseSwitch::EN_AUTO_POSITION_CLOSE_SWITCH_ENABLE == \
			Follow::SetupGet()->SystemSetupGet().AutoPositionCloseSwitch)
		{
			pFollowObj->FollowProcessPatchOut();
		}

		if (_interval == Follow::SetupGet()->SystemSetupGet().TimeSLTPCorrectionInterval &&
			shared::FollowSystemSetup::EnCorrectionSLTPSwitch::EN_CORRECTION_SLTP_SWITCH_ENABLE == \
			Follow::SetupGet()->SystemSetupGet().CorrectionSLTPSwitch)
		{
			pFollowObj->FollowProcessPathSLTP();
		}

		if (_interval == 1000 * 60 * 60)
		{
			auto checkTime = ::time(0);
			if (FollowGet()->ServerApi())
			{
				checkTime = FollowGet()->ServerApi()->TimeCurrent();
			}
			FollowGet()->License(checkTime);
		}


		return 0;
	}

}///namespace follow