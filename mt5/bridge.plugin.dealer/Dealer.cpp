#include "stdafx.h"
#include "Dealer.h"

namespace dealer {

	HINSTANCE global_hInstance = nullptr;
	Dealer* global_Dealer = nullptr;
	Dealer* DealerGet() { return global_Dealer; }

	Dealer::Dealer()
	{
		memset(&m_ServerInfo, 0x00, sizeof(m_ServerInfo));
	}

	Dealer::~Dealer()
	{

	}

	Setup* Dealer::SetupGet()
	{
		return DealerGet()->m_pSetup;
	}
	shared::SharedApi* Dealer::ApiGet()
	{
		return DealerGet()->m_pApi;
	}
	Exchange* Dealer::ExchangeGet()
	{
		return DealerGet()->m_pExchange;
	}
	Shared* Dealer::SharedGet()
	{
		return DealerGet()->m_pShared;
	}
	std::shared_ptr<spdlog::logger> Dealer::Log(sk::EnLogType logType)
	{
		return DealerGet()->m_pLog->log(DealerGet()->m_LogKey, logType);
	}
	void Dealer::Release()
	{
		delete this;
	}

	MTAPIRES Dealer::MTSynchronize()
	{
		MTAPIRES result = MT_RET_OK;
		if ((result = MTServerTimeSync()) != MT_RET_OK)
		{
			return result;
		}
		if ((result = MTConGroupSync()) != MT_RET_OK)
		{
			return result;
		}
		if ((result = MTUserSync()) != MT_RET_OK)
		{
			return result;
		}
		if ((result = MTSymbolSync()) != MT_RET_OK)
		{
			return result;
		}
		if ((result = MTPositionSync()) != MT_RET_OK)
		{
			return result;
		}
		return result;
	}

