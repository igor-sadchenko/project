#include "stdafx.h"
#include "Follow.h"

#define ENABLE_ONMSG_SYNC_SEND 0

#define MT_MSG_PRINT(obj)\
if(obj)\
{\
if (SetupGet()->EnableLog())\
{\
MTAPISTR str; \
Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(L"{} {}", __FUNCTIONW__, obj->Print(str)); \
}\
}\

namespace follow {

	void Follow::OnOrderAdd(const IMTOrder* order)
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

	void Follow::OnOrderUpdate(const IMTOrder* order)
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

	void Follow::OnOrderDelete(const IMTOrder* order)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		Global::SharedGet()->m_OnOrderFieldQ.pop(order->Order(), [](const auto&, auto&) {});
		shared::serverplugin::OnOrderField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeOrderField(order, MsgPak.Order);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnDealAdd(const IMTDeal* deal)
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

	void Follow::OnDealUpdate(const IMTDeal* deal)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		if (!deal) return;
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnDealDelete(const IMTDeal* deal)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		MT_MSG_PRINT(deal);
		if (!deal) return;
		shared::serverplugin::OnDealField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeDealField(deal, MsgPak.Deal);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnDealPerform(const IMTDeal* deal, IMTAccount* account, IMTPosition* position)
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
			Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(SKAPIRES_DESC(SKAPIRES::LICENSE_RET_ERROR_VERIFY_FAILED));
			return;
		}break;
		}///switch

		if (deal && position)
		{
			Follow::SharedGet()->m_PositionDealQ.push(deal->PositionID(), deal->Deal());

			FollowObjGet()->m_OrdIDReqFieldQ.pop(
				deal->Order(),
				[&](const auto&, auto& reqField)
				{
					shared::serverplugin::OnTradeSignalField TradeSignal;
					shared::Business::MakeDealField(deal, TradeSignal.SignalDeal);
					shared::Business::MakePositionField(position, TradeSignal.SignalPosition);
					TradeSignal.SignalRequest = reqField;
					TradeSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW;
					FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);
				});
		}
#endif
	}

	void Follow::OnTradeRequestAdd(const IMTRequest* request, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order)
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

	void Follow::OnTradeRequestUpdate(const IMTRequest* request)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnTradeRequestDelete(const IMTRequest* request)
	{
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnTradeRequestRefuse(const IMTRequest* request)
	{
		if (!request) return;
		shared::serverplugin::OnTradeSignalField TradeSignal;
		shared::Business::MakeRequestField(request, TradeSignal.SignalRequest);
		TradeSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW;
		FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnTradeRequestField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_REFUSE);
		shared::Business::MakeRequestField(request, MsgPak.Request);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnTradeRequestProcess(const IMTRequest* request, const IMTConfirm* confirm, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order, const IMTDeal* deal) {

		//Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
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
#if 0
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
			Follow::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(SKAPIRES_DESC(SKAPIRES::LICENSE_RET_ERROR_VERIFY_FAILED));
			return;
		}break;
		}///switch

		if (deal && position && request)
		{
			shared::serverplugin::OnTradeSignalField TradeSignal;
			shared::Business::MakeDealField(deal, TradeSignal.SignalDeal);
			shared::Business::MakePositionField(position, TradeSignal.SignalPosition);
			shared::Business::MakeRequestField(request, TradeSignal.SignalRequest);
			TradeSignal.SignalType = (UINT)RouteObj::EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW;
			FollowObjGet()->m_TradeFollowSignalQ.push(TradeSignal);
		}
