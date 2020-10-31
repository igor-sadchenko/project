#include "stdafx.h"
#include "ServiceObj.h"


namespace proxy {
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	IServiceObj::IServiceObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		m_pNetApi(pNetworkApi),
		m_pContext(pNetworkContext) 
	{
		m_Address = pNetworkContext->IPAddr();
		Enable(true);
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_UNDEFINED;
	}

	IServiceObj::~IServiceObj() 
	{

	}

	int IServiceObj::Write(const sk::network::EnNetCmd& cmd, const sk::packet& pak) const 
	{
		if (!m_pContext || !Enable()) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		m_pNetApi->Packet(pak, head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}

	int IServiceObj::WriteEx(const sk::network::EnNetCmd& cmd, const sk::packet& pak, const sk::network::Header::ExternData& externData,const SKAPIRES& retcode) const
	{
		if (!m_pContext || !Enable()) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		head.DataExtern = externData;
		head.Retcode = static_cast<decltype(head.Retcode)>(retcode);
		m_pNetApi->Packet(pak, head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}

	int IServiceObj::WriteEx(const sk::network::EnNetCmd& cmd, const sk::packet& pak, const sk::network::HEADER::EnDataProcessFlag& flag, const sk::network::Header::ExternData& exten) const
	{
		if (!m_pContext || !Enable()) return -1;
		sk::packet enpak;
		sk::network::Header head;
		head.Cmd = (cmd);
		head.DataExtern = exten;
		head.DataProcessFlag = flag;
		m_pNetApi->Packet(pak, head, enpak);
		m_pContext->Write(enpak);
		return 0;
	}

	int IServiceObj::SendLoginResult(const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_100000070, 
			"",
			sk::network::Header::ExternData(),
			retcode);
	}

	int IServiceObj::SendShutdown() const 
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000030, "");
	}

	int IServiceObj::SendShowWindow(const bool& show) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000070,
			"",
			sk::network::Header::ExternData(0, 0, show ? 1 : 0),
			SKAPIRES(0));
	}

	int IServiceObj::SendSystemTime() const 
	{
		auto currTime = sk::Helper::TickCountGet<std::chrono::seconds>();
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000010, sk::packet((char*)&currTime, sizeof(currTime)));
	}

	int IServiceObj::SendBridgeSystemSetup(const shared::BridgeSystemSetup& setup) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000020, sk::packet((char*)&setup, shared::LENBRIDGESYSTEMSETUP));
	}

	int IServiceObj::SendMarketDataUpdate(const sk::packet& quotes) const 
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000310, quotes);
	}

	int IServiceObj::ProductsRequest() const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000110, "");
	}

	int IServiceObj::SendMarketDataCache(const sk::packet& mds) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000320, mds);
	}
	int IServiceObj::SendMTUsers(const sk::network::HEADER::EnDataProcessFlag& flag, const sk::packet& users) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000510, users, flag, sk::network::Header::ExternData());
	}
	int IServiceObj::SendMTSymbols(const sk::packet& symbols) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000520, symbols);
	}
	int IServiceObj::SendSystemAssignLoginsPostOrder() const
	{
		Global::GetSetup()->m_SystemAssignLoginsPostOrderQ.iterate(
			[&](const auto& exchange,auto& logins) 
			{
				if (!logins.empty())
				{
					sk::packet logs;
					logins.iterate([&](auto& login) {logs.append((char*)&login,sizeof(UINT64)); });
					if (!logs.empty())
					{
						WriteEx(
							sk::network::EnNetCmd::EN_NETCMD_300000060,
							logs,
							sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT,
							sk::network::HEADER::ExternData(0, exchange, 0));
					}
				}
			});
		return 0;
	}
	int IServiceObj::SendSystemAssignLoginsPostOrder(const shared::SystemID& ExchangeID, const sk::network::HEADER::EnDataProcessFlag& flag, const sk::packet& logins) const
	{
		return WriteEx(
			sk::network::EnNetCmd::EN_NETCMD_300000060,
			logins,
			flag,
			sk::network::HEADER::ExternData(0, ExchangeID, 0));
	}















}///namespace proxy