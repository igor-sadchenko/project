#include "stdafx.h"
#include "ServiceObjLPMarketData.h"


namespace proxy {

	LPMarketDataObj::LPMarketDataObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext) 
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_LPMARKETDATA;
	}

	LPMarketDataObj::~LPMarketDataObj() 
	{

	}

	int LPMarketDataObj::MarketDataCacheRequest() const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000320, "");
	}

	int LPMarketDataObj::SendProducts(const std::vector<shared::LPProduct>& products) const
	{
		sk::packet send;
		for (const auto& prod : products)
		{
			send.append((char*)&prod, shared::LENLPPRODUCT);
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000120, send);
	}


}///namespace proxy