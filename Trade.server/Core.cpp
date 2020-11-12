#include "stdafx.h"
#include "Core.h"

namespace local {

	Core::Core()
	{
		Init();
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
		m_pServer->Open();
		return 0;
	}

	int Core::Close() 
	{
		m_pServer->Close();
		return 0;
	}


}///namespace local