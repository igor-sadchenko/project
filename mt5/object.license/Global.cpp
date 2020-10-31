#include "stdafx.h"
#include "Global.h"

namespace local {
	static Global static_global_Global;
	Global* GetGlobal() { return &static_global_Global; }

	Global::Global() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Global::~Global() {
		UnInit();
	}

	int Global::Init() {
		m_pLog = new sk::Log(GetCurrentProcessDirectory());
		m_LogKey = m_pLog->Alloc(GetCurrentProcessName(), sk::EnLogType::EN_LOG_TYPE_SIMPLE);
		m_pCore = new Core();

		return 0;
	}

	int Global::UnInit() {
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pLog);
		return 0;
	}
	std::shared_ptr<spdlog::logger> Global::Log(sk::EnLogType logType)
	{
		return GetGlobal()->m_pLog->log(GetGlobal()->m_LogKey, logType);
	}
	Core* Global::GetCore() {
		return static_global_Global.m_pCore;
	}
}///namespace local
