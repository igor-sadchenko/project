#if !defined(AFX_SKSTU_MARKETDATA_H__804BE_39A1A592_02A57754995549E_218D__HEAD__)
#define AFX_SKSTU_MARKETDATA_H__804BE_39A1A592_02A57754995549E_218D__HEAD__

namespace cfh {

	class MarketData final :
		public sk::object::IObject,
		public shared::ILiquidityProviderMarketData {
		friend class Core;
	public:
		MarketData();
		~MarketData();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		void ProcessT() override final;
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;
	protected:
		void OnFixLogon() override final;
		void OnFixLogout() override final;
		void OnFixResponse(const libfix::EnFixMessageType&, const sk::intptr&) override final;
		void OnNetLoginResult(const sk::network::Header&, const sk::packet&) override final;
		void OnNetShutdown(const sk::network::Header&, const sk::packet&) override final;
		void OnNetShowWindow(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemSetup(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemTime(const sk::network::Header&, const sk::packet&) override final;
		void OnNetProductsRequest(const sk::network::Header&, const sk::packet&) override final;
		void OnNetMDCacheReqeust(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemProducts(const sk::network::Header&, const sk::packet&) override final;
	private:
		int FixMarketDataRequest() override final;
		void FixReject(const libfix::_3__*) override final;
		void FixMarketDataRequestReject(const libfix::_Y__*) override final;
		void FixMarketDataSnapshotFullRefresh(const libfix::_W__*) override final;
	private:
		sk::container::queue<shared::marketdata::MarketData> m_MDUpdateQ;
		shared::Memmap<shared::SystemID, shared::marketdata::MarketData>* m_pMDCacheQ = nullptr;
		sk::container::map<shared::marketdata::MDReqID, shared::SystemID> m_MDRequestCacheQ;
	public:
		//shared::Memmap<shared::SystemID, shared::marketdata::MarketData>* MDCacheGet() const { return m_pMDCacheQ; }
	};

}///namespace cfh




/// /*新生联创（上海）**/
/// /*2020年05月6日 20:41:11|428**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_MARKETDATA_H__804BE_39A1A592_02A57754995549E_218D__TAIL__
