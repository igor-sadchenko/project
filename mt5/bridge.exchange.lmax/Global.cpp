#include "stdafx.h"
#include "Global.h"

namespace lmax {
	static Global static_global_Global;
	Global* GetGlobal() { return &static_global_Global; }

	Global::Global() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Global::~Global() {
		UnInit();
	}

	int Global::Init() {
		auto syspath = GetCurrentProcessDirectory();

		m_pLog = new sk::Log(syspath);
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_SIMPLE);
		m_pSetup = new Setup(syspath);
		m_pShared = new Shared();
		m_pApi = new shared::SharedApi(
			m_pLog,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_LIBFIX | shared::EnApiLoadFlag::EN_API_LOAD_FLAG_NETWORK,
			m_pSetup->ApiPath().c_str());

		m_pCore = new Core();

		return 0;
	}

	int Global::UnInit() {
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pApi);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pLog);
		SK_DELETE_PTR(m_pShared);
		return 0;
	}

	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}

	Core* Global::GetCore() {
		return GetGlobal()->m_pCore;
	}

	shared::SharedApi* Global::GetApi() {
		return GetGlobal()->m_pApi;
	}
	Setup* Global::GetSetup() {
		return GetGlobal()->m_pSetup;
	}
	void Global::SystemExit(const long& flag) {
		::InterlockedExchange(&GetGlobal()->m_SystemExitFlag, flag);
	}
	bool Global::SystemExit() {
		return ::InterlockedExchangeAdd(&GetGlobal()->m_SystemExitFlag, 0) == 1;
	}
	Shared* Global::SharedGet() {
		return GetGlobal()->m_pShared;
	}
	HWND Global::GetHwnd()
	{
		if (!GetGlobal()->m_hWnd)
			GetGlobal()->m_hWnd = ::GetConsoleWindow();
		return GetGlobal()->m_hWnd;
	}
}///namespace lmax
