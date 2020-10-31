#include "stdafx.h"
#include "ServerPipe.h"

namespace proxy {
	ServerPipe::ServerPipe() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	ServerPipe::~ServerPipe() {
		UnInit();
	}

	int ServerPipe::Init() {
		sk::intptr out = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_pipe_server_setup>(shared::api_network, shared::api_network_pipe_server_setup)(
			"\\\\.\\pipe\\TestPipeName",
			out);
		NetApi<sk::network::IPipeApi>(out);
		return 0;
	}

	int ServerPipe::UnInit() {
		NetApi<sk::network::IPipeApi>()->Release();
		return 0;
	}

	int ServerPipe::Open() {
		if (m_IsOpen.load()) return 0;
		m_IsOpenT.store(true);

		NetApi<sk::network::IPipeApi>()->Start(this);

		m_IsOpen.store(true);
		return 0;
	}

	int ServerPipe::Close() {
		if (!m_IsOpen.load()) return 0;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		NetApi<sk::network::IPipeApi>()->Stop();

		m_IsOpen.store(false);
		return 0;
	}

}///namespace proxy