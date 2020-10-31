#include "stdafx.h"
#include "Server.h"

namespace follow {
	void Server::OnNetMTSymbolsRequest(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					auto pClient = (AdminObj*)pServictObj;
					pClient->SendMTSymbols();
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetMTUsersRequest(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					auto pClient = (AdminObj*)pServictObj;
					pClient->SendMTUsers();
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetPlaformPatchPositionsProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
			m_NetClientQ.search(
				pContext->Flag(),
				[&](const auto&, auto& pServictObj)
				{
					if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
					{
						SKAPIRES result = SKAPIRES::FOLLOW_RET_ERROR;
						if (FollowGet()->FollowObjGet()->m_TradeFollowSignalQ.empty())
						{
							result = FollowGet()->FollowObjGet()->FollowProcessPatchIn();
						}
						else
						{
							result = SKAPIRES::FOLLOW_RET_ERROR_SYSTEM_BUSY;
						}
						ServiceObjType<AdminObj>(pServictObj)->SendPlaformPatchPositions(result);
					}
			});
	}
	void Server::OnNetTradeRecordRequest(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					auto pClient = (AdminObj*)pServictObj;
					auto logs = FollowGet()->FollowObjGet()->DiskLogGet()->PopNewest();
#if 0
					sk::packet logspak;
					if (logs)
					{
						do {
							auto log = logs->pop();
							if (!log) break;
							logspak.append((char*)log.get(), shared::LENFOLLOWLOGFIELD);
						} while (1);
					}
					pClient->SendTradeRecordInit(logspak);
#endif
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetFollowconUpdate(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		bool result_create = false;
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					auto pClient = (AdminObj*)pServictObj;
					auto pFollow = (shared::FollowField*)pak.data();
					if (pFollow && pFollow->Verify())
					{
						auto result = FollowGet()->FollowObjGet()->FollowconObj()->Push(pFollow->GetKey(), *pFollow);
						switch (result)
						{
						case decltype(result)::EN_PUSHED_RESULT_CREATE: {
							Follow::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info("user follow configure({}) create.", pFollow->GetKey().Get());
						}break;
						case decltype(result)::EN_PUSHED_RESULT_REPLACE: {
							Follow::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info("user follow configure({}) replace.", pFollow->GetKey().Get());
						}break;
						}
						if (pFollow->FollowAddToFlag == shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE)
						{
							FollowGet()->FollowObjGet()->m_FollowPatchInQ.push(*pFollow);
						}
						result_create = true;
					}
				}
			});

