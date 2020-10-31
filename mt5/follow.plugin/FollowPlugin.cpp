#include "stdafx.h"
#include "FollowPlugin.h"

#define MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK 0

namespace follow {
	FollowPlugin::FollowPlugin() {
		m_mutex = std::make_shared<std::mutex>();
		memset(&m_ServerInfo, 0x00, sizeof(m_ServerInfo));
	}

	FollowPlugin::~FollowPlugin() {
		/*Stop();*/
	}

	void FollowPlugin::Release() {
		delete this;
	}

	MTAPIRES FollowPlugin::Start(IMTServerAPI* pServerApi) {
		MTAPIRES retcode;
		if (!pServerApi) return(MT_RET_ERR_PARAMS);

		m_pServerApi = pServerApi;

		MTAPIRES result = MT_RET_OK;
		IMTConPlugin* pCurrPlugin = ServerApi()->PluginCreate();
		if (!pCurrPlugin) { return MT_RET_ERR_MEM; }
		if (result = ServerApi()->PluginCurrent(pCurrPlugin) != MT_RET_OK)
		{
			ServerApi()->LoggerOut(result, L"%s", L"ServerApi PluginCurrent() error.");
			pCurrPlugin->Release();
			return result;
		}
		IMTConParam* pPluginParam = ServerApi()->PluginParamCreate();
		if (!pPluginParam) { return MT_RET_ERR_MEM; }
		for (UINT i = 0; i < pCurrPlugin->ParameterTotal(); ++i)
		{
			pPluginParam->Clear();
			if (result = pCurrPlugin->ParameterNext(i, pPluginParam) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(result, L"%s", L"ServerApi ParameterNext() error.");
				pPluginParam->Release();
				pCurrPlugin->Release();
				return result;
			}
			if (!sk::SafeCmpW(L"PathObj", pPluginParam->Name(), _countof(MTPluginParam::name)) && pPluginParam->Type() == IMTConParam::ParamType::TYPE_STRING)
			{
				m_PathObj = sk::StringConvert::WStringToMBytes(pPluginParam->ValueString());
			}
		}
		pPluginParam->Release();
		pCurrPlugin->Release();
		if (m_PathObj.empty())
		{
			return MT_RET_ERR_PARAMS;
		}

		{
			static_global_Global = new Global();
			Global::SystemPath(m_PathObj);
			GetGlobal()->Init();
			Global::GetCore()->Open();
		}

		if ((retcode = ServerApi()->About(m_ServerInfo)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Server info failed {}", retcode);
		}
		if ((retcode = ServerApi()->DealSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Deal subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->TradeSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Trade subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->OrderSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Order subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->PositionSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Position subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->UserSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("User subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->GroupSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Group subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->TimeSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Time subscribe failed {}", retcode);
		}
		if ((retcode = ServerApi()->SymbolSubscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("Symbol subscribe failed {}", retcode);
		}

		m_pFollowObj = new FollowObj();
		if (m_pFollowObj)
		{
			m_pFollowObj->Open();
		}

		{
			Synchronize();
		}
		
		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn(
			"\n{}\n{}\n{}\n{}\n",
			GetGlobal()->LicenseInfo().ProductTypeStrA(),
			GetGlobal()->LicenseInfo().ProductVersionTypeStrA(),
			GetGlobal()->LicenseInfo().ProductLicenseToStrA(),
			GetGlobal()->LicenseInfo().ProductServiceToStrA());

		return MT_RET_OK;
	}

	MTAPIRES FollowPlugin::Stop(void) {

		Global::SharedGet()->m_OnPositionPtrQ.iterate([&](const auto&, auto& obj) {obj->Release(); });

		if (m_pFollowObj)
		{
			m_pFollowObj->Close();
			SK_DELETE_PTR(m_pFollowObj);
		}

		if (ServerApi()) {
			ServerApi()->DealUnsubscribe(this);
			ServerApi()->TradeUnsubscribe(this);
			ServerApi()->PositionUnsubscribe(this);
			ServerApi()->OrderUnsubscribe(this);
			ServerApi()->TimeUnsubscribe(this);
			ServerApi()->UserUnsubscribe(this);
			ServerApi()->GroupUnsubscribe(this);
			ServerApi()->SymbolUnsubscribe(this);
			m_pServerApi = nullptr;
		}

		{
			Global::GetCore()->Close();
			GetGlobal()->UnInit();
			SK_DELETE_PTR(static_global_Global);
		}

		return MT_RET_OK;
	}


	int FollowPlugin::SyncUsers()
	{
		Global::SharedGet()->m_OnUserFieldQ.clear();
		Global::SharedGet()->m_LoginGroupQ.clear();

		Global::SharedGet()->m_OnConGroupFieldQ.iterate(
			[&](const auto& group, auto& group_obj)
			{
				UINT64* logins = nullptr;
				UINT	logins_total = 0;
				if (MT_RET_OK == ServerApi()->UserLogins(group.c_str(), logins, logins_total))
				{
					for (decltype(logins_total) i = 0; i < logins_total; ++i)
					{
						auto pUser = ServerApi()->UserCreate();
						if (MT_RET_OK == ServerApi()->UserGet(logins[i], pUser))
						{
							shared::UserField User;
							shared::Business::MakeUserField(pUser, User);
							Global::SharedGet()->m_OnUserFieldQ.push(logins[i], User);
							Global::SharedGet()->m_LoginGroupQ.push(logins[i], User.Group);
						}
						pUser->Release();
					}
				}
			});
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Users total({})", Global::SharedGet()->m_OnUserFieldQ.size());
		return 0;
	}

	int FollowPlugin::SyncGroups()
	{
		Global::SharedGet()->m_OnConGroupFieldQ.clear();
		Global::SharedGet()->m_ConGroupGroupSymbolQ.clear();

		UINT nTotalGroup = ServerApi()->GroupTotal();
		for (UINT i = 0; i < nTotalGroup; ++i)
		{
			auto pGroup = ServerApi()->GroupCreate();
			if (MT_RET_OK == ServerApi()->GroupNext(i, pGroup))
			{
				shared::ConGroupField GroupField;
				shared::Business::MakeConGroupField(pGroup, GroupField);
				Global::SharedGet()->m_OnConGroupFieldQ.push(pGroup->Group(), GroupField);
				auto nSymbol = pGroup->SymbolTotal();
				if (nSymbol > 0)
				{
					sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
					for (UINT i = 0; i < nSymbol; ++i)
					{
						auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
						pGroup->SymbolNext(i, pGroupSymbol);
						Global::SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
							pGroup->Group(),
							[&](auto& cons)
							{
								shared::ConGroupSymbolField field;
								shared::Business::MakeConGroupSymbol(pGroupSymbol, field);
								cons.push(pGroupSymbol->Path(), field);
							});
						if (pGroupSymbol) pGroupSymbol->Release();
					}
				}
			}
			pGroup->Release();
		}

		{
			sk::stringw groupLog(LOGF(L"Groups({})\n",
				Global::SharedGet()->m_OnConGroupFieldQ.size()));
			Global::SharedGet()->m_ConGroupGroupSymbolQ.iterate(
				[&](const auto& group, auto& groupSymbols)
				{
					groupLog.append(L"GroupName: ").append(group).append(L"\n");
					groupSymbols.iterate(
						[&](const auto& path, auto& obj)
						{
							groupLog.append(L"GroupSymbolCon: ").append(path).append(L"\n");
						});
				});
			Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(L"{}", groupLog);
		}
		return 0;
	}

	int FollowPlugin::SyncSymbols()
	{
		Global::SharedGet()->m_SymbolPathQ.clear();
		Global::SharedGet()->m_OnConSymbolFieldQ.clear();

		UINT nTotalConSymbol = ServerApi()->SymbolTotal();
		for (UINT i = 0; i < nTotalConSymbol; ++i)
		{
			auto pSymbol = ServerApi()->SymbolCreate();
			if (MT_RET_OK == ServerApi()->SymbolNext(i, pSymbol))
			{
				shared::ConSymbolField Symbol;
				shared::Business::MakeConSymbolField(pSymbol, Symbol);
				auto path = sk::stringw(pSymbol->Path());
				auto find = path.find(pSymbol->Symbol());
				if (find != sk::stringw::npos)
				{
					path[find] = 0;
					Global::SharedGet()->m_SymbolPathQ.push(pSymbol->Symbol(), path.c_str());
				}
				else
				{
					Global::SharedGet()->m_SymbolPathQ.push(pSymbol->Symbol(), path);
				}
				Global::SharedGet()->m_OnConSymbolFieldQ.push(pSymbol->Symbol(), Symbol);
			}
			pSymbol->Release();
		}
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Symbol total({})", Global::SharedGet()->m_OnConSymbolFieldQ.size());
		return 0;
	}

	int FollowPlugin::SyncServerTime()
	{
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		auto conTime = ServerApi()->TimeCreate();
		ServerApi()->TimeGet(conTime);
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info(
				"系统时间: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(conTime->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		conTime->Release();

		GetGlobal()->License(ServerApi()->TimeCurrent());

		return 0;
	}

	int FollowPlugin::SyncPositions()
	{
		Global::SharedGet()->m_OnPositionFieldQ.clear();

		Global::SharedGet()->m_OnUserFieldQ.iterate(
			[&](const auto& login, auto& user_obj) {

				INT64 dealRequestBeginTime = 0;
				auto pAyPosition = ServerApi()->PositionCreateArray();
				if (MT_RET_OK == ServerApi()->PositionGet(login, pAyPosition))
				{
					UINT UserPositionTotal = pAyPosition->Total();
					for (UINT i = 0; i < UserPositionTotal; ++i)
					{
						auto pPosition = pAyPosition->Next(i);
						dealRequestBeginTime = \
							dealRequestBeginTime == 0 ? pPosition->TimeCreate() : __min(dealRequestBeginTime, pPosition->TimeCreate());
						auto newPosition = ServerApi()->PositionCreate();
						if (newPosition)
						{
							newPosition->Assign(pPosition);
							Global::SharedGet()->m_OnPositionPtrQ.push(newPosition->Position(), newPosition);
						}
						shared::PositionField Position;
						shared::Business::MakePositionField(pPosition, Position);
						Global::SharedGet()->m_OnPositionFieldQ.push(pPosition->Position(), Position);
					}
				}

				{
					const int dealRequectBeginTimeGotoPrev = 60;
					/*Global::ObjLog()->info("订单请求起始时间({}),前移({})秒", sk::time::Time::TimestampToTimeStr(dealRequestBeginTime), dealRequectBeginTimeGotoPrev);*/
					//! 初始化持仓关系表
					dealRequestBeginTime -= dealRequectBeginTimeGotoPrev;
					IMTDealArray* deals = m_pServerApi->DealCreateArray();
					if (deals)
					{
						if (MT_RET_OK == m_pServerApi->DealGet(dealRequestBeginTime, m_pServerApi->TimeCurrent(), login, deals))
						{
							for (UINT i = 0; i < deals->Total(); ++i)
							{
								auto deal = deals->Next(i);
								if (deal)
								{
									shared::DealField dealField;
									shared::Business::MakeDealField(deal, dealField);

									Global::SharedGet()->m_PositionRelationQ.pushpush(
										dealField.ApiDataPosition,
										[&](auto& follows)
										{
											follows.push(dealField.PositionID);
										});
								}
							}
							/*Global::ObjLog()->info("与当前持仓有关的历史订单总数({})", deals->Total());*/
						}
						deals->Release();
					}
				}
				pAyPosition->Release();
			});

		std::vector<sk::uint64> clearSignals;
		Global::SharedGet()->m_PositionRelationQ.iterate(
			[&](const auto& signalPositionID, auto& followPositionFields)
			{
				if (!Global::SharedGet()->m_OnPositionFieldQ.search(signalPositionID))
				{
					clearSignals.emplace_back(signalPositionID);
				}
				else
				{
					std::vector<sk::uint64> clearFollows;
					followPositionFields.iterate(
						[&](auto& follow)
						{
							if (!Global::SharedGet()->m_OnPositionFieldQ.search(follow))
							{
								clearFollows.emplace_back(follow);
							}
						});
					for (const auto& node : clearFollows)
					{
						followPositionFields.pop(node);
					}
					if (followPositionFields.empty())
					{
						clearSignals.emplace_back(signalPositionID);
					}
				}
			});

		for (const auto& node : clearSignals)
		{
			Global::SharedGet()->m_PositionRelationQ.pop(node);
		}
		Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("业务持仓({}),MT持仓({}),信号持仓关系({})", Global::SharedGet()->m_OnPositionFieldQ.size(), Global::SharedGet()->m_OnPositionPtrQ.size(), Global::SharedGet()->m_PositionRelationQ.size());

		return 0;
	}

	int FollowPlugin::Synchronize()
	{
		SyncServerTime();
		SyncGroups();
		SyncUsers();
		SyncSymbols();
		SyncPositions();
		return 0;
	}

	MTAPIRES FollowPlugin::PositionGetByTicket(
		__in const UINT64& ticket,
		__out shared::PositionField& outPositionField)
	{
		MTAPIRES result = MT_RET_ERROR;
		memset(&outPositionField, 0x00, sizeof(outPositionField));
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			IMTPosition* position = ServerApi()->PositionCreate();
			if (position)
			{
				position->Clear();
				if ((result = ServerApi()->PositionGetByTicket(ticket, position)) == MT_RET_OK)
				{
					shared::Business::MakePositionField(position, outPositionField);
				}
				position->Release();
			}
		}
		return result;
	}

	MTAPIRES FollowPlugin::DealGet(
		__in const UINT64& inTicketID,
		__out shared::DealField& outDealField)
	{
		MTAPIRES result = MT_RET_ERROR;
		memset(&outDealField, 0x00, sizeof(outDealField));
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			IMTDeal* pDeal = ServerApi()->DealCreate();
			if (pDeal)
			{
				pDeal->Clear();
				if ((result = ServerApi()->DealGet(inTicketID, pDeal)) == MT_RET_OK)
				{
					shared::Business::MakeDealField(pDeal, outDealField);
				}
				pDeal->Release();
			}
		}
		return result;
	}

	MTAPIRES FollowPlugin::TradeRequest
	(
		__in shared::RequestField& reqField	
	)
	{
		MTAPIRES result = MT_RET_ERROR;
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			IMTRequest* pRequest = ServerApi()->TradeRequestCreate();
			if (pRequest)
			{
				shared::Business::RequestMake(reqField, pRequest);
				result = ServerApi()->TradeRequest(pRequest);
				reqField.ID = pRequest->ID();
				pRequest->Release();
			}
		}
		return result;
	}




