#include "stdafx.h"
#include "ServiceObjAdmin.h"

namespace proxy {

	AdminObj::AdminObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext) 
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN;
	}

	AdminObj::~AdminObj()
	{

	}

	int AdminObj::SendProducts(const std::vector<shared::SystemProduct>& products) const
	{
		sk::packet send;
		for (const auto& prod : products)
		{
			send.append((char*)&prod, shared::LENSYSTEMPRODUCT);
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000120, send);
	}

	int AdminObj::SendProductCon(
		const sk::packet& con,
		const shared::SystemID& id,
		const INT& Total,
		const SKAPIRES& retcode) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000210, con, sk::network::Header::ExternData(0, id, Total), retcode);
	}

	int AdminObj::SendTradeRecordHead(const sk::packet& headStr) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000451, headStr);
	}

	int AdminObj::SendTradeRecordUpdate(const sk::packet& logs) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000452, logs);
	}

	int AdminObj::SendMarketDataUpdate(const sk::packet& mds) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000310, mds);
	}
	int AdminObj::SendExchangePositions(const sk::packet& position,const shared::SystemID& exchange, const sk::network::HEADER::EnDataProcessFlag& flag) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000420, position, flag,sk::network::Header::ExternData(0, exchange, 0));
	}
	int AdminObj::SendExchangeOnline(const sk::packet& exchanges) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000040, exchanges);
	}
	int AdminObj::SendSystemPositions(const sk::network::HEADER::ExternData& externData, const sk::packet& pak) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000530, pak, sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, externData);
	}
}///namespace proxy