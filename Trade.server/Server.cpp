#include "stdafx.h"
#include "Server.h"

namespace local {

	Server::Server()
	{
		Init();
	}

	Server::~Server()
	{
		UnInit();
	}

	int Server::Init()
	{
		std::intptr_t fixapiptr = 0;
		if (Global::SharedApiGet()->GetFunction<shared::tf_api_fix_setup>(shared::api_fix, shared::api_fix_setup)(
			Global::SetupGet()->FixObjAcceptorPathname().c_str(), std::uint64_t(fix::EnFixMode::EN_FIX_MODE_ACCEPTOR),
			fixapiptr))
		{
			return -1;
		}
		FixApi(fixapiptr);
		return 0;
	}

	int Server::UnInit()
	{
		return 0;
	}

	int Server::Open()
	{
		if (m_IsOpen.load()) return 0;

		FixApi()->Start(this);
		
		m_IsOpen.store(true);
		return 0;
	}

	int Server::Close()
	{
		if (!m_IsOpen.load()) return 0;

		FixApi()->Stop();

		m_IsOpen.store(false);
		return 0;
	}

}///namespace local