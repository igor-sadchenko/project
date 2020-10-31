#if !defined(AFX_SK_H__05A4BD90_7306_44A3_AC8C_9FF781ED4890__HEAD__)
#define AFX_SK_H__05A4BD90_7306_44A3_AC8C_9FF781ED4890__HEAD__

namespace proxy {

	class LPTradeOrderObj final : public IServiceObj 
	{
	public:
		LPTradeOrderObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~LPTradeOrderObj();
	public:
		int TradeOrderRequest(const shared::SystemOrder&) const;
		int TradeOrderTimeoutRequest(const shared::SystemOrder&) const;
		int SendProducts(const std::vector<shared::LPProduct>&) const;
		int PositionRequest() const;
	public:
		void Release() override final { delete this; }
	public:
		sk::container::map<shared::SystemID/*LPProductID*/, shared::LPProduct> m_ProductQ;
		sk::container::map<shared::SystemID/*LPProductID*/, shared::tradeorder::ExchangePosition> m_PositionQ;
	};



}///namespace proxy




//!@ /*新生联创®（上海）*/
//!@ /*Thu Aug 27 17:38:59 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__05A4BD90_7306_44A3_AC8C_9FF781ED4890__HEAD__*/