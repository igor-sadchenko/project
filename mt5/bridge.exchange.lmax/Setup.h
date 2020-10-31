#if !defined(AFX_SKSTU_SETUP_H__DEEDE_22474531_4CA02AEDB749FD3_D725__HEAD__)
#define AFX_SKSTU_SETUP_H__DEEDE_22474531_4CA02AEDB749FD3_D725__HEAD__

namespace lmax {

	class Setup final {
	public:
		Setup(const sk::stringa& objPath);
		~Setup();
	private:
		int Load();
		int LoadProducts();
	public:
		sk::stringa ApiPath() const { return m_ApiPath; }
		sk::stringa ProductSrcPathname() const { return m_ProductSrcPathname; }
		const shared::BridgeSystemSetup& BridgeSystemSetup();
		void BridgeSystemSetup(const shared::BridgeSystemSetup&);
		void BridgeSystemTime(const std::int64_t& time) { m_BridgeSystemTime.store(time); }
		const std::int64_t& BridgeSystemTime() const { return m_BridgeSystemTime.load(); }

		bool EnableTradeOrder() const { return m_EnableTradeOrder; }
		sk::stringa TradeOrderFixcon() const { return m_TradeOrderFixcon; }
		sk::stringa TradeOrderCachePathname() const { return m_TradeOrderCachePathname; }
		sk::stringa TradeOrderTcptoAddress() const { return m_TradeOrderTcptoAddress; }
		const shared::BridgeLoginField* LoginFieldTradeOrder() const { return &m_LoginFieldTradeOrder; }

		bool EnableMarketData() const { return m_EnableMarketData; }
		sk::stringa MarketDataFixcon() const { return m_MarketDataFixcon; }
		sk::stringa MarketDataCachePathname() const { return m_MarketDataCachePathname; }
		sk::stringa MarketDataTcptoAddress() const { return m_MarketDataTcptoAddress; }
		const shared::BridgeLoginField* LoginFieldMarketData() const { return &m_LoginFieldMarketData; }

		sk::container::map<shared::SystemID, shared::LPProduct> m_ProductQ;
	private:
		sk::stringa m_objPath;
		sk::stringa m_ApiPath;
		sk::stringa m_ProductSrcPathname;
		shared::BridgeSystemSetup m_BridgeSystemSetup;
		std::atomic_int64_t m_BridgeSystemTime = 0;

		sk::stringa m_MarketDataFixcon;
		sk::stringa m_TradeOrderFixcon;
		sk::stringa m_MarketDataCachePathname;
		sk::stringa m_TradeOrderCachePathname;
		bool m_EnableMarketData = false;
		bool m_EnableTradeOrder = false;
		sk::stringa m_MarketDataTcptoAddress;
		sk::stringa m_TradeOrderTcptoAddress;
		shared::BridgeLoginField m_LoginFieldMarketData;
		shared::BridgeLoginField m_LoginFieldTradeOrder;
	private:
		std::mutex m_mutex;
	};



}///namespace lmax




/// /*新生联创（上海）**/
/// /*2019年12月8日 10:52:56|568**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_SETUP_H__DEEDE_22474531_4CA02AEDB749FD3_D725__TAIL__