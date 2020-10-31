#include "stdafx.h"
#include "FollowPlugin.h"

#define ENABLE_ONMSG_SYNC_SEND 0

#define MT_MSG_PRINT(obj)\
if(obj)\
{\
if (Global::EnableRuntimeLog())\
{\
MTAPISTR str; \
Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(L"{} {}", __FUNCTIONW__, obj->Print(str)); \
}\
}\

namespace follow {

	void FollowPlugin::OnOrderAdd(const IMTOrder* order)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::OrderField orderField;
		shared::Business::MakeOrderField(order, orderField);
		Global::SharedGet()->m_OnOrderFieldQ.push(order->Order(), orderField);
		shared::serverplugin::OnOrderField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnOrderUpdate(const IMTOrder* order)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::OrderField orderField;
		shared::Business::MakeOrderField(order, orderField);
		Global::SharedGet()->m_OnOrderFieldQ.push(order->Order(), orderField);
		shared::serverplugin::OnOrderField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnOrderDelete(const IMTOrder* order)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		Global::SharedGet()->m_OnOrderFieldQ.pop(order->Order(), [](const auto&, auto&) {});
		shared::serverplugin::OnOrderField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnDealAdd(const IMTDeal* deal)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		if (!deal) return;
		shared::DealField dealSignal;
		shared::Business::MakeDealField(deal, dealSignal);
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnDealUpdate(const IMTDeal* deal)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		if (!deal) return;
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnDealDelete(const IMTDeal* deal)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		if (!deal) return;
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnDealPerform(const IMTDeal* deal, IMTAccount* account, IMTPosition* position)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		MT_MSG_PRINT(position);
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_PERFORM);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		shared::Business::MakeAccountField(account, MsgPak.Account);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif

#if ENABLE_FOLLOW_PROCESS_TYPE_ONDEALPERFORM
		if (!GetGlobal()->LicenseResult())
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->error(SKAPIRES_DESC(SKAPIRES::LICENSE_RET_ERROR_VERIFY_FAILED));
			return;
		}

		if (deal && position)
		{
			FollowObjGet()->m_OrdIDReqFieldQ.pop(
				deal->Order(), 
				[&](const auto&,auto& reqField) 
				{
					shared::serverplugin::OnTradeSignalField TradeSignal;
					shared::Business::MakeDealField(deal, TradeSignal.SignalDeal);
					shared::Business::MakePositionField(position, TradeSignal.SignalPosition);
					TradeSignal.SignalRequest = reqField;
					FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);
				});
		}
#endif
	}

	void FollowPlugin::OnTradeRequestAdd(const IMTRequest* request, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		shared::Business::MakeConGroupField(group, MsgPak.Group);
		shared::Business::MakeConSymbolField(symbol, MsgPak.Symbol);
		shared::Business::MakePositionField(position, MsgPak.Position);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
#if ENABLE_FOLLOW_PROCESS_TYPE_ONDEALPERFORM
		if (order && request)
		{
			shared::RequestField reqField;
			shared::Business::MakeRequestField(request, reqField);
			FollowObjGet()->m_OrdIDReqFieldQ.push(order->Order(), reqField);
		}
#endif
	}

	void FollowPlugin::OnTradeRequestUpdate(const IMTRequest* request)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnTradeRequestDelete(const IMTRequest* request)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnTradeRequestRefuse(const IMTRequest* request)
	{
		if (!request) return;
		shared::serverplugin::OnTradeSignalField TradeSignal;
		shared::Business::MakeRequestField(request, TradeSignal.SignalRequest);
		FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_REFUSE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnTradeRequestProcess(const IMTRequest* request, const IMTConfirm* confirm, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order, const IMTDeal* deal) {

		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(request);
		MT_MSG_PRINT(position);
		MT_MSG_PRINT(order);
		MT_MSG_PRINT(deal);
		MT_MSG_PRINT(confirm);
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_PROCESS);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		shared::Business::MakeConfirmField(confirm, MsgPak.Confirm);
		shared::Business::MakePositionField(position, MsgPak.Position);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		shared::Business::MakeConGroupField(group, MsgPak.Group);
		shared::Business::MakeConSymbolField(symbol, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif

#if !ENABLE_FOLLOW_PROCESS_TYPE_ONDEALPERFORM
		if (!GetGlobal()->LicenseResult())
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->error(SKAPIRES_DESC(SKAPIRES::LICENSE_RET_ERROR_VERIFY_FAILED));
			return;
		}

		if (deal && position && request)
		{
			shared::serverplugin::OnTradeSignalField TradeSignal;
			shared::Business::MakeDealField(deal, TradeSignal.SignalDeal);
			shared::Business::MakePositionField(position, TradeSignal.SignalPosition);
			shared::Business::MakeRequestField(request, TradeSignal.SignalRequest);
			FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);
		}
