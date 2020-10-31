#if !defined(AFX_SK_H__957E9764_0283_45A2_888C_CCF9FFA6387A__HEAD__)
#define AFX_SK_H__957E9764_0283_45A2_888C_CCF9FFA6387A__HEAD__

namespace proxy {

	class AdminObj final : public IServiceObj {
	public:
		AdminObj(sk::network::INetworkApi*, sk::network::INetworkContext*);
		~AdminObj();
	public:
		void Release() override final { delete this; }
		int SendMarketDataUpdate(const sk::packet&) const override final;
		int SendExchangePositions(const sk::packet& ,const shared::SystemID&, const sk::network::HEADER::EnDataProcessFlag&) const;
		int SendProductCon(const sk::packet&, const shared::SystemID&, const INT&, const SKAPIRES&) const;
		int SendTradeRecordHead(const sk::packet&) const;
		int SendSystemPositions(const sk::network::HEADER::ExternData&, const sk::packet&) const;
		int SendTradeRecordUpdate(const sk::packet&) const;
		int SendProducts(const std::vector<shared::SystemProduct>&) const;
		int SendExchangeOnline(const sk::packet&) const;
	};

}///namespace proxy

//!@ /*新生联创®（上海）*/
//!@ /*Thu Aug 27 17:56:49 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__957E9764_0283_45A2_888C_CCF9FFA6387A__HEAD__*/