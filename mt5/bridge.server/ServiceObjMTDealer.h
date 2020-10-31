#if !defined(AFX_SK_H__D8A9F2B8_CCD7_4636_91F1_F865843E27E6__HEAD__)
#define AFX_SK_H__D8A9F2B8_CCD7_4636_91F1_F865843E27E6__HEAD__

namespace proxy {

	class MTDealerObj final : public IServiceObj {
	public:
		MTDealerObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~MTDealerObj();
	public:
		void Release() override final { delete this; }
		int RequestMTUserGet() const;
		int RequestMTSymbolGet() const;
		int RequestSystemPositionGet(const sk::network::HEADER::ExternData&) const;
		int TradeOrderResSend(const shared::SystemOrder&) const;
		int SendProducts(const std::vector<shared::MTGatewayProduct>&) const;
	};


}///namespace proxy

//!@ /*新生联创®（上海）*/
//!@ /*Thu Aug 27 17:51:31 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__D8A9F2B8_CCD7_4636_91F1_F865843E27E6__HEAD__*/
