#include "stdafx.h"
#include "Global.h"

namespace manager {
	static Global static_global_Global;
	Global* GetGlobal() { return &static_global_Global; }

	Global::Global() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Global::~Global() {
		UnInit();
	}

	int Global::Init() {
		auto sysPath = GetCurrentProcessDirectory();
		m_pSetup = new Setup(sysPath);
		if (0 != m_pSetup->Load())
		{
			return -1;
		}

		m_pLog = new sk::Log(sysPath);
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_SIMPLE);

		m_pShared = new Shared();
		m_pCore = new Core();
		return 0;
	}

	int Global::UnInit() {
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pShared);
		SK_DELETE_PTR(m_pLog);
		SK_DELETE_PTR(m_pSetup);
		return 0;
	}

	Core* Global::CoreGet()
	{
		return static_global_Global.m_pCore;
	}

	Shared* Global::SharedGet()
	{
		return static_global_Global.m_pShared;
	}

	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}

	Setup* Global::SetupGet()
	{
		return GetGlobal()->m_pSetup;
	}
}///namespace manager
