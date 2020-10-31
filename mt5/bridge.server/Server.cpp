#include "stdafx.h"
#include "Server.h"

namespace proxy {

	Server::Server() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Server::~Server() {
		UnInit();
	}

	int Server::Init() {
		sk::intptr out = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_tcp_server_setup>(shared::api_network, shared::api_network_tcp_server_setup)(
			Global::GetSetup()->GetServerTcpAddress().c_str(),
			out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}

	int Server::UnInit() {
		m_ServiceObjQ.iterate(
			[&](const auto&, auto& pServiceObj) {
				pServiceObj->Release();
				pServiceObj = nullptr;
			});
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	int Server::Open() {
		if (m_IsOpen.load()) return 0;
		m_IsOpenT.store(true);

		NetApi<sk::network::INetworkApi>()->Start(this);

		m_IsOpen.store(true);
		return 0;
	}

	int Server::Close() {
		if (!m_IsOpen.load()) return 0;

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "Shutting down network service . . . " << std::endl;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_ServiceObjQ.iterate([&](const auto&, auto& obj) {obj->Enable(false); });
		NetApi<sk::network::INetworkApi>()->Stop();
		m_ServiceObjQ.clear();

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << "Network service shutdown complete . " << std::endl;

		m_IsOpen.store(false);
		return 0;
	}


	void Server::OnTcpServerLogon(sk::network::INetworkContext* pContext)
	{

	}

	void Server::OnTcpServerLogout(sk::network::INetworkContext* pContext)
	{
		m_ServiceObjQ.pop(
			pContext->Flag(),
			[&](const auto&, IServiceObj* pServiceObj)
			{
				//!@ 准备队列标志位
				Global::GetSetup()->m_ReadyContextMapQ.search(pContext->Flag(), [&](const auto&, auto& status) {status = false; });


				switch (pServiceObj->ServiceObjFlagGet())
				{
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED:
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("MT报价网关模块(({:X})({}))退出.", pServiceObj->LoginFlag(), pContext->IPAddr());
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER:
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("MT经济商模块(({:X})({}))退出.", pServiceObj->LoginFlag(), pContext->IPAddr());
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA:
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("交易所报价模块(({:X})({}))退出.", pServiceObj->LoginFlag(), pContext->IPAddr());
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("管理器模块(({:X})({}))退出.", pServiceObj->LoginFlag(), pContext->IPAddr());
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER:
				{
					if (QuoteFinalGet()) //!@ 交易所下线,清理相应的聚合缓存数据
					{
						QuoteFinalGet()->Pop(shared::SystemIDType::ExchangeType(pContext->Flag()));
					}
					Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("交易所订单模块(({:X})({}))退出.", pServiceObj->LoginFlag(), pContext->IPAddr());
				}break;
				default:
				{
				}break;
				}///switch
				//!@ 关闭服务对象上下文
				pServiceObj->Release();

				Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->info("{}({})", __FUNCTION__, pContext->IPAddr());
			});
	}

	void Server::OnTcpServerRead(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::EnNetCmd::EN_NETCMD_100000070:
		{
			OnNetLoginProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000010:
		{
			OnNetSystemTimeProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000020:
		{
			OnNetSystemSetupProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000040:
		{
			OnNetExchangeOnlineProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000050:
		{
			OnNetLogpakProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000060:
		{
			OnNetSystemAssignLoginsPostOrderProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000410:
		{
			OnNetTradeOrderProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000414:
		{
			OnNetOrderFailedProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000415:
		{
			OnNetOrderTimeoutProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000420:
		{
			OnNetExchangePositionProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000510:
		{
			OnNetMTUsersProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000520:
		{
			OnNetMTSymbolsProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000530:
		{
			OnNetSystemPositionProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000110:
		{
			OnNetProductsGetProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000120:
		{
			OnNetProductsProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000310:
		{
			OnNetMarketDataUpdate(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000320:
		{
			OnNetMarketDataCache(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000210:
		{
			OnNetProductConProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000451:
		{
			OnNetTradeRecordHeadProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_300000452:
		{
			OnNetTradeRecordUpdateProcess(pContext, head, pak);
		}break;
		}///switch
	}












}///namespace proxy