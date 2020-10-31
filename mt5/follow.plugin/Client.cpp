#include "stdafx.h"
#include "Client.h"

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
		std::vector<sk::packet> reppak;
		sk::network::Header head;
		head.Cmd = (cmd);
		m_pNetApi->Packet(pak, head, reppak);
		m_pContext->Write(reppak);
		return 0;
	}
	int IServiceObj::SendLoginResult(const shared::FollowObjLoginField& loginResult)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_100201, sk::packet((char*)&loginResult, shared::LENFOLLOWOBJLOGINFILED));
	}
	int IServiceObj::SendTradeRequestAck(const sk::packet& ack) {
		return Write(sk::network::EnNetCmd::EN_NETCMD_102108, ack);
	}
	int IServiceObj::SendPositioncons()
	{
		sk::packet users;
		Follow::SharedGet()->m_OnUserFieldQ.iterate(
			[&](const auto& login, auto& userObj)
			{
				users.append((char*)&userObj, shared::LENUSERFIELD);
			});
		return Write(sk::network::EnNetCmd::EN_NETCMD_102105, users);
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
	int AdminObj::SendMTSymbols()
	{
		sk::packet symbols;
		Follow::SharedGet()->m_OnConSymbolFieldQ.iterate(
			[&](const auto& symbol, auto& symbolObj)
			{
				symbols.append((char*)&symbolObj, shared::LENCONSYMBOLFIELD);
			});
		return Write(sk::network::EnNetCmd::EN_NETCMD_102106, symbols);
	}
	int AdminObj::SendTradeRecordInit(const sk::packet& pak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_10030B, pak);
	}
	int AdminObj::SendTradeRecordUpdate(const sk::packet& pak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_10030C, pak);
	}
	int AdminObj::SendMTUsers()
	{
		sk::packet users;
		Follow::SharedGet()->m_OnUserFieldQ.iterate(
			[&](const auto& login, auto& userObj)
			{
				shared::UserSimpleField user;
				user.Login = login;
				sk::SafeCopyW(user.Group, userObj.Group, _countof(user.Group));
				users.append((char*)&user, shared::LENUSERSIMPLEFIELD);
			});
		return Write(sk::network::EnNetCmd::EN_NETCMD_102105, users);
	}
	int AdminObj::SendFollowcons()
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
		return Write(sk::network::EnNetCmd::EN_NETCMD_102101, follows);
	}
	int AdminObj::SendFollowconCreateAck(const sk::packet& ack)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102103, ack);
	}
	int AdminObj::SendFollowconDeleteAck(const sk::packet& ack)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102104, ack);
	}
	int AdminObj::SendPlaformPatchPositionsAck(const sk::packet& ack)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102201, ack);
	}
	int AdminObj::SendPlaformPatchPositionsResult(const sk::packet& res)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102202, res);
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
	int DeveloperObj::SendOnMsgMT(const sk::packet& msgpak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102001, msgpak);
	}
	int DeveloperObj::SendRuntimLog(const sk::packet& logpak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102002, logpak);
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
	int GuestObj::SendOnMsgMT(const sk::packet& msgpak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102001, msgpak);
	}
	int GuestObj::SendRuntimLog(const sk::packet& logpak)
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_102002, logpak);
	}

}///namespace follow