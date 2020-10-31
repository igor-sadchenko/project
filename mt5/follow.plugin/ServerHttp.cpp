#include "stdafx.h"
#include "ServerHttp.h"

namespace followplugin {

	ServerHttp::ServerHttp() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	ServerHttp::~ServerHttp() {
		UnInit();
	}

	int ServerHttp::Init() {
		sk::intptr NtApi = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_http_setup>(shared::api_network, shared::api_network_http_setup)(
			Global::SetupGet()->ServerHttpAddr().c_str(),
			NtApi);
		assert(NtApi);
		m_pNtApi = PtrToType(sk::network::INetworkApi, NtApi);
		return 0;
	}
	int ServerHttp::UnInit() {
		if (m_pNtApi) {
			m_pNtApi->Release();
		}
		return 0;
	}

	int ServerHttp::Open() {
		NtApi()->Open(this);
		return 0;
	}
	int ServerHttp::Close() {
		NtApi()->Close();
		return 0;
	}

	void ServerHttp::OnHttpRead(const sk::network::HttpPacket& request, sk::packet& response) {
		switch (request.Cmd) {
		case sk::network::NetCmd::EN_NETCMD_101001: {

		}break;
		case sk::network::NetCmd::EN_NETCMD_101002: {

		}break;
		case sk::network::NetCmd::EN_NETCMD_101003: {

		}break;
		case sk::network::NetCmd::EN_NETCMD_101004: {

		}break;
		case sk::network::NetCmd::EN_NETCMD_101005: {

		}break;
		}
	}

}///namespace followplugin