#endif
	}

	void FollowPlugin::OnPositionAdd(const IMTPosition* position)
	{
		if (!position) return;
		MT_MSG_PRINT(position);
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);
		Global::SharedGet()->m_OnPositionFieldQ.push(position->Position(), positionField);
		auto newPosistion = ServerApi()->PositionCreate();
		newPosistion->Assign(position);
		Global::SharedGet()->m_OnPositionPtrQ.push(newPosistion->Position(), newPosistion);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnPositionUpdate(const IMTPosition* position)
	{
		if (!position) return;
		MT_MSG_PRINT(position);
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
		Global::SharedGet()->m_OnPositionPtrQ.search(
			position->Position(),
			[&](const auto&, auto& positionObj)
			{
				positionObj->Assign(position);
			});
		Global::SharedGet()->m_OnPositionFieldQ.search(
			position->Position(),
			[&](const auto&, auto& positionFiled)
			{
				//!@ 为了判断更新内容 发送更新之前的信号
				FollowObjGet()->m_PositionFollowSignalQ.push(positionFiled);
				positionFiled = positionField;
			});
		FollowObjGet()->m_FollowProcessPosIDRouteQ.pop(
			position->Position(),
			[&](const auto&, auto& routePrev)
			{
				{//!@ 跟单日志 -- 完成(交易跟单完成)
					routePrev.FollowLog.FollowObj.Position = position->Position();
					routePrev.FollowLog.FollowObj.Action = position->Action();
					routePrev.FollowLog.FollowObj.Price = position->PriceCurrent();
					routePrev.FollowLog.FollowObj.PriceSL = position->PriceSL();
					routePrev.FollowLog.FollowObj.PriceTP = position->PriceTP();
					routePrev.FollowLog.FollowObj.Volume = position->Volume();
					sk::SafeCopyW(routePrev.FollowLog.FollowObj.Symbol, position->Symbol(), _countof(routePrev.FollowLog.FollowObj.Symbol));
					routePrev.FollowLog.ProcessResult = shared::FollowLogField::EnProcessResult::EN_PROCESS_RESULT_SUCCESS;
					routePrev.FollowLog.CompletionFlag = shared::FollowLogField::EnCompletionFlag::EN_COMPLETION_STATUS_FINISHED;
					Global::SharedGet()->m_FollowLogQ.push(routePrev.FollowLog);
				}

				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info(
						"持仓更新完成[login({}->{}),position({}->{}),symbol({}-{}),SL({}->{}),TP({}->{}),校正({})]",
						routePrev.PositionFollow.Login,
						routePrev.PositionSignal.Login,
						routePrev.PositionFollow.Position,
						routePrev.PositionSignal.Position,
						sk::StringConvert::WStringToMBytes(routePrev.PositionFollow.Symbol),
						sk::StringConvert::WStringToMBytes(routePrev.PositionSignal.Symbol),
						routePrev.PositionFollow.PriceSL,
						routePrev.PositionSignal.PriceSL,
						routePrev.PositionFollow.PriceTP,
						routePrev.PositionSignal.PriceTP,
						routePrev.Follow.PointSLTP);
				}
			});
	}

	void FollowPlugin::OnPositionDelete(const IMTPosition* position)
	{
		if (!position) return;
		MT_MSG_PRINT(position);
		Global::SharedGet()->m_OnPositionPtrQ.pop(position->Position(), [](const auto&, auto& obj) {obj->Release(); });
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
		Global::SharedGet()->m_OnPositionFieldQ.pop(position->Position(), [&](const auto&, auto&) {});
	}

	void FollowPlugin::OnUserAdd(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		Global::SharedGet()->m_OnUserFieldQ.push(user->Login(), User);
		Global::SharedGet()->m_LoginGroupQ.push(user->Login(), user->Group());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnUserUpdate(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		Global::SharedGet()->m_OnUserFieldQ.push(user->Login(), User);
		Global::SharedGet()->m_LoginGroupQ.push(user->Login(), user->Group());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}
	void FollowPlugin::OnUserDelete(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		Global::SharedGet()->m_OnUserFieldQ.pop(user->Login(), [](const auto&, auto&) {});
		Global::SharedGet()->m_LoginGroupQ.pop(user->Login(), [](const auto&, auto&) {});
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnGroupAdd(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		Global::SharedGet()->m_OnConGroupFieldQ.push(config->Group(), Group);
		//! 组配置添加
		auto nSymbol = config->SymbolTotal();
		if (nSymbol > 0)
		{
			sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
			for (UINT i = 0; i < nSymbol; ++i)
			{
				auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
				config->SymbolNext(i, pGroupSymbol);
				Global::SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
					config->Group(),
					[&](auto& cons)
					{
						shared::ConGroupSymbolField field;
						shared::Business::MakeConGroupSymbol(pGroupSymbol, field);
						cons.push(pGroupSymbol->Path(), field);
					});
				if (pGroupSymbol) pGroupSymbol->Release();
			}
		}
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnGroupField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeConGroupField(config, MsgPak.Group);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnGroupUpdate(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		Global::SharedGet()->m_OnConGroupFieldQ.push(config->Group(), Group);
		//! 组配置更新
		auto nSymbol = config->SymbolTotal();
		if (nSymbol > 0)
		{
			sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
			for (UINT i = 0; i < nSymbol; ++i)
			{
				auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
				config->SymbolNext(i, pGroupSymbol);
				Global::SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
					config->Group(),
					[&](auto& cons)
					{
						shared::ConGroupSymbolField field;
						shared::Business::MakeConGroupSymbol(pGroupSymbol, field);
						cons.push(pGroupSymbol->Path(), field);
					});
				if (pGroupSymbol) pGroupSymbol->Release();
			}
		}
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnGroupField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeConGroupField(config, MsgPak.Group);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnGroupDelete(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		Global::SharedGet()->m_OnConGroupFieldQ.pop(config->Group(), [](const auto&, auto&) {});
		//! 组配置删除
		Global::SharedGet()->m_ConGroupGroupSymbolQ.pop(config->Group(), [](const auto&, auto&) {});
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnGroupField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeConGroupField(config, MsgPak.Group);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnSymbolAdd(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		Global::SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		Global::SharedGet()->m_SymbolPathQ.push(config->Symbol(), config->Path());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnSymbolUpdate(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		Global::SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		Global::SharedGet()->m_SymbolPathQ.push(config->Symbol(), config->Path());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnSymbolDelete(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		Global::SharedGet()->m_OnConSymbolFieldQ.pop(config->Symbol(), [](const auto&, auto&) {});
		Global::SharedGet()->m_SymbolPathQ.pop(config->Symbol(), [](const auto&, auto&) {});

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void FollowPlugin::OnTimeUpdate(const IMTConTime* config)
	{
		if (!config) return;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		if (config)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
				"系统时间更新: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(config->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		GetGlobal()->License(ServerApi()->TimeCurrent());
	}

}///namespace follow