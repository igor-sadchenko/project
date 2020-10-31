#include "stdafx.h"
#include "ServiceObj.h"

namespace follow {
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	IServiceObj::IServiceObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		m_pNetApi(pNetworkApi),
		m_pContext(pNetworkContext)
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_UNDEFINED;
	}
	IServiceObj::~IServiceObj()
	{

	}
	int IServiceObj::Write(const sk::network::EnNetCmd& cmd, const sk::packet& pak) const
	{
		if (!m_pContext) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		m_pNetApi->Packet(pak, head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}
	int IServiceObj::WriteEx(const sk::network::EnNetCmd& cmd, const sk::packet& pak, const SKAPIRES& retcode) const
	{
		if (!m_pContext) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		head.Retcode = static_cast<decltype(head.Retcode)>(retcode);
		m_pNetApi->Packet(pak, head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}
	int IServiceObj::WriteEx(const sk::network::EnNetCmd& cmd, const INT& iFlag, const SKAPIRES& retcode) const
	{
		if (!m_pContext) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		head.Retcode = static_cast<decltype(head.Retcode)>(retcode);
		head.DataExtern.ExternDataI = iFlag;
		m_pNetApi->Packet("", head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}
	int IServiceObj::SendLoginResult(const SKAPIRES& retcode)
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_100000070, "", retcode);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	AdminObj::AdminObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext)
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN;
	}

	AdminObj::~AdminObj()
	{

	}
	int AdminObj::SendMTSymbols() const
	{
		sk::packet symbols;
		Follow::SharedGet()->m_OnConSymbolFieldQ.iterate(
			[&](const auto& symbol, auto& symbolObj)
			{
				symbols.append((char*)&symbolObj, shared::LENCONSYMBOLFIELD);
			});
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000300, symbols);
	}
	int AdminObj::SendTradeRecordHead(const sk::packet& pak) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000451, pak);
	}
	int AdminObj::SendTradeRecordUpdate(const sk::packet& pak) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000452, pak);
	}
	int AdminObj::SendMTUsers() const
	{
		sk::packet users;
		Follow::SharedGet()->m_OnUserFieldQ.iterate(
			[&](const auto& login, auto& userObj)
			{
				shared::UserSimpleField user;
				user.Login = login;
				sk::SafeCopyA(user.Group, sk::StringConvert::WStringToMBytes(userObj.Group).c_str(), _countof(user.Group));
				users.append((char*)&user, shared::LENUSERSIMPLEFIELD);
			});
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000200, users);
	}
	int AdminObj::SendFollowconInitialize() const
	{
		auto pFollows = FollowGet()->FollowObjGet()->FollowconObj()->Map();
		sk::packet follows;
		if (pFollows && !pFollows->empty()) {
			pFollows->iterate(
				[&](const auto& followKey, auto& followCon)
				{
					follows.append((char*)&followCon, shared::LENFOLLOWFIELD);
				});
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000110, follows);
	}
	int AdminObj::SendFollowconUpdate(const SKAPIRES& retcode, const sk::packet& ack) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000120, ack, retcode);
	}
	int AdminObj::SendFollowconDelete(const SKAPIRES& retcode, const sk::packet& ack) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000130, ack, retcode);
	}
	int AdminObj::SendPlaformPatchPositions(const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000510, "", retcode);
	}
	int AdminObj::SendSystemSetup(const sk::packet& setup) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000020, setup);
	}
	int AdminObj::SendServerTimeS() const
	{
		auto current_time_s = sk::Helper::TickCountGet<std::chrono::seconds>();
		return Write(sk::network::EnNetCmd::EN_NETCMD_200000610, sk::packet((char*)&current_time_s, sizeof(decltype(current_time_s))));
	}
	int AdminObj::SendQuickFunctionSwitchFollow(const UINT& flag, const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000031, flag, retcode);
	}
	int AdminObj::SendQuickFunctionSwitchPatch(const UINT& flag, const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000032, flag, retcode);
	}
	int AdminObj::SendQuickFunctionSwitchSLTP(const UINT& flag, const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_200000033, flag, retcode);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	DeveloperObj::DeveloperObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext)
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_DEVELOPER;
	}

	DeveloperObj::~DeveloperObj()
	{

	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	GuestObj::GuestObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext)
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_GUEST;
	}

	GuestObj::~GuestObj()
	{

	}

}///namespace follow