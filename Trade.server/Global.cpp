#include "stdafx.h"
#include "Global.h"

namespace local {
	static Global static_global_Global;
	Global* GlobalGet() { return &static_global_Global; }

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
		SK_DELETE_PTR(m_pSetup);
		SK_DELETE_PTR(m_pCore);
		SK_DELETE_PTR(m_pSharedApi);
		return 0;
	}

	Setup* Global::SetupGet()
	{
		return GlobalGet()->m_pSetup;
	}

	Core* Global::CoreGet()
	{
		return GlobalGet()->m_pCore;
	}

	shared::SharedApi* Global::SharedApiGet()
	{
		return GlobalGet()->m_pSharedApi;
	}
}///namespace local
