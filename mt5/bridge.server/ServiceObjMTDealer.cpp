#include "stdafx.h"
#include "ServiceObjMTDealer.h"

namespace proxy {

	MTDealerObj::MTDealerObj(sk::network::INetworkApi* pNetworkApi, sk::network::INetworkContext* pNetworkContext) :
		IServiceObj(pNetworkApi, pNetworkContext) 
	{
		m_ServiceObjFlag = shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_MTDEALER;
	}

	MTDealerObj::~MTDealerObj() 
	{

	}

	int MTDealerObj::SendProducts(const std::vector<shared::MTGatewayProduct>& products) const
	{
		sk::packet send;
		for (const auto& prod : products)
		{
			send.append((char*)&prod, shared::LENMTGATEWAYPRODUCT);
		}
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000120, send);
	}
	int MTDealerObj::TradeOrderResSend(const shared::SystemOrder& result) const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000410, sk::packet((char*)&result, shared::LENSYSTEMORDER));
	}

	int MTDealerObj::RequestMTUserGet() const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000510, "");
	}

	int MTDealerObj::RequestMTSymbolGet() const
	{
		return Write(sk::network::EnNetCmd::EN_NETCMD_300000520, "");
	}

	int MTDealerObj::RequestSystemPositionGet(const sk::network::HEADER::ExternData& externData) const
	{
		return WriteEx(sk::network::EnNetCmd::EN_NETCMD_300000530, "", sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, externData);
	}

}///namespace proxy