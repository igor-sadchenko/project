#include "stdafx.h"
#include "UIClient.h"

namespace client {

	void UIClient::OnTcpClientConnect()
	{
		auto sk = 0;
	}

	void UIClient::OnTcpClientDisconnect()
	{
		auto sk = 0;
	}

	void UIClient::OnTcpClientWelcome(const sk::network::Header& head, const sk::packet& pak)
	{
		NetLoginRequest(Global::SetupGet()->LoginField());
	}

	void UIClient::OnTcpClientRead(const sk::network::Header& head, const sk::packet& pak)
	{
		switch (head.Cmd)
		{
		case sk::network::EnNetCmd::EN_NETCMD_100000050:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000060:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_100000070:
		{

		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000020:
		{
			OnNetSystemSetup(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000031:
		{
			OnNetSystemQuickFunctionSwitchFollow(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000032:
		{
			OnNetSystemQuickFunctionSwitchPatch(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000033:
		{
			OnNetSystemQuickFunctionSwitchSLTP(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000110:
		{
			OnNetFollowCons(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000120:
		{
			OnNetFollowUpdate(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000130:
		{
			OnNetFollowDelete(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000200:
		{
			OnNetMTUsers(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000300:
		{
			OnNetMTSymbols(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000410:
		{
			OnNetFollowRecordInitialize(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000451:
		{
			OnNetSystemTradeRecordHead(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000452:
		{
			OnNetSystemTradeRecordUpdate(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000510:
		{
			OnNetPlaformPatchPositions(head, pak);
		}break;
		case sk::network::EnNetCmd::EN_NETCMD_200000610:
		{
			OnNetServerTimeUpdate(head, pak);
		}break;
		default:
		{
			break;
		}
		}///switch

		Global::SharedGet()->m_NetSync.Response(head.Cmd, head, pak);
	}








	int UIClient::NetFollowConsDeleteRequest(const sk::packet& followcon) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000130;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(followcon, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetFollowConsUpdateRequest(const sk::packet& followcon) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000120;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(followcon, head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemTradeRecordHead() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000451;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemTradeRecordUpdate(int count) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.DataExtern.ExternDataI = count;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000452;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetServerTimeRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000610;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetQuickFunctionSwitchFollow() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000031;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetQuickFunctionSwitchPatch() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000032;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetQuickFunctionSwitchSLTP() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000033;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemSetupRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000020;
		head.DataProcessFlag = sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetSystemSetupSend(const shared::FollowSystemSetup& setup) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000020;
		head.DataProcessFlag = sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE;
		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&setup,shared::LENFOLLOWSYSTEMSETUP), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetPlatfomPatchPositionRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000510;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetFollowRecordRequet() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000410;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetFollowConsRequest() const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_200000110;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet("", head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	int UIClient::NetLoginRequest(const shared::FollowObjLoginField& login) const
	{
		if (!NetApi<sk::network::INetworkApi>() || NetApi<sk::network::INetworkApi>()->NetStatus() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED)
			return -1;
		sk::network::Header head;
		head.Cmd = sk::network::EnNetCmd::EN_NETCMD_100000070;

		sk::packet enpak;
		NetApi<sk::network::INetworkApi>()->Packet(sk::packet((char*)&login, shared::LENLOGINFIELD), head, enpak);
		NetApi<sk::network::INetworkApi>()->Write(enpak);
		return 0;
	}
	void UIClient::OnNetFollowRecordUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
	}
	void UIClient::OnNetFollowRecordInitialize(const sk::network::Header& head, const sk::packet& pak)
	{
		auto logs = sk::Helper::OneUnknownParseStruct<shared::FollowLogField>(pak.data(), pak.size());
		if (logs && !logs->empty())
		{
			Global::SharedGet()->m_FollowLogQ.clear();
			for (const auto& log : *logs)
			{
				if (log.Verify())
				{
					Global::SharedGet()->m_FollowLogQ.push(log.TimeUpdate, log);
				}				
			}
		}
		if (!Global::SharedGet()->m_FollowLogQ.empty())
		{
			OnUIFollowRecordInitialize();
		}		
	}
	void UIClient::OnNetSystemTradeRecordHead(const sk::network::Header& head, const sk::packet& pak)
	{
		OnUISystemTradeRecordHead(pak);
	}

	void UIClient::OnNetSystemTradeRecordUpdate(const sk::network::Header& head, const sk::packet& records)
	{
		if (records.empty()) return;
		auto pList = GetCtrl<CListUI>(_TEXT("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"));
		if (!pList) return;

		if (pList->GetCount() > 1000)
			pList->RemoveAll();

		std::vector<sk::packet> logs;
		sk::Helper::OneUnknownParseA(records, '@', logs);

		for (auto rit = logs.rbegin(); rit != logs.rend(); ++rit)
		{
			std::vector<sk::packet> log;
			sk::Helper::OneUnknownParseA(*rit, '\t', log);


			auto pListElem = new CListTextElementUI();
			pList->AddAt(pListElem, 0);

			int pos = 0;
			for (const auto& item : log)
			{
				pListElem->SetText(pos, sk::StringConvert::MBytesToWString(item).c_str());
				++pos;
			}
		}
		pList->NeedParentUpdate();
	}
	void UIClient::OnNetServerTimeUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		OnUIServerTimeUpdate(*((time_t*)pak.data()));
	}
	void UIClient::OnNetSystemQuickFunctionSwitchFollow(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(SKAPIRES(head.Retcode)))
		{
			Global::SharedGet()->m_FollowFieldQ.iterate(
				[&](const auto&,shared::FollowField& followcon) {
					followcon.FollowFlag = head.DataExtern.ExternDataI;
				});	

			OnUIFollowConsUpdate();
		}
	}
	void UIClient::OnNetSystemQuickFunctionSwitchPatch(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(SKAPIRES(head.Retcode)))
		{
			Global::SharedGet()->m_FollowFieldQ.iterate(
				[&](const auto&, shared::FollowField& followcon) {
					followcon.FollowAddToFlag = head.DataExtern.ExternDataI;
				});

			OnUIFollowConsUpdate();
		}
	}
	void UIClient::OnNetSystemQuickFunctionSwitchSLTP(const sk::network::Header& head, const sk::packet& pak)
	{
		if (SKAPIRES_SUCCESS(SKAPIRES(head.Retcode)))
		{
			Global::SharedGet()->m_FollowFieldQ.iterate(
				[&](const auto&, shared::FollowField& followcon) {
					followcon.PointSLTPFlag = head.DataExtern.ExternDataI;
				});

			OnUIFollowConsUpdate();
		}
	}
	void UIClient::OnNetSystemSetup(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		Global::SetupGet()->SystemSetup(*((shared::FollowSystemSetup*)pak.data()));
		UISystemSetupSet();
	}
	void UIClient::OnNetPlaformPatchPositions(const sk::network::Header& head, const sk::packet& pak)
	{
		if (pak.empty())
			return;
		OnUIPlaformPatchPositions(SKAPIRES(head.Retcode));
	}
	void UIClient::OnNetMTUsers(const sk::network::Header& head, const sk::packet& pak)
	{
		auto users = sk::Helper::OneUnknownParseStruct<shared::UserSimpleField>(pak.data(), pak.size());
		if (users && !users->empty()) {
			for (const auto& user : *users) {
				Global::SharedGet()->m_MTUserQ.push(user.Login, user);
			}
		}
	}
	void UIClient::OnNetMTSymbols(const sk::network::Header& head, const sk::packet& pak)
	{
		auto symbols = sk::Helper::OneUnknownParseStruct<shared::ConSymbolField>(pak.data(), pak.size());
		if (symbols && !symbols->empty()) {
			for (const auto& symbol : *symbols) {
				Global::SharedGet()->m_MTConSymbolQ.push(symbol.Symbol, symbol);
			}
		}
	}
	void UIClient::OnNetFollowCons(const sk::network::Header& head, const sk::packet& pak)
	{
		auto follows = sk::Helper::OneUnknownParseStruct<shared::FollowField>(pak.data(), pak.size());
		if (follows && !follows->empty())
		{
			for (const auto& follow : *follows) 
			{
				Global::SharedGet()->m_FollowFieldQ.push(follow.GetKey(), follow);
			}
		}

		OnUIFollowConsUpdate();
	}
	void UIClient::OnNetFollowUpdate(const sk::network::Header& head, const sk::packet& pak)
	{
		auto follows = sk::Helper::OneUnknownParseStruct<shared::FollowField>(pak.data(), pak.size());
		if (follows && !follows->empty())
		{
			for (const auto& follow : *follows)
			{
				Global::SharedGet()->m_FollowFieldQ.push(follow.GetKey(), follow);
			}
		}
		OnUIFollowConsUpdate();
	}
	void UIClient::OnNetFollowDelete(const sk::network::Header& head, const sk::packet& pak)
	{		
		if (pak.empty())
			return;
		auto followKey = (shared::FollowKey*)pak.data();
		Global::SharedGet()->m_FollowFieldQ.pop(*followKey, [](const auto&, auto&) {});
		OnUIFollowConsUpdate();
	}
}///namespace client