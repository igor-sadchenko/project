#include "stdafx.h"
#include "Global.h"

namespace follow {
	HINSTANCE extern_global_hinstance = nullptr;
	Global* static_global_Global = nullptr;
	FollowPlugin* static_global_FollowPlugin = nullptr;
	Global* GetGlobal() { return static_global_Global; }
	FollowPlugin* FollowPluginGet() { return static_global_FollowPlugin; }

	Global::Global() 
	{
	}

	Global::~Global() 
	{
	}

	int Global::Init() {
		sk::stringa curProcPath = sk::Helper::GetCurrentProcessDirectoryA(extern_global_hinstance).get();
		m_pSetup = new Setup(m_SystemPath);
		if (m_pSetup->Load()) {
#if defined(_DEBUG)
			sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !");
#endif
			return -1;
		}
		m_pLog = new sk::Log(m_pSetup->LogsPath());
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_ALL);
#if defined(_DEBUG)
		m_pApi = new shared::SKAPI(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			m_pSetup->ApiPath().c_str(), "skapi.network.d.dll");
#else
		m_pApi = new shared::SKAPI(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			m_pSetup->ApiPath().c_str());
#endif
		m_pShared = new Shared();
		m_pCore = new Core();
		return 0;
	}

	int Global::UnInit() {
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		return 0;
	}
	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}
	shared::SKAPI* Global::GetApi() {
		return GetGlobal()->m_pApi;
	}
	Core* Global::GetCore() {
		return GetGlobal()->m_pCore;
	}
	Setup* Global::SetupGet() {
		return GetGlobal()->m_pSetup;
	}
	const bool Global::EnableRuntimeLog()
	{
		if (GetGlobal()->m_pSetup)
		{
			return GetGlobal()->m_pSetup->EnableLog();
		}
		return false;
	}
	Shared* Global::SharedGet() 
	{
		return GetGlobal()->m_pShared;
	}
	void Global::SystemPath(const sk::stringa& path)
	{
		GetGlobal()->m_SystemPath = path;
	}
	sk::stringa Global::SystemPath()
	{
		return GetGlobal()->m_SystemPath;
	}
	void Global::SendLogToView(const sk::packet& log) 
	{
		if (GetGlobal()->m_pShared)
		{
			GetGlobal()->m_pShared->m_PacketFormViewLogQ.push(log);
		}
	}

	int Global::License(__int64 time /*= 0*/)
	{
		if (time <= 0) time = ::time(0);
		std::set<std::string> macs;
		auto HardwareInfo = sk::Helper::GetLocalMachineAllNetWorkCardMAC(macs);

		auto licenseResult = sk::License::Verify(
			sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_SA,
			HardwareInfo.c_str(),
			time,
			sk::License::Load(sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_SA),
			GetGlobal()->m_LicenseInfo);

		if (SKAPIRES::LICENSE_RET_SUCCESS != licenseResult)
		{
			GetGlobal()->m_License.store(false);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error(SKAPIRES_DESC(licenseResult));
		}
		else
		{
			GetGlobal()->m_License.store(true);
		}
		return 0;
	}
}///namespace follow