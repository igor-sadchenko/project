#include "stdafx.h"
#include "Global.h"

namespace local {
	Global* gpGlobal = nullptr;
	HINSTANCE ghInstance = nullptr;
	Global* GlobalGet() { return gpGlobal; }
#if ENABLE_CEF
	CefRefPtr<local::SimpleApp> gpCefApp;
#endif
	Global::Global()
	{
		Init();
	}

	Global::~Global()
	{
		UnInit();
	}

	int Global::Init()
	{
		return 0;
	}

	int Global::UnInit()
	{
		return 0;
	}
	bool Global::SystemExit()
	{
		return gpGlobal ? gpGlobal->m_SystemExit.load() : false;
	}
	void Global::SystemExit(const bool& flag)
	{
		gpGlobal->m_SystemExit.store(flag);
	}
}///namespace local