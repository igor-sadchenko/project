#include "stdafx.h"
#include "Feeder.h"

namespace feeder {

	HINSTANCE global_hInstance = nullptr;
	Feeder* global_Feeder = nullptr;
	Feeder* FeederGet() { return global_Feeder; }

	Feeder::Feeder()
	{
		memset(&m_ServerInfo, 0x00, sizeof(m_ServerInfo));
	}

	Feeder::~Feeder()
	{

	}

	Setup* Feeder::SetupGet()
	{
		return FeederGet()->m_pSetup;
	}
	shared::SharedApi* Feeder::ApiGet()
	{
		return FeederGet()->m_pApi;
	}
	DataSource* Feeder::DataSourceGet()
	{
		return FeederGet()->m_pDataSource;
	}
	Shared* Feeder::SharedGet()
	{
		return FeederGet()->m_pShared;
	}
	std::shared_ptr<spdlog::logger> Feeder::Log(sk::EnLogType logType)
	{
		return FeederGet()->m_pLog->log(FeederGet()->m_LogKey, logType);
	}
	void Feeder::Release()
	{
		delete this;
	}

	MTAPIRES Feeder::MTSynchronize()
	{
		MTAPIRES result = MT_RET_OK;
		if ((result = MTServerTimeSync()) != MT_RET_OK)
		{
			return result;
		}
		if ((result = MTSymbolSync()) != MT_RET_OK)
		{
			return result;
		}
		return result;
	}




	MTAPIRES Feeder::Start(IMTServerAPI* pServerApi)
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
			if (result = ServerApi()->TickSubscribe(this) != MT_RET_OK)
			{
				ServerApi()->LoggerOut(EnMTLogCode::MTLogErr, L"%s(%u)", L"TickSubscribe() error.", result);
				return result;
			}

			if ((result = ParamentersGet()) != MT_RET_OK)
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
				m_pDataSource = new DataSource();
				if (m_pDataSource->Open())
				{
					SK_DELETE_PTR(m_pDataSource);
					return MT_RET_ERROR;
				}
			}
			m_PluginReady.store(true);
			m_Threads.emplace_back([&]() {ProcessT(); });
		}//!@ 插件服务环境准备 -- 结束

		return MT_RET_OK;
	}
	MTAPIRES Feeder::Stop()
	{
		ServerApi()->TickUnsubscribe(this);
		m_PluginReady.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();
		if (m_pSetup->EnableTcpto())
		{
			if (m_pDataSource)
			{
				m_pDataSource->Close();
				SK_DELETE_PTR(m_pDataSource);
			}
		}
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		//sk::Helper::UnLoadDllAt(sk::Helper::GetCurrentProcessName(), sk::StringConvert::MBytesToWString(shared::api_network));
		return MT_RET_OK;
	}

	int Feeder::License(__int64 time /*= 0*/)
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
}///namespace feeder