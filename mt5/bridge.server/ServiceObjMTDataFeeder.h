#if !defined(AFX_SK_H__AC07AA80_D485_4C18_9B8D_377D1442ED96__HEAD__)
#define AFX_SK_H__AC07AA80_D485_4C18_9B8D_377D1442ED96__HEAD__

namespace proxy {
	class MTDataFeedObj final : public IServiceObj 
	{
	public:
		MTDataFeedObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~MTDataFeedObj();
	public:
		void Release() override final { delete this; }
		int SendMarketDataUpdate(const sk::packet&) const override final;
		int SendProducts(const std::vector<shared::MTFeederProduct>&) const;
	};
}///namespace proxy

//!@ /*新生联创®（上海）*/
//!@ /*Thu Aug 27 17:51:23 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__AC07AA80_D485_4C18_9B8D_377D1442ED96__HEAD__*/
