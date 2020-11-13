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
		m_pClient = new Client();
		return 0;
	}

	int Core::UnInit()
	{
		SK_DELETE_PTR(m_pClient);
		return 0;
	}

	int Core::Open()
	{
		m_pClient->Open();
		return 0;
	}

	int Core::Close()
	{
		m_pClient->Close();
		return 0;
	}



}///namespace local