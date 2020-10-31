#include "stdafx.h"
#include "Global.h"

namespace client {
	HINSTANCE extern_global_hinstance = nullptr;
	static Global static_global_Global;
	Global* GetGlobal() { return &static_global_Global; }

	Global::Global() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Global::~Global() {
		UnInit();
	}

	int Global::Init() {
		m_pSetup = new Setup(GetCurrentProcessDirectory());
		m_pObjectNotify = new sk::NotifyObject();
		m_pLog = new sk::Log(m_pSetup->ObjPath());
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_ALL);
#if !defined(_DEBUG)
		m_pApi = new shared::SharedApi(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			m_pSetup->ApiPath().c_str());
#else
		m_pApi = new shared::SharedApi(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			m_pSetup->ApiPath().c_str());
#endif
		m_pShared = new Shared();
		m_pCore = new Core();
		return 0;
	}

	int Global::UnInit() {
		if (m_pCore) m_pCore->Close();
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pObjectNotify);
		SK_DELETE_PTR(m_pLog);
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pSetup);
		return 0;
	}
	Core* Global::GetCore() {
		return GetGlobal()->m_pCore;
	}
	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}
	shared::SharedApi* Global::GetApi() {
		return GetGlobal()->m_pApi;
	}
	Shared* Global::SharedGet() {
		return GetGlobal()->m_pShared;
	}
	sk::NotifyObject* Global::GetObjectNotify() {
		return GetGlobal()->m_pObjectNotify;
	}
	Setup* Global::SetupGet() {
		return GetGlobal()->m_pSetup;
	}
	void Global::SystemExit()
	{
		GetGlobal()->m_SystemExit.store(true);
	}
	bool Global::IsSystemExit()
	{
		return GetGlobal()->m_SystemExit.load();
	}
}///namespace client