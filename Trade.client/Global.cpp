#include "stdafx.h"
#include "Global.h"

namespace local {
	Global* gpGlobal = nullptr;
	HINSTANCE ghInstance = nullptr;
	Global* GlobalGet() { return gpGlobal; }

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
		m_pWindow = new Window();
		return 0;
	}

	int Global::UnInit()
	{
		SK_DELETE_PTR(m_pWindow);
		return 0;
	}

	
	Window* Global::WindowGet()
	{
		return gpGlobal ? gpGlobal->m_pWindow : nullptr;
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