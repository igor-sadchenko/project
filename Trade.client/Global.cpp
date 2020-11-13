#include "stdafx.h"
#include "Global.h"

namespace local {
	static Global gGlobal;
	HINSTANCE ghInstance = nullptr;
	Global* GlobalGet() { return &gGlobal; }
#if ENABLE_CEF
	CefRefPtr<local::SimpleApp> gpCefApp;
#endif
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
		m_pSetup = new Setup();

		m_pSharedApi = new shared::SharedApi(
			nullptr,
			shared::EnApiLoadFlag::EN_API_LOAD_FLAG_FIX,
			R"(..\..\bin\shared\x64\debug\)"
		);

		m_pCore = new Core();
		return 0;
	}

	int Global::UnInit()
	{
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pSharedApi);
		return 0;
	}

	Core* Global::CoreGet()
	{
		return GlobalGet()->m_pCore;
	}

	Setup* Global::SetupGet()
	{
		return GlobalGet()->m_pSetup;
	}

	shared::SharedApi* Global::SharedApi()
	{
		return GlobalGet()->m_pSharedApi;
	}










	bool Global::SystemExit()
	{
		return GlobalGet() ? GlobalGet()->m_SystemExit.load() : false;
	}
	void Global::SystemExit(const bool& flag)
	{
		GlobalGet()->m_SystemExit.store(flag);
	}
}///namespace local