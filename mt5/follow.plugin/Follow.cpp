#include "stdafx.h"
#include "Follow.h"

namespace follow {
	HINSTANCE global_hInstance = nullptr;
	Follow* static_global_Follow = nullptr;
	Follow* FollowGet() { return static_global_Follow; }

	Follow::Follow()
	{
		memset(&m_ServerInfo, 0x00, sizeof(m_ServerInfo));
	}

	Follow::~Follow()
	{
		/*Stop();*/
	}

	void Follow::Release() 
	{
		delete this;
	}

	Server* Follow::ServerGet()
	{
		return FollowGet()->m_pServer;
	}
	Setup* Follow::SetupGet()
	{
		return FollowGet()->m_pSetup;
	}
	Shared* Follow::SharedGet()
	{
		return FollowGet()->m_pShared;
	}
	shared::SharedApi* Follow::SharedApiGet()
	{
		return FollowGet()->m_pSKApi;
	}
	std::shared_ptr<spdlog::logger> Follow::Log(sk::EnLogType logType)
	{
		return FollowGet()->m_pLog->log(FollowGet()->m_LogKey, logType);
	}

	MTAPIRES Follow::Start(IMTServerAPI* pServerApi) 
	{
#if defined(_DEBUG)
		m_pSetup = new Setup(R"(C:\sk.project.setup\plugin.follow.obj\)");
		if (m_pSetup->Load())
		{
			SK_DELETE_PTR(m_pSetup);
			return MT_RET_ERROR;
		}
		if (m_pSetup->SystemSetupLoad())
		{
			SK_DELETE_PTR(m_pSetup);
			return MT_RET_ERROR;
		}
		m_pLog = new sk::Log(m_pSetup->ObjPath());
		m_LogKey = m_pLog->Alloc(sk::Helper::GetCurrentProcessNameA(global_hInstance), sk::EnLogType::EN_LOG_TYPE_ALL);
		m_pSKApi = new shared::SharedApi(m_pLog, shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK, m_pSetup->ApiPath().c_str());
		m_pServer = new Server();
		m_pServer->Open();
		return MT_RET_OK;
#endif



		if (m_PluginReady.load()) 
			return MT_RET_OK;
		if (!pServerApi) 
			return MT_RET_ERR_PARAMS;

		m_pServerApi = pServerApi;

		License();

		if (sk::LicenseInfo::EnProductVersionType(LicenseGet()) == sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_INVALID)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"ProductVersionType ivnalid(%u),Please contact the developer!", "-1");
			return MT_RET_ERR_PARAMS;
		}

		MTAPIRES result = MT_RET_OK;

		if ((result = ServerApi()->About(m_ServerInfo)) != MT_RET_OK) 
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"About() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->DealSubscribe(this)) != MT_RET_OK)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"DealSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->TradeSubscribe(this)) != MT_RET_OK) 
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"TradeSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->OrderSubscribe(this)) != MT_RET_OK) 
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"OrderSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->PositionSubscribe(this)) != MT_RET_OK)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"PositionSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->UserSubscribe(this)) != MT_RET_OK) 
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"UserSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->GroupSubscribe(this)) != MT_RET_OK) 
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"GroupSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->TimeSubscribe(this)) != MT_RET_OK)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"TimeSubscribe() failed(%u)", result);
			return result;
		}
		if ((result = ServerApi()->SymbolSubscribe(this)) != MT_RET_OK)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"SymbolSubscribe() failed(%u)", result);
			return result;
		}
		
		auto pCurrPlugin = ServerApi()->PluginCreate();
		result = ServerApi()->PluginCurrent(pCurrPlugin);
		if (result != MT_RET_OK)
		{
			ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"PluginCurrent() failed(%u)", result);
			pCurrPlugin->Release();
			return result;
		}
		auto PathObj = shared::Business::MTParamenterGet<IMTServerAPI, IMTConPlugin>(m_pServerApi, pCurrPlugin, L"PathObj");
		pCurrPlugin->Release();
		if (PathObj.empty())
		{
			return MT_RET_ERR_PARAMS;
		}
		auto ansiPathObj = sk::StringConvert::WStringToMBytes(PathObj);

		m_pShared = new Shared();
		m_pSetup = new Setup(ansiPathObj);
		if (m_pSetup->Load())
		{
			SK_DELETE_PTR(m_pSetup);
			return MT_RET_ERROR;
		}
		if (m_pSetup->SystemSetupLoad())
		{
			SK_DELETE_PTR(m_pSetup);
			return MT_RET_ERROR;
		}
		m_pLog = new sk::Log(m_pSetup->ObjPath());
		m_LogKey = m_pLog->Alloc(sk::Helper::GetCurrentProcessNameA(global_hInstance), sk::EnLogType::EN_LOG_TYPE_ALL);
		m_pSKApi = new shared::SharedApi(m_pLog, shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,m_pSetup->ApiPath().c_str());

		if (!m_pSKApi || !m_pSKApi->Verify())
		{
			SK_DELETE_PTR(m_pShared);
			SK_DELETE_PTR(m_pSetup);
			SK_DELETE_PTR(m_pSKApi);
			SK_DELETE_PTR(m_pLog);
			return MT_RET_ERR_MEM;
		}


		{
			Synchronize();
		}

		m_pFollowObj = new FollowObj();
		m_pFollowObj->Open();

		m_pServer = new Server();
		if (m_pServer->Open()) 
		{
			m_pFollowObj->Close();
			SK_DELETE_PTR(m_pFollowObj);
			SK_DELETE_PTR(m_pServer);
			return MT_RET_ERROR; 
		}

		Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn(
			"\n{}\n{}\n{}\n{}\n",
			m_LicenseInfo.ProductTypeStrA(),
			m_LicenseInfo.ProductVersionTypeStrA(),
			m_LicenseInfo.ProductLicenseToStrA(),
			m_LicenseInfo.ProductServiceToStrA());

		m_PluginReady.store(true);
		return MT_RET_OK;
	}

	int Follow::License(__int64 time /*= 0*/)
	{
		if (time <= 0) time = ::time(0);
		sk::MachineHardwareInformation MachineLocal;
		if (sk::Helper::MachineHardwareInformationGet(MachineLocal))
			return -1;

		auto licenseResult = sk::License::Verify(
			sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_SA,
			MachineLocal,
			time,
			sk::License::Load(sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_SA),
			m_LicenseInfo);

		if (SKAPIRES::LICENSE_RET_SUCCESS != licenseResult)
		{
			LicenseSet(sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_INVALID);

			if(FollowGet()&& FollowGet()->m_pLog)
				Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error(SKAPIRES_DESC(licenseResult));
		}
		else
		{
			LicenseSet(m_LicenseInfo.ProductVersionType);
		}
		return 0;
	}

	MTAPIRES Follow::Stop(void) 
	{
#if defined(_DEBUG)
		auto ProcessName = sk::Helper::GetCurrentProcessName();
		std::multimap<HMODULE, sk::string> view;
		sk::Helper::ViewProcessLoadDll(ProcessName, view);


		if (m_pServer) m_pServer->Close();
		SK_DELETE_PTR(m_pServer);
		SK_DELETE_PTR(m_pSKApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);

		view.clear();
		sk::Helper::ViewProcessLoadDll(ProcessName, view);


		
		sk::Helper::UnLoadDllAt(sk::Helper::GetCurrentProcessName(), sk::StringConvert::MBytesToWString(shared::api_network));

		view.clear();
		sk::Helper::ViewProcessLoadDll(ProcessName, view);

		sk::Helper::UnLoadDllAt(sk::Helper::GetCurrentProcessName(), sk::StringConvert::MBytesToWString(shared::api_network));

		view.clear();
		sk::Helper::ViewProcessLoadDll(ProcessName, view);
		return MT_RET_OK;
#endif
		if (m_pFollowObj)
		{
			m_pFollowObj->Close();
			SK_DELETE_PTR(m_pFollowObj);
		}

		if (m_pServer) m_pServer->Close();

		if (ServerApi())
		{
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

		SK_DELETE_PTR(m_pServer);
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pSKApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		//sk::Helper::UnLoadDllAt(sk::Helper::GetCurrentProcessName(), sk::StringConvert::MBytesToWString(shared::api_network));
		m_PluginReady.store(false);
		return MT_RET_OK;
	}


	int Follow::SyncUsers()
	{
		SharedGet()->m_OnUserFieldQ.clear();
		SharedGet()->m_LoginGroupQ.clear();

		SharedGet()->m_OnConGroupFieldQ.iterate(
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
							SharedGet()->m_OnUserFieldQ.push(logins[i], User);
							SharedGet()->m_LoginGroupQ.push(logins[i], User.Group);
						}
						pUser->Release();
					}
				}
			});
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Users total({})", SharedGet()->m_OnUserFieldQ.size());
		return 0;
	}

	int Follow::SyncGroups()
	{
		SharedGet()->m_OnConGroupFieldQ.clear();
		SharedGet()->m_ConGroupGroupSymbolQ.clear();

		UINT nTotalGroup = ServerApi()->GroupTotal();
		for (UINT i = 0; i < nTotalGroup; ++i)
		{
			auto pGroup = ServerApi()->GroupCreate();
			if (MT_RET_OK == ServerApi()->GroupNext(i, pGroup))
			{
				shared::ConGroupField GroupField;
				shared::Business::MakeConGroupField(pGroup, GroupField);
				SharedGet()->m_OnConGroupFieldQ.push(pGroup->Group(), GroupField);
				auto nSymbol = pGroup->SymbolTotal();
				if (nSymbol > 0)
				{
					sk::container::map<sk::stringw, IMTConGroupSymbol*> IMTConGroupSymbolPtrNoteQ;
					for (UINT i = 0; i < nSymbol; ++i)
					{
						auto pGroupSymbol = ServerApi()->GroupSymbolCreate();
						pGroup->SymbolNext(i, pGroupSymbol);
						SharedGet()->m_ConGroupGroupSymbolQ.pushpush(
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
				SharedGet()->m_OnConGroupFieldQ.size()));
			SharedGet()->m_ConGroupGroupSymbolQ.iterate(
				[&](const auto& group, auto& groupSymbols)
				{
					groupLog.append(L"GroupName: ").append(group).append(L"\n");
					groupSymbols.iterate(
						[&](const auto& path, auto& obj)
						{
							groupLog.append(L"GroupSymbolCon: ").append(path).append(L"\n");
						});
				});
			Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(L"{}", groupLog);
		}
		return 0;
	}

	int Follow::SyncSymbols()
	{
		SharedGet()->m_SymbolPathQ.clear();
		SharedGet()->m_OnConSymbolFieldQ.clear();

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
					SharedGet()->m_SymbolPathQ.push(pSymbol->Symbol(), path.c_str());
				}
				else
				{
					SharedGet()->m_SymbolPathQ.push(pSymbol->Symbol(), path);
				}
				SharedGet()->m_OnConSymbolFieldQ.push(pSymbol->Symbol(), Symbol);
			}
			pSymbol->Release();
		}
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Symbol total({})", SharedGet()->m_OnConSymbolFieldQ.size());
		return 0;
	}

	int Follow::SyncServerTime()
	{
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		auto conTime = ServerApi()->TimeCreate();
		ServerApi()->TimeGet(conTime);
		{
			Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info(
				"系统时间: 时区[{}],时差[{}],时间[{}]",
				shared::Business::MTGMT(conTime->TimeZone()),
				m_ServerTimeDiff.load(),
				sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));
		}
		conTime->Release();

		License(ServerApi()->TimeCurrent());

		return 0;
	}

	int Follow::SyncPositions()
	{
		SharedGet()->m_OnPositionFieldQ.clear();

		SharedGet()->m_OnUserFieldQ.iterate(
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
							SharedGet()->m_OnPositionPtrQ.push(newPosition->Position(), newPosition);
						}
						shared::PositionField Position;
						shared::Business::MakePositionField(pPosition, Position);
						SharedGet()->m_OnPositionFieldQ.push(pPosition->Position(), Position);

						SharedGet()->m_PositionDealQ.push(pPosition->Position(), 0);
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

									SharedGet()->m_PositionRelationQ.pushpush(
										dealField.ApiDataPosition,
										[&](auto& follows)
										{
											follows.push(dealField.PositionID, dealField.Deal);
										});

									SharedGet()->m_PositionDealQ.search(
										dealField.PositionID, 
										[&](const auto&,auto& deal) 
										{
											if (deal < dealField.Deal)
												deal = dealField.Deal;
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
		SharedGet()->m_PositionRelationQ.iterate(
			[&](const auto& signalPositionID, auto& followPositionFields)
			{
				if (!SharedGet()->m_OnPositionFieldQ.search(signalPositionID))
				{
					clearSignals.emplace_back(signalPositionID);
				}
				else
				{
					std::vector<sk::uint64> clearFollows;
					followPositionFields.iterate(
						[&](const auto& follow,auto& deal)
						{
							if (!SharedGet()->m_OnPositionFieldQ.search(follow))
							{
								clearFollows.emplace_back(follow);
							}
						});
					for (const auto& node : clearFollows)
					{
						followPositionFields.pop(node, [](const auto&,auto&) {});
					}
					if (followPositionFields.empty())
					{
						clearSignals.emplace_back(signalPositionID);
					}
				}
			});

		for (const auto& node : clearSignals)
		{
			SharedGet()->m_PositionRelationQ.pop(node, [](const auto&,auto&) {});
		}
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("业务持仓({}),MT持仓({}),信号持仓关系({})", SharedGet()->m_OnPositionFieldQ.size(), SharedGet()->m_OnPositionPtrQ.size(), SharedGet()->m_PositionRelationQ.size());

		return 0;
	}

	int Follow::Synchronize()
	{
		SyncServerTime();
		SyncGroups();
		SyncUsers();
		SyncSymbols();
		SyncPositions();
		return 0;
	}

	MTAPIRES Follow::PositionGetByTicket(
		__in const UINT64& ticket,
		__out shared::PositionField& outPositionField)
	{
		MTAPIRES result = MT_RET_ERROR;
		memset(&outPositionField, 0x00, sizeof(outPositionField));
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

	MTAPIRES Follow::DealGet(
		__in const UINT64& inTicketID,
		__out shared::DealField& outDealField)
	{
		MTAPIRES result = MT_RET_ERROR;
		memset(&outDealField, 0x00, sizeof(outDealField));
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

	MTAPIRES Follow::TradeRequest
	(
		__in shared::RequestField& reqField	
	)
	{
		MTAPIRES result = MT_RET_ERROR;
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




	MTAPIRES Follow::DealApiDataAt
	(
		__in const UINT64& ticket,
		__in const std::function<MTAPIRES(IMTDeal*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
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
	MTAPIRES Follow::PositionApiDataAt(
		__in const UINT64& ticket,
		__in const std::function<MTAPIRES(IMTPosition*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
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
	MTAPIRES Follow::PositionApiDataAt(
		__in const UINT64& login,
		__in const wchar_t* symbol,
		__in const std::function<MTAPIRES(IMTPosition*)>& cbProcess
	)
	{
		MTAPIRES result = MT_RET_ERROR;
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
	UINT64 Follow::ApiDataGet
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
	MTAPIRES Follow::ApiDataSet
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

	UINT64 Follow::ApiDataGet
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
	MTAPIRES Follow::ApiDataSet
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



