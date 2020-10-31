#if !defined(AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__)
#define AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__


namespace feeder {
	class Shared final {
	public:
		Shared() {}
		~Shared() {}
	public:
		sk::container::set<sk::stringw> m_GatewaySymbolQ;
		sk::container::set<sk::stringw> m_BridgeSymbolQ;
		sk::container::map<sk::stringw, shared::ConSymbolField> m_MTSymbolQ;
		sk::container::map<sk::stringw, shared::ConSymbolField> m_OnConSymbolFieldQ;
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateQ;
		sk::container::map<shared::SystemID, shared::marketdata::MarketData> m_MarketDataQ;
		//! Symbol映射关系
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTFeederProduct> m_MTSymbolProdconQ;
		sk::container::map<shared::SystemID/*产品ID*/, shared::Instrument/*MTSymbol*/> m_ProdIDMTSymbolQ;
		sk::container::map<shared::SystemID/*产品ID*/, shared::MTFeederProduct> m_ProdIDProdconQ;

		sk::container::map<shared::SystemID/*桥系统产品ID*/, sk::container::set<shared::MTFeederProduct>> m_SymbolMappingQ;
	public:
		shared::Serial OrderSerial() { return ::_InterlockedExchangeAdd64(&m_OrderSerial, 1ll); }
	private:
		//!签名 : m_OrderSerial
		//!说明 : 本程序模块定单序列号
		//!注意 : 与程序模块的生命周期保存一致
		//!日期 : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_OrderSerial = 1ll;
	};





}///namespace feeder




/// /*新生联创（上海）**/
/// /*2020年01月17日 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__