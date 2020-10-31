#if !defined(AFX_SK_H__A9636E1E_7A26_4632_8B11_4D31A56717B0__HEAD__)
#define AFX_SK_H__A9636E1E_7A26_4632_8B11_4D31A56717B0__HEAD__

namespace proxy {

	class LPMarketDataObj final : public IServiceObj {
	public:
		LPMarketDataObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~LPMarketDataObj();
	public:
		int MarketDataCacheRequest() const;
		int SendProducts(const std::vector<shared::LPProduct>&) const;
	public:
		void Release() override final { delete this; }
	public:
		sk::container::map<shared::SystemID/*LPProductID*/, shared::LPProduct> m_ProductQ;
		sk::container::map<shared::SystemID/*LPProductID*/, shared::marketdata::MarketData> m_MarketDataQ;
		time_t MDUpdateTimestamp() const { return m_MDUpdateTimestamp.load(); }
		void MDUpdateTimestamp(const time_t& at) { m_MDUpdateTimestamp.store(at); }
	private:
		std::atomic<time_t> m_MDUpdateTimestamp = 0;
	};

}///namespace proxy

//!@ /*新生联创®（上海）*/
//!@ /*Thu Aug 27 17:47:07 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__A9636E1E_7A26_4632_8B11_4D31A56717B0__HEAD__*/