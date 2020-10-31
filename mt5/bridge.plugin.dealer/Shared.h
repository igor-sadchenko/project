#if !defined(AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__)
#define AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__


namespace dealer {
	class Shared final {
	public:
		Shared() {}
		~Shared() {}
	public:
		//!@ ָ���ʻ��׵��������
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<UINT64/*MT5Login*/>> m_SystemAssignLoginsPostOrderQ;


		sk::container::map<UINT64, shared::UserField> m_MTUserQ;
		sk::container::map<sk::stringw, shared::ConGroupField> m_MTConGroupQ;
		sk::container::map<sk::stringw, shared::ConSymbolField> m_OnConSymbolFieldQ;
		sk::container::set<sk::stringw> m_GatewaySymbolQ;//! ����Symbols
		sk::container::map<sk::stringw, shared::ConSymbolField> m_MTSymbolQ;//! MT Symbol
		sk::container::map<UINT64, shared::PositionField> m_MTPositionQ;

		//! Symbolӳ���ϵ
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTGatewayProduct> m_MTSymbolProdconQ;
		sk::container::map<shared::SystemID/*��ƷID*/, shared::Instrument/*MTSymbol*/> m_ProdIDMTSymbolQ;
		sk::container::map<shared::SystemID/*��ƷID*/, shared::MTGatewayProduct> m_ProdIDProdconQ;

		sk::container::map<shared::SystemID/*��ϵͳ��ƷID*/, sk::container::set<shared::MTGatewayProduct>> m_SymbolMappingQ;

		//!@ OrderRequest Cache
		sk::container::queue<shared::SystemOrder> m_OrderReqQ;
		//!@ OrderRequestResult Cache
		sk::container::queue<shared::SystemOrder> m_OrderResQ;
		//!@ �����ٴ�����ļ�¼����
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_RerequestCacheQ;



		//!@ Gateway Order Task Route Cache
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_GatewayOrderTaskRouteCacheQ;
		//!@ ��ʱ�������񻺴�
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_GatewayOrderTaskRouteTimeoutCacheQ;
		//!ǩ�� : m_GatewayOrderOpenQ
		//!˵�� : �����������
		//!ע�� : 
		//!���� : Thu Jun 11 19:16:05 UTC+0800 2020
		sk::container::map<shared::Serial, std::shared_ptr<DealerOrder>> m_DealerOrderQ;
		sk::container::map<shared::Serial, std::shared_ptr<DealerOrder>> m_DelayDealerOrderQ;
		sk::container::queue<shared::Serial> m_DealerOrderDispatchQ;
	public:
		shared::Serial OrderSerial() { return ::_InterlockedExchangeAdd64(&m_OrderSerial, 1ll); }
	private:
		//!ǩ�� : m_OrderSerial
		//!˵�� : ������ģ�鶨�����к�
		//!ע�� : �����ģ����������ڱ���һ��
		//!���� : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_OrderSerial = 1ll;
	};





}///namespace dealer




/// /*�����������Ϻ���**/
/// /*2020��01��17�� 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__