	MTAPIRES Dealer::Start(IMTServerAPI* pServerApi)
	{
		MTAPIRES result = MT_RET_OK;
		if (m_PluginReady.load()) 
			return result;
		if (!pServerApi)
			return MT_RET_ERR_PARAMS;

		m_pServerApi = pServerApi;

		License();

		if (sk::LicenseInfo::EnProductVersionType(LicenseGet()) == sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_INVALID)
		{
			m_pServerApi->LoggerOut(EnMTLogCode::MTLogErr, L"ProductVersionType ivnalid(%u),Please contact the developer!", "-1");
			return MT_RET_ERR_PARAMS;
		}


		{//!@ 插件服务环境准备 -- 开始
			if (result = ServerApi()->About(m_ServerInfo) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"ServerApi About() error.", result);
				return result;
			}
			if (result = ServerApi()->TradeSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"TradeSubscribe() error.", result);
				return result;
			}
			if (result = ServerApi()->DealSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"DealSubscribe() error.", result);
				return result;
			}
			if (result = ServerApi()->PositionSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"PositionSubscribe() error.", result);
				return result;
			}
			if (result = ServerApi()->TimeSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"TimeSubscribe() error.", result);
				return result;
			}
			if (result = ServerApi()->SymbolSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"SymbolSubscribe() error.", result);
				return result;
			}


			if ((result = ParamentersGet()) != MT_RET_OK)
			{
				return result;
			}

			if ((result = RouteCreate()) != MT_RET_OK)
			{
				return result;
			}

			m_pShared = new Shared();
			m_pSetup = new Setup(m_PathObj);
			if (m_pSetup->Load())
			{
				SK_DELETE_PTR(m_pShared);
				SK_DELETE_PTR(m_pSetup);
				return MT_RET_ERR_PARAMS;
			}

			m_pLog = new sk::Log(m_pSetup->ObjPath());
			m_LogKey = m_pLog->Alloc(sk::Helper::GetCurrentProcessNameA(global_hInstance), sk::EnLogType::EN_LOG_TYPE_ALL);
			m_pApi = new shared::SharedApi(m_pLog, shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK, m_pSetup->ApiPath().c_str());
			if (!m_pApi || !m_pApi->Verify())
			{
				SK_DELETE_PTR(m_pShared);
				SK_DELETE_PTR(m_pSetup);
				SK_DELETE_PTR(m_pApi);
				SK_DELETE_PTR(m_pLog);
				return MT_RET_ERR_MEM;
			}

			MTSynchronize();

			if (m_pSetup->EnableTcpto())
			{
				m_pExchange = new Exchange();
				if (m_pExchange->Open())
				{
					SK_DELETE_PTR(m_pExchange);
					return MT_RET_ERROR;
				}
			}

			m_pRequest = ServerApi()->TradeRequestCreate();
			m_pConfirm = ServerApi()->TradeConfirmCreate();

			if (MT_RET_OK != ServerApi()->DealerStart(m_DealerManagerLogin.load(), this))
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"not found dealer login(%u)", m_DealerManagerLogin.load());
				return MT_RET_ERROR;
			}

			m_PluginReady.store(true);
			m_Threads.emplace_back([&]() {ProcessT(); });
			m_Threads.emplace_back([&]() {DealerLockT(); });
			m_Threads.emplace_back([&]() {ProcessCheckT(); });
		}//!@ 插件服务环境准备 -- 结束

		return MT_RET_OK;
	}
	MTAPIRES Dealer::Stop()
	{
		ServerApi()->TradeUnsubscribe(this);
		ServerApi()->DealUnsubscribe(this);
		ServerApi()->TimeUnsubscribe(this);
		ServerApi()->SymbolUnsubscribe(this);
		ServerApi()->PositionUnsubscribe(this);
		ServerApi()->DealerStop(m_DealerManagerLogin.load(), this);
		m_PluginReady.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();
		if (m_pSetup->EnableTcpto())
		{
			if (m_pExchange)
			{
				m_pExchange->Close();
				SK_DELETE_PTR(m_pExchange);
			}
		}
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		MTRelease(m_pRequest);
		MTRelease(m_pConfirm);
		//sk::Helper::UnLoadDllAt(sk::Helper::GetCurrentProcessName(), sk::StringConvert::MBytesToWString(shared::api_network));
		return MT_RET_OK;
	}

	MTAPIRES Dealer::MTPositionSync()
	{
		MTAPIRES result = MT_RET_OK;
		SharedGet()->m_MTPositionQ.clear();
		SharedGet()->m_MTUserQ.iterate(
			[&](const auto& login, auto& user_obj)
			{
				auto pAyPosition = ServerApi()->PositionCreateArray();
				if (MT_RET_OK == ServerApi()->PositionGet(login, pAyPosition))
				{
					UINT UserPositionTotal = pAyPosition->Total();
					for (UINT i = 0; i < UserPositionTotal; ++i)
					{
						auto pPosition = pAyPosition->Next(i);
						if (!pPosition)
							continue;
						shared::PositionField Position;
						shared::Business::MakePositionField(pPosition, Position);
						SharedGet()->m_MTPositionQ.push(pPosition->Position(), Position);
					}
				}
			});
		return result;
	}
	MTAPIRES Dealer::MTConGroupSync()
	{
		MTAPIRES result = MT_RET_OK;
		SharedGet()->m_MTConGroupQ.clear();

		UINT nTotalGroup = ServerApi()->GroupTotal();
		for (UINT i = 0; i < nTotalGroup; ++i)
		{
			auto pGroup = ServerApi()->GroupCreate();
			if ((result = ServerApi()->GroupNext(i, pGroup)) == MT_RET_OK)
			{
				shared::ConGroupField GroupField;
				shared::Business::MakeConGroupField(pGroup, GroupField);
				SharedGet()->m_MTConGroupQ.push(pGroup->Group(), GroupField);
			}
			pGroup->Release();
			if (result != MT_RET_OK)
			{
				return result;
			}
		}
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Group total({})", SharedGet()->m_MTConGroupQ.size());
		return result;
	}

	MTAPIRES Dealer::MTUserSync()
	{
		SharedGet()->m_MTUserQ.clear();
		MTAPIRES result = MT_RET_OK;
		SharedGet()->m_MTConGroupQ.iterate(
			[&](const auto& group, auto& group_obj,const auto&,auto& itberak)
			{
				UINT64* logins = nullptr;
				UINT	 logins_total = 0;
				if ((result = ServerApi()->UserLogins(group.c_str(), logins, logins_total)) == MT_RET_OK)
				{
					for (decltype(logins_total) i = 0; i < logins_total; ++i)
					{
						auto pUser = ServerApi()->UserCreate();
						if ((result = ServerApi()->UserGet(logins[i], pUser)) == MT_RET_OK)
						{
							shared::UserField User;
							shared::Business::MakeUserField(pUser, User);
							SharedGet()->m_MTUserQ.push(logins[i], User);
						}
						pUser->Release();
						if (result != MT_RET_OK)
						{
							break;
						}
					}
				}
				if (result != MT_RET_OK)
				{
					itberak = true;
				}
			});
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("Users total({})", SharedGet()->m_MTUserQ.size());
		return result;
	}










	MTAPIRES Dealer::PositionCheck
	(
		__in const UINT64& ticket
	)
	{
		MTAPIRES result = MT_RET_ERR_NOTFOUND;
		if (ServerApi())
		{
			IMTPosition* position = ServerApi()->PositionCreate();
			if (position)
			{
				result = ServerApi()->PositionGetByTicket(ticket, position);
				position->Release();
			}
		}
		return result;
	}

	MTAPIRES Dealer::PositionGetByTicket(
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

	MTAPIRES Dealer::DealGet(
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

	MTAPIRES Dealer::TradeRequest
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




	MTAPIRES Dealer::DealApiDataAt
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
	MTAPIRES Dealer::PositionApiDataAt(
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
	MTAPIRES Dealer::PositionApiDataAt(
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
	UINT64 Dealer::ApiDataGet
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
	MTAPIRES Dealer::ApiDataSet
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

	UINT64 Dealer::ApiDataGet
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
	MTAPIRES Dealer::ApiDataSet
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

	int Dealer::License(__int64 time /*= 0*/)
	{
		if (time <= 0) time = ::time(0);
		sk::MachineHardwareInformation MachineLocal;
		if (sk::Helper::MachineHardwareInformationGet(MachineLocal))
			return -1;

		auto licenseResult = sk::License::Verify(
			sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_BRIGDE_PROXY,
			MachineLocal,
			time,
			sk::License::Load(sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_BRIGDE_PROXY),
			m_LicenseInfo);

		if (SKAPIRES::LICENSE_RET_SUCCESS != licenseResult)
		{
			LicenseSet(sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_INVALID);
			if(m_pLog)
				Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error(SKAPIRES_DESC(licenseResult));
		}
		else
		{
			LicenseSet(m_LicenseInfo.ProductVersionType);
		}
		return 0;
	}
}///namespace dealer