		m_NetClientQ.iterate(
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					ServiceObjType<AdminObj>(pServictObj)->SendFollowconUpdate(
						result_create ? SKAPIRES::FOLLOW_RET_SUCCESS_CREATE_CON : SKAPIRES::FOLLOW_RET_ERROR_CREATE_CON
						, pak);
				}
			});
		
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}

	void Server::OnNetFollowconDelete(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak) 
	{
		bool result_delete = false;
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				auto pClient = (AdminObj*)pServictObj;
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					if (!pak.empty())
					{
						auto pFollowKey = (shared::FollowKey*)pak.data();
						auto idle = FollowGet()->FollowObjGet()->FollowconObj()->Pop(*pFollowKey);
						Follow::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info("{} user follow configure({}) delete.", __FUNCTION__, pFollowKey->Get());
						result_delete = true;
					}
				}
			});

		m_NetClientQ.iterate(
			[&](const auto&, auto& pServictObj)
			{
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					ServiceObjType<AdminObj>(pServictObj)->SendFollowconDelete(
						result_delete ? SKAPIRES::FOLLOW_RET_SUCCESS_DELETE_CON : SKAPIRES::FOLLOW_RET_ERROR_DELETE_CON
						, pak);
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetServerTimeRequest(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak) {
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				auto pClient = (AdminObj*)pServictObj;
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					pClient->SendServerTimeS();
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetFollowconRequest(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak) {
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServictObj)
			{
				auto pClient = (AdminObj*)pServictObj;
				if (pServictObj->ServiceObjFlag() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
				{
					pClient->SendFollowconInitialize();
				}
			});
		FollowGet()->Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}({})", __FUNCTION__, pContext->IPAddr());
	}
	void Server::OnNetSystemSetupProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pContext->Flag())))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
		{
			switch (head.DataProcessFlag)
			{
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT:
				[[fallthrough]];
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE:
			{
				if (pak.empty())
					break;
				shared::FollowSystemSetup* setup = (shared::FollowSystemSetup*)pak.data();
				if (setup)
				{

					Follow::SetupGet()->SystemSetupSet(*setup);
					m_NetClientQ.iterate(
						[&](const auto&, IServiceObj* clientObj)
						{
							ServiceObjType<AdminObj>(clientObj)->SendSystemSetup(sk::packet((char*)setup, shared::LENFOLLOWSYSTEMSETUP));
						});
					Follow::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("{}({})", __FUNCTION__, "System setup updated.");
				}
			}break;
			case sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT:
			{
				auto setup = Follow::SetupGet()->SystemSetupGet();
				m_NetClientQ.search(
					pContext->Flag(),
					[&](const auto&, auto& clientObj)
					{
						ServiceObjType<AdminObj>(clientObj)->SendSystemSetup(sk::packet((char*)&setup, shared::LENFOLLOWSYSTEMSETUP));
					});
			}break;
			}///switch
		}break;
		default:
		{

		}break;
		}///switch

	}
	void Server::OnNetLoginProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
		{
			pContext->Closed();
			return;
		}
		auto pLoginField = (shared::FollowObjLoginField*)pak.data();

		SKAPIRES LoginResult = SKAPIRES::FOLLOW_RET_ERROR;
		auto pServiceObj = std::make_shared<ServiceObj>(NetApi<sk::network::INetworkApi>(), pContext);

		Follow::SetupGet()->m_ClientsMapQ.search(
			pLoginField->ID,
			[&](const auto&, auto& clientField)
			{
				LoginResult = SKAPIRES::FOLLOW_RET_SUCCESS;
				if (sk::SafeCmpA(clientField.Password, pLoginField->Password, _countof(clientField.Password)))
				{
					LoginResult = SKAPIRES::FOLLOW_RET_ERROR_LOGIN_PASSWORD;
				}
				if (SKAPIRES_SUCCESS(LoginResult))
				{
					m_NetClientQ.iterate(
						[&](const auto& serviceObjID, auto& serviceObj, const auto&, auto& itbreak)
						{
							if (serviceObjID == pLoginField->ID)
							{
								itbreak = true;
								LoginResult = SKAPIRES::FOLLOW_RET_ERROR_LOGIN_REPEAT;
							}
						});
				}
			});

		if (!SKAPIRES_SUCCESS(LoginResult))
		{
			pServiceObj->SendLoginResult(LoginResult);
			pContext->Closed();
			return;
		}


		pContext->Flag(pLoginField->ID);

		IServiceObj* pTargetClient = nullptr;
		switch (shared::EnServiceObjectFlag(shared::SystemIDType::ServiceObjectType(pLoginField->ID)))
		{
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN:
		{
			pTargetClient = new AdminObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;
			m_NetClientQ.push(pContext->Flag(), pTargetClient);
			ServiceObjType<AdminObj>(pTargetClient)->SendMTSymbols();
			ServiceObjType<AdminObj>(pTargetClient)->SendMTUsers();
			ServiceObjType<AdminObj>(pTargetClient)->SendSystemSetup(sk::packet((char*)&Follow::SetupGet()->SystemSetupGet(), shared::LENFOLLOWSYSTEMSETUP));
		}break;
		case shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_DEVELOPER:
		{
			pTargetClient = new GuestObj(NetApi<sk::network::INetworkApi>(), pContext);
			if (!pTargetClient)
				break;
			m_NetClientQ.push(pContext->Flag(), pTargetClient);
		}break;
		default:
		{
		}break;
		}///switch

		if (SKAPIRES_SUCCESS(LoginResult) && pTargetClient)
		{
			pServiceObj->SendLoginResult(SKAPIRES::FOLLOW_RET_SUCCESS);
		}
		else
		{
			pServiceObj->SendLoginResult(SKAPIRES::FOLLOW_RET_ERROR);
			pContext->Closed();
		}

	}
	void Server::OnNetQuickFunctionSwitchFollow(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(), 
			[&](const auto&,auto& pServiceObj)
			{			
				if (FollowGet()->FollowObjGet()->FollowconObj()->Empty())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchFollow(0, SKAPIRES::FOLLOW_RET_ERROR);
				}
				else
				{
					int flag = -1;
					FollowGet()->FollowObjGet()->FollowconObj()->Iterate(
						[&](const auto&, shared::FollowField& Followcon)
						{
							if (flag < 0)
							{
								if (Followcon.FollowFlag == shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_ENABLE)
									Followcon.FollowFlag = shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_DISABLE;
								else
									Followcon.FollowFlag = shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_ENABLE;

								flag = Followcon.FollowFlag;
							}
							else
							{
								Followcon.FollowFlag = flag;
							}
						});

					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchFollow(flag, SKAPIRES::FOLLOW_RET_SUCCESS);
				}
			});

	}
	void Server::OnNetQuickFunctionSwitchPatch(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				if (FollowGet()->FollowObjGet()->FollowconObj()->Empty())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchPatch(0, SKAPIRES::FOLLOW_RET_ERROR);
				}
				else
				{
					int flag = -1;
					FollowGet()->FollowObjGet()->FollowconObj()->Iterate(
						[&](const auto&, shared::FollowField& Followcon)
						{
							if (flag < 0)
							{
								if (Followcon.FollowAddToFlag == shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE)
									Followcon.FollowAddToFlag = shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_DISABLE;
								else
									Followcon.FollowAddToFlag = shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE;

								flag = Followcon.FollowAddToFlag;
							}
							else
							{
								Followcon.FollowAddToFlag = flag;
							}
						});

					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchPatch(flag, SKAPIRES::FOLLOW_RET_SUCCESS);
				}
			});
	}
	void Server::OnNetQuickFunctionSwitchSLTP(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				if (FollowGet()->FollowObjGet()->FollowconObj()->Empty())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchSLTP(0, SKAPIRES::FOLLOW_RET_ERROR);
				}
				else
				{
					int flag = -1;
					FollowGet()->FollowObjGet()->FollowconObj()->Iterate(
						[&](const auto&, shared::FollowField& Followcon)
						{
							if (flag < 0)
							{
								if (Followcon.PointSLTPFlag == shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE)
									Followcon.PointSLTPFlag = shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_DISABLE;
								else
									Followcon.PointSLTPFlag = shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE;

								flag = Followcon.PointSLTPFlag;
							}
							else
							{
								Followcon.PointSLTPFlag = flag;
							}
						});

					ServiceObjType<AdminObj>(pServiceObj)->SendQuickFunctionSwitchSLTP(flag, SKAPIRES::FOLLOW_RET_SUCCESS);
				}
			});
	}
	void Server::OnNetTradeRecordUpdateProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		auto logs = FollowGet()->FollowObjGet()->DiskLogGet()->PopNewest();
		sk::packet records;
		if (head.DataExtern.ExternDataI > 0)
		{
			size_t count = 0;
			for (auto rit = logs.rbegin(); rit != logs.rend(); ++rit)
			{
				++count;
				records.append(*rit).append("@");
				if (count >= head.DataExtern.ExternDataI)
					break;
			}
		}

		if (!records.empty()) records.pop_back();

		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, auto& pServiceObj)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlag())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordUpdate(records);
				}
			});
	}
	void Server::OnNetTradeRecordHeadProcess(sk::network::INetworkContext* pContext, const sk::network::Header& head, const sk::packet& pak)
	{
		auto vhead = FollowGet()->FollowObjGet()->DiskLogGet()->HeadStr();
		sk::packet shead;
		for (const auto& node : vhead)
		{
			shead.append(node).append("\t");
		}
		if (!shead.empty()) shead.pop_back();

		m_NetClientQ.search(
			pContext->Flag(),
			[&](const auto&, IServiceObj* pServiceObj)
			{
				if (shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN == pServiceObj->ServiceObjFlag())
				{
					ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordHead(shead);
				}
			});
	}



}///namespace follow