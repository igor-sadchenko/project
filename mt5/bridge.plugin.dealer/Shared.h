#if !defined(AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__)
#define AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__HEAD__


namespace dealer {
	class Shared final {
	public:
		Shared() {}
		~Shared() {}
	public:
		//!@ 指定帐户抛单缓存队列
		sk::container::map<shared::SystemID/*ExchangeID*/, sk::container::set<UINT64/*MT5Login*/>> m_SystemAssignLoginsPostOrderQ;


		sk::container::map<UINT64, shared::UserField> m_MTUserQ;
		sk::container::map<sk::stringw, shared::ConGroupField> m_MTConGroupQ;
		sk::container::map<sk::stringw, shared::ConSymbolField> m_OnConSymbolFieldQ;
		sk::container::set<sk::stringw> m_GatewaySymbolQ;//! 网关Symbols
		sk::container::map<sk::stringw, shared::ConSymbolField> m_MTSymbolQ;//! MT Symbol
		sk::container::map<UINT64, shared::PositionField> m_MTPositionQ;

		//! Symbol映射关系
		sk::container::map<shared::Instrument/*MTSymbol*/, shared::MTGatewayProduct> m_MTSymbolProdconQ;
		sk::container::map<shared::SystemID/*产品ID*/, shared::Instrument/*MTSymbol*/> m_ProdIDMTSymbolQ;
		sk::container::map<shared::SystemID/*产品ID*/, shared::MTGatewayProduct> m_ProdIDProdconQ;

		sk::container::map<shared::SystemID/*桥系统产品ID*/, sk::container::set<shared::MTGatewayProduct>> m_SymbolMappingQ;

		//!@ OrderRequest Cache
		sk::container::queue<shared::SystemOrder> m_OrderReqQ;
		//!@ OrderRequestResult Cache
		sk::container::queue<shared::SystemOrder> m_OrderResQ;
		//!@ 触发再次请求的记录缓存
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_RerequestCacheQ;



		//!@ Gateway Order Task Route Cache
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_GatewayOrderTaskRouteCacheQ;
		//!@ 超时定单任务缓存
		sk::container::map<UINT/*MTRequestID*/, shared::TradeTaskRouteGateway> m_GatewayOrderTaskRouteTimeoutCacheQ;
		//!签名 : m_GatewayOrderOpenQ
		//!说明 : 交易任务队列
		//!注意 : 
		//!日期 : Thu Jun 11 19:16:05 UTC+0800 2020
		sk::container::map<shared::Serial, std::shared_ptr<DealerOrder>> m_DealerOrderQ;
		sk::container::map<shared::Serial, std::shared_ptr<DealerOrder>> m_DelayDealerOrderQ;
		sk::container::queue<shared::Serial> m_DealerOrderDispatchQ;
	public:
		shared::Serial OrderSerial() { return ::_InterlockedExchangeAdd64(&m_OrderSerial, 1ll); }
	private:
		//!签名 : m_OrderSerial
		//!说明 : 本程序模块定单序列号
		//!注意 : 与程序模块的生命周期保存一致
		//!日期 : Fri Jun 12 01:05:41 UTC+0800 2020
		volatile shared::Serial m_OrderSerial = 1ll;
	};





}///namespace dealer




/// /*新生联创（上海）**/
/// /*2020年01月17日 11:33:26|995**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SHARED_H__347E0_11F5B0C6_B6CA8AFC668080F_086A__TAIL__