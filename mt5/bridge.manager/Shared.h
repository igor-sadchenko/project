#if !defined(AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__HEAD__)
#define AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__HEAD__

namespace client 
{
	class Shared {
	public:
		Shared() 
		{
			m_TradeRecordQ.SetMaxSize(100);
		}
		~Shared() {}
	public:
#if 0
		sk::container::map<shared::SymbolA, sk::container::set<shared::ProductLP>> m_SystemProductTreeQ;
		sk::container::map<shared::MarketType/*SysProductID*/, shared::ProductSYS> m_ProductQ;//! ���в�Ʒ/δ�ۺϵĲ�Ʒ
		sk::container::map<shared::MarketType/*ȫ·��*/, shared::MarketData> m_ProductMarketDataQ;//! ���в�Ʒ������ӳ��
#endif
		sk::container::map<UINT64, shared::UserSimpleField> m_MTUserSimpleQ;
		sk::container::map<shared::SystemID/*ϵͳ��ƷID*/, shared::SystemProduct> m_SystemProductQ;
		sk::container::map<shared::SystemID/*ϵͳ��ƷID*/, sk::stringa/*ϵͳ��Ʒ���û���*/> m_SystemSymbolConQ;
		sk::container::queue<shared::marketdata::MarketData> m_MarketDataUpdateQ;
		sk::container::map<shared::SystemID/*ȫ·��*/, shared::marketdata::MarketData> m_MarketDataQ;

		sk::container::multimap<time_t, shared::BridgeTradeRecord> m_TradeRecordQ;

		sk::container::map<shared::SystemID, shared::BridgeLoginField> m_ExchangeOnlineQ;
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::map<shared::SystemID/*ExchangeProductID*/, shared::tradeorder::ExchangePosition>> m_ExchangePositionQ;
		sk::container::map<shared::SystemID/*ExchangeID*/,sk::container::map<shared::SystemID/*SystemPID*/, shared::SimplePositionField>> m_SystemPositionQ;
		shared::NetSync m_NetSync;

		//!@ ָ���ʻ��׵��������
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<UINT64/*MT5Login*/>> m_SystemAssignLoginsPostOrderQ;
	};



}





/// /*�����������Ϻ���**/
/// /*2019��12��30�� 11:51:48|304**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Shared_H__71054_EF40B005_502F65870A2A6B4_2685__TAIL__