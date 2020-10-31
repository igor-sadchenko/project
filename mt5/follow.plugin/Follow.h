#if !defined(AFX_SKSTU_PLUGIN_H__FB34F_6CD490C1_8A81D52DC1A913E_0220__HEAD__)
#define AFX_SKSTU_PLUGIN_H__FB34F_6CD490C1_8A81D52DC1A913E_0220__HEAD__

namespace follow {
	class Follow :
		public IMTDealSink,
		public IMTTradeSink,
		public IMTOrderSink,
		public IMTPositionSink,
		public IMTConTimeSink,
		public IMTUserSink,
		public IMTConSymbolSink,
		public IMTConGroupSink,
		public IMTServerPlugin {

	public:
		Follow();
		~Follow();
		void Release() override final;
		MTAPIRES Start(IMTServerAPI* server) override final;
		MTAPIRES Stop() override final;
	private:
		MTServerInfo m_ServerInfo;
		IMTServerAPI* m_pServerApi = nullptr;
		FollowObj* m_pFollowObj = nullptr;
		std::atomic_int64_t m_ServerTime = 0;
		std::atomic_int64_t m_ServerTimeDiff = 0;
		std::shared_ptr<std::mutex> m_mutex;
		sk::stringa m_PathObj;

		Server* m_pServer = nullptr;
		Setup* m_pSetup = nullptr;
		Shared* m_pShared = nullptr;
		shared::SharedApi* m_pSKApi = nullptr;
		sk::Log* m_pLog = nullptr;
		sk::stringa m_LogKey;
#pragma region{business}
	public:
		std::atomic_bool m_PluginReady = false;
		static Server* ServerGet();
		static Setup* SetupGet();
		static Shared* SharedGet();
		static shared::SharedApi* SharedApiGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);

		std::int64_t ServerTime() const { return m_ServerTime.load(); }
		std::int64_t ServerTimeDiff() const { return m_ServerTimeDiff.load(); }
		IMTServerAPI* ServerApi() const { return m_pServerApi; }
		FollowObj* FollowObjGet() const { return m_pFollowObj; }

		MTAPIRES TradeRequest(
			__in shared::RequestField&);
		MTAPIRES DealGet(
			__in const UINT64&,
			__out shared::DealField&);
		MTAPIRES PositionGetByTicket(
			__in const UINT64&,
			__out shared::PositionField&);
		MTAPIRES DealApiDataAt(
			__in const UINT64& ticket,
			__in const std::function<MTAPIRES(IMTDeal*)>&);
		MTAPIRES PositionApiDataAt(
			__in const UINT64& ticket,
			__in const std::function<MTAPIRES(IMTPosition*)>&);
		MTAPIRES PositionApiDataAt(
			__in const UINT64& login,
			__in const wchar_t* symbol,
			__in const std::function<MTAPIRES(IMTPosition*)>&);
#pragma endregion{business}

#pragma region{function}
	public:
		static UINT64 ApiDataGet(
			__in const IMTPosition*,
			__in const shared::EnMTApiParamType&);
		static MTAPIRES ApiDataSet(
			__in IMTPosition*,
			__in const shared::EnMTApiParamType&,
			__in const UINT64&);
		static UINT64 ApiDataGet(
			__in const IMTDeal*,
			__in const shared::EnMTApiParamType&);
		static MTAPIRES ApiDataSet(
			__in IMTDeal*,
			__in const shared::EnMTApiParamType&,
			__in const UINT64&);
#pragma endregion{function}












	private:
		int Synchronize();
		int SyncServerTime();
		int SyncUsers();
		int SyncGroups();
		int SyncSymbols();
		int SyncPositions();
	private:
		void OnDealAdd(const IMTDeal* deal) override final;
		void OnDealUpdate(const IMTDeal* deal) override final;
		void OnDealDelete(const IMTDeal* deal) override final;
		void OnDealPerform(const IMTDeal* deal, IMTAccount* account, IMTPosition* position) override final;

		void OnTradeRequestAdd(const IMTRequest* request, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order)override final;
		void OnTradeRequestUpdate(const IMTRequest* request)  override final;
		void OnTradeRequestDelete(const IMTRequest* request)  override final;
		void OnTradeRequestRefuse(const IMTRequest* request) override final;
		void OnTradeRequestProcess(const IMTRequest* request, const IMTConfirm* confirm, const IMTConGroup* group, const IMTConSymbol* symbol, const IMTPosition* position, const IMTOrder* order, const IMTDeal* deal)override final;

		void OnOrderAdd(const IMTOrder* order) override final;
		void OnOrderUpdate(const IMTOrder* order) override final;
		void OnOrderDelete(const IMTOrder* order) override final;

		void OnPositionAdd(const IMTPosition* position) override final;
		void OnPositionUpdate(const IMTPosition* position) override final;
		void OnPositionDelete(const IMTPosition* position) override final;

		void OnUserAdd(const IMTUser* user) override final;
		void OnUserUpdate(const IMTUser* user) override final;
		void OnUserDelete(const IMTUser* user) override final;

		void OnGroupAdd(const IMTConGroup* config) override final;
		void OnGroupUpdate(const IMTConGroup* config) override final;
		void OnGroupDelete(const IMTConGroup* config) override final;

		void OnSymbolAdd(const IMTConSymbol* config) override final;
		void OnSymbolUpdate(const IMTConSymbol* config) override final;
		void OnSymbolDelete(const IMTConSymbol* config) override final;

		void OnTimeUpdate(const IMTConTime* config) override final;
	private:
		sk::LicenseInfo m_LicenseInfo;
		long long m_LicenseVersionType = 0;
		long long LicenseGet() { return InterlockedExchangeAdd64(&m_LicenseVersionType,0); }
		void LicenseSet(const sk::LicenseInfo::EnProductVersionType& at) { InterlockedExchange64(&m_LicenseVersionType,long long(at)); }
	public:
		int License(__int64 time = 0);


	};

	extern HINSTANCE global_hInstance;
	Follow* FollowGet();
	extern Follow* static_global_Follow;

}///namespace follow




/// /*新生联创（上海）**/
/// /*2019年09月10日 9:23:15|379**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_PLUGIN_H__FB34F_6CD490C1_8A81D52DC1A913E_0220__TAIL__


