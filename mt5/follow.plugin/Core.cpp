#include "stdafx.h"
#include "Core.h"

namespace follow {

	Core::Core()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Core::~Core()
	{
		UnInit();
	}

	int Core::Init()
	{
		m_pServer = new Server();
		return 0;
	}
	int Core::UnInit()
	{
		SK_DELETE_PTR(m_pServer);
		return 0;
	}

	int Core::Open() 
	{
		if (m_IsOpen.load()) { return 0; }

		if (Global::SetupGet()->EnableServerTcp())
		{
			ServerGet()->Open();
		}

		m_IsOpen.store(true);
		return 0;
	}

	int Core::Close() 
	{
		if (!m_IsOpen.load()) { return 0; }

		if (Global::SetupGet()->EnableServerTcp())
		{
			ServerGet()->Close();
		}

		m_IsOpen.store(false);
		return 0;
	}
}///namespace follow