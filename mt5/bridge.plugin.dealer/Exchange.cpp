#include "stdafx.h"
#include "Exchange.h"

namespace dealer {

	Exchange::Exchange()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Exchange::~Exchange()
	{
		UnInit();
	}

	int Exchange::Init()
	{
		sk::intptr out = 0;
		Dealer::ApiGet()->GetFunction<shared::tf_api_network_tcp_client_setup>(shared::api_network, shared::api_network_tcp_client_setup)(DealerGet()->SetupGet()->TcptoAddr().c_str(), out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}

	int Exchange::UnInit()
	{
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	bool Exchange::Ready() const
	{
		return
			m_IsOpen.load() &&
			ReadySystemSetup.load() &&
			ReadyProductCon.load() &&
			NetApi<sk::network::INetworkApi>()->NetStatus() == sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED;
	}

	int Exchange::Open()
	{
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);
		NetApi<sk::network::INetworkApi>()->Start(this);

		m_IsOpen.store(true);
		return 0;
	}

	int Exchange::Close()
	{
		if (!m_IsOpen.load()) return 0;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();
		NetApi<sk::network::INetworkApi>()->Stop();

		m_IsOpen.store(false);
		return 0;
	}

	void Exchange::OnTcpClientConnect()
	{
		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info("{}", "Connecting to exchange server success.");
	}
	void Exchange::OnTcpClientDisconnect()
	{
		ReadySystemSetup.store(false);
		ReadyLoginVeirfy.store(false);
		ReadyProductCon.store(false);

		Dealer::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info("{}", "DisConnecting to exchange server success.");
	}
	void Exchange::OnTcpClientWelcome(const sk::network::Header&, const sk::packet&)
	{
		NetLoginRequest();
	}
	void Exchange::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::NetCmd::EN_NETCMD_100000070://!@ Bridge Login Result
		{
			OnNetLoginResult(head,pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000010:
		{
			OnNetSystemTimeUpdate(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000020: 
		{
			OnNetSystemSetupUpdate(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000060:
		{
			OnNetSystemAssignLoginsPostOrder(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000110:
		{
			OnNetProductsGet(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000120:
		{
			OnNetSystemProducts(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000410:
		{
			OnNetTradeOrderRes(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000510:
		{
			OnNetMTUsersGet(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000520:
		{
			OnNetMTSymbolsGet(head, pak);
		}break;
		case sk::network::NetCmd::EN_NETCMD_300000530:
		{
			OnNetSystemPositionsGet(head, pak);
		}break;
		default:
		{

		}break;
		}///switch
	}


}///namespace dealer