	MTAPIRES FollowPlugin::DealApiDataAt
	(
		__in const UINT64& ticket,
		__in const std::function<MTAPIRES(IMTDeal*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			auto pDeal = ServerApi()->DealCreate();
			if (pDeal)
			{
				if ((result = ServerApi()->DealGet(ticket, pDeal)) == MT_RET_OK)
				{
					result = cbProcess(pDeal);
					if (MT_RET_OK == result)
					{
						result = ServerApi()->DealUpdate(pDeal);
					}
				}
				pDeal->Release();
			}
		}
		return result;
	}
	MTAPIRES FollowPlugin::PositionApiDataAt(
		__in const UINT64& ticket,
		__in const std::function<MTAPIRES(IMTPosition*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			auto pPosition = ServerApi()->PositionCreate();
			if (pPosition)
			{
				if ((result = ServerApi()->PositionGetByTicket(ticket, pPosition)) == MT_RET_OK)
				{
					result = cbProcess(pPosition);
					if (MT_RET_OK == result)
					{
						result = ServerApi()->PositionUpdate(pPosition);
					}
				}
				pPosition->Release();
			}
		}
		return result;
	}
	MTAPIRES FollowPlugin::PositionApiDataAt(
		__in const UINT64& login,
		__in const wchar_t* symbol,
		__in const std::function<MTAPIRES(IMTPosition*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
#if MT_FOLLOW_PLUGIN_ENABLE_MUTEX_LOCK>0
		std::lock_guard<std::mutex> lock(*m_mutex);
#endif
		if (ServerApi())
		{
			auto pPosition = ServerApi()->PositionCreate();
			if (pPosition)
			{
				if ((result = ServerApi()->PositionGet(login, symbol, pPosition)) == MT_RET_OK)
				{
					result = cbProcess(pPosition);
					if (MT_RET_OK == result)
					{
						result = ServerApi()->PositionUpdate(pPosition);
					}
				}
				pPosition->Release();
			}
		}
		return result;
	}
	UINT64 FollowPlugin::ApiDataGet
	(
		__in const IMTPosition* positionObj,
		__in const shared::EnMTApiParamType& paramType
	)
	{
		UINT64 result = 0;
		if (positionObj)
		{
			positionObj->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)paramType, result);
		}
		return result;
	}
	MTAPIRES FollowPlugin::ApiDataSet
	(
		__in IMTPosition* positionObj,
		__in const shared::EnMTApiParamType& paramType,
		__in const UINT64& value
	)
	{
		MTAPIRES result = MT_RET_ERROR;
		if (positionObj)
		{
			result = positionObj->ApiDataSet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)paramType, value);
		}
		return result;
	}

	UINT64 FollowPlugin::ApiDataGet
	(
		__in const IMTDeal* dealObj,
		__in const shared::EnMTApiParamType& paramType
	)
	{
		UINT64 result = 0;
		if (dealObj)
		{
			dealObj->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)paramType, result);
		}
		return result;
	}
	MTAPIRES FollowPlugin::ApiDataSet
	(
		__in IMTDeal* dealObj,
		__in const shared::EnMTApiParamType& paramType,
		__in const UINT64& value
	)
	{
		MTAPIRES result = MT_RET_ERROR;
		if (dealObj)
		{
			result = dealObj->ApiDataSet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)paramType, value);
		}
		return result;
	}
}///namespace follow



