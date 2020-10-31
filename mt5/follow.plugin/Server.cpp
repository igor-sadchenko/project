#include "stdafx.h"
#include "Server.h"

namespace follow {

	Server::Server()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Server::~Server()
	{
		UnInit();
	}

	int Server::Init()
	{
		sk::intptr out = 0;
		Follow::SharedApiGet()->GetFunction<shared::tf_api_network_tcp_server_setup>(shared::api_network, shared::api_network_tcp_server_setup)(
			Follow::SetupGet()->ServerTcpAddr().c_str(),
			out);
		NetApi<sk::network::INetworkApi>(out);
		return 0;
	}

	int Server::UnInit() {
		m_NetClientQ.iterate(
			[&](const auto&, auto& pContext)
			{
				pContext->Release();
				pContext = nullptr;
			});
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	int Server::Open() {
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);
		/*m_Threads.emplace_back([&]() {ProcessT(); });*/
		/*m_Threads.emplace_back([&]() {ProcessRuntimeNetCMDT(); });*/
		NetApi<sk::network::INetworkApi>()->Start(this);

		Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
		m_IsOpen.store(true);
		return 0;
	}

	int Server::Close() {
		if (!m_IsOpen.load()) return 0;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		NetApi<sk::network::INetworkApi>()->Stop();
		Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", __FUNCTION__);
		m_IsOpen.store(false);
		return 0;
	}

	void Server::OnTcpServerLogon(sk::network::INetworkContext* pContext)
	{

	}
	void Server::OnTcpServerLogout(sk::network::INetworkContext* pContext)
	{
		m_NetClientQ.pop(pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				switch (pServiceObj->ServiceObjFlag())
				{
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("管理器客户端退出({})", pContext->IPAddr());
				}break;
				case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_DEVELOPER:
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("开发者客户端退出({})", pContext->IPAddr());
				}break;
				default:
				{
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("未知客户端退出({})", pContext->IPAddr());
				}break;
				}
				pServiceObj->Release();
			});
	}

	void Server::OnTcpServerRead(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::EnNetCmd::EN_NETCMD_100000030: {//! Logon

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000040: {//! Logout
			OnTcpServerLogout(pContext);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000070: {//! LoginResult/LoginRequest
			OnNetLoginProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000020: {//! 系统配置
			OnNetSystemSetupProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000031: {//! 系统配置
			OnNetQuickFunctionSwitchFollow(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000032: {//! 系统配置
			OnNetQuickFunctionSwitchPatch(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000033: {//! 系统配置
			OnNetQuickFunctionSwitchSLTP(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000110: {//! 请求跟单配置
			OnNetFollowconRequest(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000120: {//! 跟单配置更新/创建
			OnNetFollowconUpdate(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000130: {//! 跟单配置删除
			OnNetFollowconDelete(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000200: {//! 请求MT用户
			OnNetMTUsersRequest(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000300: {//! 请求MT符号
			OnNetMTSymbolsRequest(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000410: {//! 请求交易记录初始化
			OnNetTradeRecordRequest(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000451: {//! 请求交易记录头
			OnNetTradeRecordHeadProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000452: {//! 请求交易记录更新
			OnNetTradeRecordUpdateProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000510: {//! 测试创建跟单配置
			OnNetPlaformPatchPositionsProcess(pContext, head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000610: {
			OnNetServerTimeRequest(pContext, head, pak);
		}break;
		}///switch
		Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({:X})", __FUNCTION__, head.Cmd);
	}

	void Server::ProcessRuntimeNetCMDT()
	{
		do {
#if 0
			if (!Follow::SharedGet()->m_RuntimeNetCMDQ.empty())
			{
				Follow::SharedGet()->m_RuntimeNetCMDQ.iterate_clear(
					[&](const sk::network::NetCmd& cmd, sk::packet& pak, bool& itbreak, bool& itclear)
					{
						switch (cmd)
						{
						case sk::network::NetCmd::EN_NETCMD_200000510:
						{
							FollowGet()->FollowObjGet()->FollowconObj()->Iterate(
								[&](const shared::FollowKey& key, shared::FollowField& follow)
								{
									if (shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE == follow.FollowAddToFlag)
									{
										FollowGet()->FollowObjGet()->m_FollowPatchInQ.push(follow);
									}
								});

							do {
								if (FollowGet()->FollowObjGet()->m_FollowPatchInQ.empty())
								{
									sk::network::NETPAKPTR pHead = (sk::network::NETPAKPTR)pak.data();
									m_NetClientQ.search(
										pHead->Serial,
										[&](const auto&, IServiceObj* pServiceObj)
										{
											shared::CommRequestResult res;
											res.Retcode = SKAPIRES::FOLLOW_RET_SUCCESS;
											ServiceObjType<AdminObj>(pServiceObj)->SendPlaformPatchPositions(SKAPIRES::FOLLOW_RET_SUCCESS,"");
										});
									itclear = true;
									break;
								}
							} while (1);
						}break;
						}
					});
			}
#endif
			if (!m_IsOpenT.load()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} while (1);
	}

	void Server::ProcessT() {
		do {
#if 0
			do {
				if (Follow::SharedGet()->m_PacketFormViewQ.empty())
					break;
				auto pak = Follow::SharedGet()->m_PacketFormViewQ.pop();
				if (!pak)
					break;
				m_NetClientQ.iterate(
					[&](const auto& context, auto& pServictObj)
					{
						if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_DEVELOPER)
						{
							auto pClient = (GuestObj*)pServictObj;
							//pClient->SendOnMsgMT(*pak);
						}
					});
			} while (1);

			do {
				if (Follow::SharedGet()->m_PacketFormViewLogQ.empty())
					break;
				auto pak = Follow::SharedGet()->m_PacketFormViewLogQ.pop();
				if (!pak)
					break;
				m_NetClientQ.iterate(
					[&](const auto& context, auto& pServictObj)
					{
						if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_DEVELOPER)
						{
							auto pClient = (GuestObj*)pServictObj;
							//pClient->SendRuntimLog(*pak);
						}
					});
			} while (1);

			do {
				if (Follow::SharedGet()->m_PacketFormManagerQ.empty())
					break;
				auto pak = Follow::SharedGet()->m_PacketFormManagerQ.pop();
				if (!pak)
					break;
				m_NetClientQ.iterate(
					[&](const auto& context, auto& pServictObj)
					{
						if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
						{
							auto pClient = (AdminObj*)pServictObj;
							pClient->SendTradeRecordUpdate(*pak);
						}
					});
			} while (1);

#endif

			if (!m_IsOpenT.load()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		} while (1);
	}
}///namespace follow