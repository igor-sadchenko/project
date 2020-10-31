#include "stdafx.h"
#include "ServiceObj.h"

namespace proxy {

	LPTradeOrderObj::LPTradeOrderObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext) 
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPTRADEORDER;
	}

	LPTradeOrderObj::~LPTradeOrderObj() 
	{

	}

	int LPTradeOrderObj::TradeOrderRequest(const shared::SystemOrder& order) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000410, sk::packet((char*)&order, shared::LENSYSTEMORDER));
	}
	int LPTradeOrderObj::TradeOrderTimeoutRequest(const shared::SystemOrder& order) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000415, sk::packet((char*)&order, shared::LENSYSTEMORDER));
	}
	int LPTradeOrderObj::SendProducts(const std::vector<shared::LPProduct>& products) const
	{
		sk::packet send;
		for (const auto& prod : products)
		{
			send.append((char*)&prod, shared::LENLPPRODUCT);
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000120, send);
	}

	int LPTradeOrderObj::PositionRequest() const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000420, "");
	}
}///namespace proxy