#endif
	}

	void Follow::OnPositionAdd(const IMTPosition* position)
	{
		if (!position) return;
		//MT_MSG_PRINT(position);
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);
		SharedGet()->m_OnPositionFieldQ.push(position->Position(), positionField);
		auto newPosistion = ServerApi()->PositionCreate();
		newPosistion->Assign(position);
		SharedGet()->m_OnPositionPtrQ.push(newPosistion->Position(), newPosistion);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnPositionUpdate(const IMTPosition* position)
	{
		if (!position) return;
		//MT_MSG_PRINT(position);
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
		SharedGet()->m_OnPositionPtrQ.search(
			position->Position(),
			[&](const auto&, auto& positionObj)
			{
				positionObj->Assign(position);
			});

		SharedGet()->m_OnPositionFieldQ.search(
			position->Position(),
			[&](const auto&, auto& positionFiled)
			{
				positionFiled = positionField;
			});


		FollowObjGet()->m_PositionUpdateRouteQ.search(
			position->Position(), 
			[&](const auto&,shared::serverplugin::OnTradeSignalField& signalField) 
			{
				signalField.SignalPosition = positionField;
				signalField.InitRequest.Position = position->Position();
				signalField.InitRequest.Type = position->Action();
				signalField.InitRequest.PriceSL = position->PriceSL();
				signalField.InitRequest.PriceTP = position->PriceTP();
				FollowObjGet()->m_TradeFollowSignalQ.push(signalField);
			});
#if 0
		SharedGet()->m_OnPositionFieldQ.search(
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
					//!@"时间","任务类型","帐户","数量","价格","持仓","品种","多空","开平","SL","TP","信号源","信号持仓","信号SL","信号TP","运行时间","返回代码","原因"
					auto record = sk::Log::Format(
						"{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
						sk::time::Time::TimestampToTimeStr(sk::Helper::TickCountGet<std::chrono::milliseconds>(), true),
						"编辑定单",
						routePrev.PositionFollow.Login,
						routePrev.PositionFollow.Volume,
						sk::Helper::FloatToStrA(routePrev.PositionFollow.PriceOpen, 5),
						routePrev.PositionFollow.Position,
						sk::StringConvert::WStringToMBytes(routePrev.PositionFollow.Symbol),
						"-",
						"-",
						sk::Helper::FloatToStrA(routePrev.PositionFollow.PriceSL, 5),
						sk::Helper::FloatToStrA(routePrev.PositionFollow.PriceTP, 5),
						routePrev.PositionSignal.Login,
						routePrev.PositionSignal.Position,
						sk::Helper::FloatToStrA(routePrev.PositionSignal.PriceSL, 5),
						sk::Helper::FloatToStrA(routePrev.PositionSignal.PriceTP, 5),
						"-",
						"-",
						"-"
					);
					Follow::SharedGet()->m_FollowLogQ.push(record);
				}

				{
					Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info(
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
#endif
	}

	void Follow::OnPositionDelete(const IMTPosition* position)
	{
		if (!position) return;
		//MT_MSG_PRINT(position);
		SharedGet()->m_OnPositionPtrQ.pop(position->Position(), [](const auto&, auto& obj) {obj->Release(); });
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnPositionField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakePositionField(position, MsgPak.Position);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
		SharedGet()->m_OnPositionFieldQ.pop(position->Position(), [&](const auto&, auto&) {});

		Follow::SharedGet()->m_PositionRelationQ.iterate_clear(
			[&](const auto& signalPositionID, auto& followPositionIDs, auto& itbreak, auto& itclear)
			{
				if (followPositionIDs.pop(position->Position(), [](const auto&,auto&) {}))
				{
					itclear = followPositionIDs.empty();
					itbreak = true;
				}
			});

		Follow::SharedGet()->m_PositionDealQ.pop(position->Position(), [](const auto&,auto&) {});
	}

	void Follow::OnUserAdd(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		SharedGet()->m_OnUserFieldQ.push(user->Login(), User);
		SharedGet()->m_LoginGroupQ.push(user->Login(), user->Group());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnUserUpdate(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		SharedGet()->m_OnUserFieldQ.push(user->Login(), User);
		SharedGet()->m_LoginGroupQ.push(user->Login(), user->Group());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}
	void Follow::OnUserDelete(const IMTUser* user)
	{
		if (!user) return;
		shared::UserField User;
		shared::Business::MakeUserField(user, User);
		SharedGet()->m_OnUserFieldQ.pop(user->Login(), [](const auto&, auto&) {});
		SharedGet()->m_LoginGroupQ.pop(user->Login(), [](const auto&, auto&) {});
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnUserField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeUserField(user, MsgPak.User);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnGroupAdd(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		SharedGet()->m_OnConGroupFieldQ.push(config->Group(), Group);
		//! 组配置添加
		auto nSymbol = config->SymbolTotal();
		if (nSymbol > 0)
		{
			sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
			for (UINT i = 0; i < nSymbol; ++i)
			{
				auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
				config->SymbolNext(i, pGroupSymbol);
				SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
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

	void Follow::OnGroupUpdate(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		SharedGet()->m_OnConGroupFieldQ.push(config->Group(), Group);
		//! 组配置更新
		auto nSymbol = config->SymbolTotal();
		if (nSymbol > 0)
		{
			sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
			for (UINT i = 0; i < nSymbol; ++i)
			{
				auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
				config->SymbolNext(i, pGroupSymbol);
				SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
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

	void Follow::OnGroupDelete(const IMTConGroup* config)
	{
		if (!config) return;
		shared::ConGroupField Group;
		shared::Business::MakeConGroupField(config, Group);
		SharedGet()->m_OnConGroupFieldQ.pop(config->Group(), [](const auto&, auto&) {});
		//! 组配置删除
		SharedGet()->m_ConGroupGroupSymbolQ.pop(config->Group(), [](const auto&, auto&) {});
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnGroupField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeConGroupField(config, MsgPak.Group);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnSymbolAdd(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		SharedGet()->m_SymbolPathQ.push(config->Symbol(), config->Path());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_ADD);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnSymbolUpdate(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		SharedGet()->m_SymbolPathQ.push(config->Symbol(), config->Path());
#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_UDPATE);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnSymbolDelete(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.pop(config->Symbol(), [](const auto&, auto&) {});
		SharedGet()->m_SymbolPathQ.pop(config->Symbol(), [](const auto&, auto&) {});

#if (ENABLE_ONMSG_SYNC_SEND)
		shared::serverplugin::OnSymbolField MsgPak(shared::serverplugin::EnMsgTypeSub::EN_MSG_TYPE_SUB_DELETE);
		shared::Business::MakeConSymbolField(config, MsgPak.Symbol);
		Global::SharedGet()->m_PacketFormViewQ.push(sk::packet((char*)&MsgPak, sizeof(MsgPak)));
#endif
	}

	void Follow::OnTimeUpdate(const IMTConTime* config)
	{
		if (!config) return;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		if (config)
		{
			Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
				"系统时间更新: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(config->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		License(ServerApi()->TimeCurrent());
	}

}///namespace follow