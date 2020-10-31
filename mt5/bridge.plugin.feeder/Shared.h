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
		//! Symbolӳ���ϵ
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTFeederProduct> m_MTSymbolProdconQ;
		sk::container::map<shared::SystemID/*��ƷID*/, shared::Instrument/*MTSymbol*/> m_ProdIDMTSymbolQ;
		sk::container::map<shared::SystemID/*��ƷID*/, shared::MTFeederProduct> m_ProdIDProdconQ;

		sk::container::map<shared::SystemID/*��ϵͳ��ƷID*/, sk::container::set<shared::MTFeederProduct>> m_SymbolMappingQ;
	public:
		shared::Serial OrderSerial() { return ::_InterlockedExchangeAdd64(&m_OrderSerial, 1ll); }
	private:
		//!ǩ�� : m_OrderSerial
		//!˵�� : ������ģ�鶨�����к�
		//!ע�� : �����ģ����������ڱ���һ��
		//!���� : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_OrderSerial = 1ll;
	};





}///namespace feeder




/// /*�����������Ϻ���**/
/// /*2020��01��17�� 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__