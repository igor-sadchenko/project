#if !defined(AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__HEAD__)
#define AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__HEAD__

namespace proxy {
	class Shared
	{
		friend class SymbolCon;
	public:
		Shared();
		~Shared();
	public:
		//!@ ����ʹ��
		sk::container::map<INT64, shared::SystemOrder> m_TestOrderRequestQ;

		//!@ ���׼�¼
		sk::container::queue<sk::packet> m_RecordQ;
		//!˵�� : MTUser
		//!ע�� : 
		//!���� : Wed Jun 17 14:31:34 UTC+0800 2020
		sk::container::map<UINT64, shared::UserField> m_MTUserQ;
		//!˵�� : MTSymbol
		//!ע�� : 
		//!���� : Sun Aug 30 01:41:29 UTC+0800 2020
		sk::container::map<sk::stringw, shared::ConSymbolField> m_MTSymbolQ;
		//! ���񱨼۶���(�ۺϺ�ı���)
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateFormTaskQ;
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateQ;
		sk::container::map<shared::SystemID/*FullID*/, shared::marketdata::MarketData> m_MarketDataQ;

		//! ---------------------------------Ԥ��ʼ�����ݻ���---------------------------------
		//! Feeder Symbols
		sk::container::map<shared::SystemID/*MTGatewayID*/, sk::container::map<sk::stringa,shared::ConSymbolField/*MTSymobl*/>> m_FeederSymbolQ;
		//! Gateway Symbols
		sk::container::map<shared::SystemID/*MTGatewayID*/, sk::container::map<sk::stringa,shared::ConSymbolField/*MTSymobl*/>> m_GatewaySymbolQ;
		//! LP Products
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<shared::LPProduct>> m_LPProductQ;


		//! Exchange Products
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::ExchangeProduct>> m_ExchangeProductQ;
		//! MTDataFeed Products
		sk::container::map<shared::SystemID/*DataFeedID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::MTFeederProduct>> m_MTDataFeedProductsQ;
		//! MTGateway Products
		sk::container::map<shared::SystemID/*GatewayID*/, sk::container::map<shared::SystemID/*ProductID*/, shared::MTGatewayProduct>> m_MTGatewayProductsQ;
	public:
		//! ---------------------------------����ʱ�ۺϱ��ۻ���---------------------------------
		sk::container::map<shared::SystemID/*��ϵͳID*/, shared::marketdata::MarketData> m_MarketDataFinalQ;
		//! ---------------------------------����ʱ���ݻ���---------------------------------
		//! ��ϵͳSymbol At ID
		sk::container::map<shared::Instrument/*��ϵͳSymbol*/, shared::SystemID/*��ϵͳID*/> m_SystemProductIDQ;
		//! ��ϵͳƷ��
		sk::container::map<shared::SystemID/*��ϵͳID*/, shared::SystemProduct> m_SystemProductFinalQ;
		//! Exchange(LP)Ʒ��ӳ��
		sk::container::map<shared::SystemID/*��ϵͳID*/, sk::container::set<shared::LPProduct>> m_LPProductFinalQ;
		//! FeederƷ��ӳ��
		sk::container::map<shared::SystemID/*��ϵͳID*/, sk::container::set<shared::MTFeederProduct>> m_FeederProductFinalQ;
		//! GatewayƷ��ӳ��
		sk::container::map<shared::SystemID/*��ϵͳID*/, sk::container::set<shared::MTGatewayProduct>> m_GatewayProductFinalQ;

		//! Symbolӳ���ϵ
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTGatewayProduct> m_MTSymbolProdconQ;
	public:
		shared::Serial Serial() { return ::_InterlockedExchangeAdd64(&m_Serial, 1ll); }
	private:
		//!ǩ�� : m_OrderSerial
		//!˵�� : ������ģ�鶨�����к�
		//!ע�� : �����ģ����������ڱ���һ��
		//!���� : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_Serial = 1ll;
	};



}///namespace proxy




/// /*�����������Ϻ���**/
/// /*2019��11��18�� 8:48:17|365**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__D8CCC_96B3D1F0_D7D86A320C9B079_3384__TAIL__