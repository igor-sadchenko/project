#include "stdafx.h"
#include "ServiceObjMTDataFeeder.h"

namespace proxy {

	MTDataFeedObj::MTDataFeedObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext) 
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDATAFEED;
	}

	MTDataFeedObj::~MTDataFeedObj() 
	{

	}

	int MTDataFeedObj::SendProducts(const std::vector<shared::MTFeederProduct>& products) const
	{
		sk::packet send;
		for (const auto& prod : products)
		{
			send.append((char*)&prod, shared::LENMTFEEDERPRODUCT);
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000120, send);
	}

	int MTDataFeedObj::SendMarketDataUpdate(const sk::packet& mds) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000310, mds);
	}

}///namespace proxy