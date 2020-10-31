#include "stdafx.h"
#include "Dealer.h"

namespace dealer {

	void Dealer::OnDealPerform(const IMTDeal* deal, IMTAccount* account, IMTPosition* position)
	{
		if (!deal)
			return;
		//auto log = sk::Log::Format("ExternalID({}),{}\0\0", sk::StringConvert::WStringToMBytes(deal->ExternalID()), __FUNCTION__);
		//ExchangeGet()->NetSystemLogpakSend(log);
	}

	void Dealer::OnPositionAdd(const IMTPosition* position)
	{
		if (!position)
			return;
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);
		SharedGet()->m_MTPositionQ.push(positionField.Position, positionField);
	}

	void Dealer::OnPositionUpdate(const IMTPosition* position)
	{
		if (!position)
			return;
		shared::PositionField positionField;
		shared::Business::MakePositionField(position, positionField);
		SharedGet()->m_MTPositionQ.push(positionField.Position, positionField);
	}

	void Dealer::OnPositionDelete(const IMTPosition* position)
	{
		if (!position)
			return;
		SharedGet()->m_MTPositionQ.pop(position->Position(), [&](const auto&, auto&) {});
	}

	MTAPIRES Dealer::HookTradeRequestRoute(IMTRequest* request, IMTConfirm* confirm, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order)
	{
		return MT_RET_OK;
	}

	MTAPIRES Dealer::HookTradeRequestRuleFilter(IMTRequest* request, IMTConRoute* rule, const IMTConGroup* group)
	{
		return MT_RET_OK_NONE;
	}

	MTAPIRES Dealer::HookTradeRequestRuleApply(IMTRequest* request, IMTConRoute* rule, const IMTConGroup* group)
	{
		return MT_RET_OK_NONE;
	}
	void Dealer::OnTradeRequestRefuse(const IMTRequest* request)
	{

	}
	void Dealer::OnTradeRequestProcess(const IMTRequest* request, const IMTConfirm* confirm, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order, const IMTDeal* deal)
	{
#if 0
		if (SetupGet()->BridgeSystemSetup().DevelopDebugSwitch == shared::BridgeSystemSetup::EnDevelopDebugSwitch::EN_DEVELOP_DEBUG_MODE_DISABLE)
			return;
		sk::packet srvLog = \
			sk::Log::Format(
				"({})(reqID({}),reqVol({}),reqRetcode({}))\0",
				__FUNCTION__,
				request->ID(),
				request->Volume(),
				request->ResultRetcode()
			);
		ExchangeGet()->NetSystemLogpakSend(srvLog);
#endif
}




	void Dealer::OnUserAdd(const IMTUser* user)
	{
		shared::UserField userField;
		shared::Business::MakeUserField(user, userField);
		SharedGet()->m_MTUserQ.push(userField.Login, userField);
		if (ExchangeGet())
		{
			ExchangeGet()->NetMTUsersSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, sk::packet((char*)&userField, shared::LENUSERFIELD));
		}
	}

	void Dealer::OnUserUpdate(const IMTUser* user)
	{
		shared::UserField userField;
		shared::Business::MakeUserField(user, userField);
		SharedGet()->m_MTUserQ.pushpush(
			userField.Login, 
			[&](auto& userObj) 
			{
				if (sk::SafeCmpW(userObj.Group, user->Group(), _countof(userObj.Group)))
				{
					if (ExchangeGet())
					{
						ExchangeGet()->NetMTUsersSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, sk::packet((char*)&userField, shared::LENUSERFIELD));
					}
				}
			});
	}

	void Dealer::OnUserDelete(const IMTUser* user)
	{
		SharedGet()->m_MTUserQ.pop(user->Login(), [](const auto&, auto&) {});
		if (ExchangeGet())
		{
			shared::UserField userField;
			shared::Business::MakeUserField(user, userField);
			ExchangeGet()->NetMTUsersSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE, sk::packet((char*)&userField, shared::LENUSERFIELD));
		}
	}

	void Dealer::OnSymbolAdd(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		SharedGet()->m_MTSymbolQ.push(config->Symbol(), Symbol);
		if (ExchangeGet())
		{
			ExchangeGet()->NetProductsSend();
			ExchangeGet()->NetMTSymbolsSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, sk::packet((char*)&Symbol, shared::LENCONSYMBOLFIELD));
		}
	}

	void Dealer::OnSymbolUpdate(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.push(config->Symbol(), Symbol);
		SharedGet()->m_MTSymbolQ.push(config->Symbol(), Symbol);
		if (ExchangeGet())
		{
			ExchangeGet()->NetProductsSend();
			ExchangeGet()->NetMTSymbolsSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, sk::packet((char*)&Symbol, shared::LENCONSYMBOLFIELD));
		}
	}

	void Dealer::OnSymbolDelete(const IMTConSymbol* config)
	{
		if (!config) return;
		shared::ConSymbolField Symbol;
		shared::Business::MakeConSymbolField(config, Symbol);
		SharedGet()->m_OnConSymbolFieldQ.pop(config->Symbol(), [](const auto&, auto&) {});
		SharedGet()->m_MTSymbolQ.pop(config->Symbol(), [&](const auto&, auto&) {});
		if (ExchangeGet())
		{
			ExchangeGet()->NetProductsSend();
			ExchangeGet()->NetMTSymbolsSend(sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE, sk::packet((char*)&Symbol, shared::LENCONSYMBOLFIELD));
		}
	}

	void Dealer::OnTimeUpdate(const IMTConTime* config)
	{
		if (!config) return;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
			"系统时间更新: 时区[{}],时差[{}],时间[{}]",
			shared::Business::MTGMT(config->TimeZone()),
			m_ServerTimeDiff.load(),
			sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));

		License(ServerApi()->TimeCurrent());
	}


}///namespace gateway