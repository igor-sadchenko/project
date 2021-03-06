#include "stdafx.h"
#include "Global.h"

namespace proxy {
	static Global static_global_Global;
	Global* GetGlobal() { return &static_global_Global; }

	Global::Global() 
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Global::~Global() 
	{
		UnInit();
	}

	int Global::Init() 
	{
		//OpenProcess(0, TRUE, _getpid());
		m_pLog = new sk::Log(GetCurrentProcessDirectory());
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_SIMPLE);
		m_pSetup = new Setup();
#if !defined(_DEBUG)
		m_pApi = new shared::SharedApi(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			GetSetup()->GetApiPath().c_str());
#else
		m_pApi = new shared::SharedApi(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			R"(..\..\..\..\shared\x64\debug\)");
#endif

		License();

		auto sk = sk::Helper::GetCpuidEx();

		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn(
			"\n{}\n{}\n{}\n{}\n",
			LicenseInfo().ProductTypeStrA(),
			LicenseInfo().ProductVersionTypeStrA(),
			LicenseInfo().ProductLicenseToStrA(),
			LicenseInfo().ProductServiceToStrA());

		m_pShared = new Shared();
		m_pCore = new Core();

		return 0;
	}

	int Global::UnInit() 
	{
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		return 0;
	}

	sk::Log* Global::LogGet()
	{
		return GetGlobal()->m_pLog;
	}

	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}

	Core* Global::GetCore() 
	{
		return GetGlobal()->m_pCore;
	}

	shared::SharedApi* Global::GetApi()
	{
		return GetGlobal()->m_pApi;
	}

	Shared* Global::SharedGet() 
	{
		return GetGlobal()->m_pShared;
	}

	HANDLE Global::GetProcessHandle() 
	{
		return GetGlobal()->m_hProcess;
	}

	Setup* Global::GetSetup() 
	{
		return GetGlobal()->m_pSetup;
	}

	void Global::SystemExit(const long& flag)
	{
		::InterlockedExchange(&GetGlobal()->m_SystemExitFlag, flag);
	}

	bool Global::SystemExit() 
	{
		return ::InterlockedExchangeAdd(&GetGlobal()->m_SystemExitFlag, 0) == 1;
	}

	int Global::License(__int64 time)
	{
		if (time <= 0) time = ::time(0);

		sk::MachineHardwareInformation MachineLocal;
		if (sk::Helper::MachineHardwareInformationGet(MachineLocal))
		{
			for (int i = 0; i < 3; ++i)
			{
				Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", "Unknown hardware (MAC),The program cannot continue!");
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			SystemExit(1);
			return 0;
		}

		auto licenseResult = sk::License::Verify(
			sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_BRIGDE_PROXY,
			MachineLocal,
			time,
			sk::License::Load(sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_BRIGDE_PROXY),
			GetGlobal()->m_LicenseInfo);

		if (SKAPIRES::LICENSE_RET_SUCCESS != licenseResult)
		{
			GetGlobal()->LicenseSet(0);

			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error(SKAPIRES_DESC(licenseResult));

			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error(
				"{}",
				R"(

--------------------------------------------------------------------
    当前设备、机器没有经过授权许可，
    本系统程序功能将无法正常使用，由此给您带来不便敬请谅解。
    如有需求请联系作者获取体验版本授权，谢谢您！

                                      新生联创 开发团队
                                                  www.skstu.com                  
--------------------------------------------------------------------
)");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			SystemExit(1);
		}
		else
		{
			GetGlobal()->LicenseSet(1);
		}

		return 0;
	}

}///namespace proxy
