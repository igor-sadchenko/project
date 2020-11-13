#include "stdafx.h"
#include "Client.h"

namespace local {

	Client::Client()
	{
		Init();
	}

	Client::~Client()
	{
		UnInit();
	}

	int Client::Init()
	{
		std::intptr_t fixapiptr = 0;
		if (Global::SharedApi()->GetFunction<shared::tf_api_fix_setup>(shared::api_fix, shared::api_fix_setup)(
			Global::SetupGet()->FixObjInitiatorPathname().c_str(), std::uint64_t(fix::EnFixMode::EN_FIX_MODE_INITIATOR),
			fixapiptr))
		{
			return -1;
		}
		FixApi(fixapiptr);
		return 0;
	}

	int Client::UnInit()
	{
		FixApi()->Release();
		return 0;
	}

	int Client::Open()
	{
		if (m_IsOpen.load()) return 0;

		FixApi()->Start(this);

		m_IsOpen.store(true);
		return 0;
	}

	int Client::Close()
	{
		if (!m_IsOpen.load()) return 0;

		FixApi()->Stop();

		m_IsOpen.store(false);
		return 0;
	}

}///namespace local