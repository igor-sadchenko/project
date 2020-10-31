#if !defined(AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__HEAD__)
#define AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__HEAD__

namespace dealer {

	class Dealer final :
		public IMTTradeSink,
		public IMTConTimeSink,
		public IMTConSymbolSink,
		public IMTUserSink,
		public IMTServerPlugin,
		public IMTRequestSink,
		public IMTDealSink,
		public IMTPositionSink
	{
	public:
		Dealer();
		~Dealer();
	private:
		sk::stringa m_PathObj = "";
		sk::stringw m_RouteRuleName;
		std::atomic<UINT64> m_DealerManagerLogin = 0;

		sk::stringa m_LogKey = "";
		sk::Log* m_pLog = nullptr;
		Setup* m_pSetup = nullptr;
		Exchange* m_pExchange = nullptr;
		Shared* m_pShared = nullptr;
		shared::SharedApi* m_pApi = nullptr;
		std::atomic_int64_t m_ServerTime = 0;
		std::atomic_int64_t m_ServerTimeDiff = 0;
		std::atomic_bool m_License = false;
		std::atomic_bool m_IsOpenT = false;
		std::vector<std::thread> m_Threads;
	public:
		sk::stringa PathObj() const { return m_PathObj; }
		IMTServerAPI* ServerApi() const { return m_pServerApi; }	
		INT64 ServerTime() const { return m_ServerTime.load(); }
		INT64 ServerTimeDiff() const { return m_ServerTimeDiff.load(); }
		bool PluginReady() const { return m_PluginReady.load(); }
	public:
		static Setup* SetupGet();
		static Shared* SharedGet();
		static shared::SharedApi* ApiGet();
		static Exchange* ExchangeGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		void Release() override final;
		MTServerInfo m_ServerInfo;
		IMTServerAPI* m_pServerApi = nullptr;
		std::atomic_bool m_PluginReady = false;
		IMTRequest* m_pRequest = nullptr;
		IMTConfirm* m_pConfirm = nullptr;
	public:
		MTAPIRES Start(IMTServerAPI* server) override final;
		MTAPIRES Stop() override final;
	private:	
		//!签名 : RouteCreate
		//!说明 : 添加/更新 路由规则
		//!注意 : 
		//!日期 : Wed Jun 10 10:30:02 UTC+0800 2020
		MTAPIRES RouteCreate();
		//!签名 : PluginParamentersGet
		//!说明 : 获取插件参数
		//!注意 : 
		//!日期 : Wed Jun 10 10:32:49 UTC+0800 2020
		MTAPIRES ParamentersGet();

		MTAPIRES MTSynchronize();
		MTAPIRES MTServerTimeSync();
		MTAPIRES MTSymbolSync();
		MTAPIRES MTUserSync();
		MTAPIRES MTConGroupSync();
		MTAPIRES MTPositionSync();
	private:
		void OnDealPerform(const IMTDeal*, IMTAccount*, IMTPosition*) override final;
		void OnTradeRequestAdd(const IMTRequest*, const IMTConGroup*, const IMTConSymbol*, const IMTPosition*, const IMTOrder*) override final {}
		void OnTradeRequestUpdate(const IMTRequest*)  override final {}
		void OnTradeRequestDelete(const IMTRequest*)  override final {}
		void OnTradeRequestRefuse(const IMTRequest*) override final;
		void OnTradeRequestProcess(const IMTRequest*, const IMTConfirm*, const IMTConGroup*, const IMTConSymbol*, const IMTPosition*, const IMTOrder*, const IMTDeal*) override final;
		MTAPIRES HookTradeRequestRoute(IMTRequest*, IMTConfirm*, const IMTConGroup*, const IMTConSymbol*, const IMTPosition*, const IMTOrder*) override final;
		MTAPIRES  HookTradeRequestRuleFilter(IMTRequest*, IMTConRoute*, const IMTConGroup*) override final;
		MTAPIRES  HookTradeRequestRuleApply(IMTRequest*, IMTConRoute*, const IMTConGroup*) override final;
		void OnSymbolAdd(const IMTConSymbol*) override final;
		void OnSymbolUpdate(const IMTConSymbol*) override final;
		void OnSymbolDelete(const IMTConSymbol*) override final;
		void OnUserAdd(const IMTUser*) override final;
		void OnUserUpdate(const IMTUser*) override final;
		void OnUserDelete(const IMTUser*) override final;
		void OnTimeUpdate(const IMTConTime*) override final;
		void OnPositionAdd(const IMTPosition*) override final;
		void OnPositionUpdate(const IMTPosition*) override final;
		void OnPositionDelete(const IMTPosition*) override final;
	private:
		void ProcessT();
		void DealerLockT();
		void ProcessCheckT();
		bool GatewayProcessAuto(const IMTRequest*,bool continueRequest = false);
		bool GatewayProcessAuto(const shared::RequestField&);
		void GetOrderActionByRequestAction(const UINT&, shared::tradeorder::EnOrderAction&);
		bool BridgeSystemGeneralSetupVerify();
	public:
		static void ConfirmPriceProcess(__in const shared::SystemOrder&,__out shared::TradeTaskRouteGateway&);
	private:
		MTAPIRES QuickConfirm(const shared::ConfirmField&, const UINT&);
		MTAPIRES QuickConfirm(const IMTRequest*, const UINT&);
#pragma region{business}
	public:
		MTAPIRES TradeRequest(
			__in shared::RequestField&);
		MTAPIRES DealGet(
			__in const UINT64&,
			__out shared::DealField&);
		MTAPIRES PositionGetByTicket(
			__in const UINT64&,
			__out shared::PositionField&);
		MTAPIRES PositionCheck(
			__in const UINT64&);
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
		sk::LicenseInfo m_LicenseInfo;
		long long m_LicenseVersionType = 0;
		long long LicenseGet() { return InterlockedExchangeAdd64(&m_LicenseVersionType, 0); }
		void LicenseSet(const sk::LicenseInfo::EnProductVersionType& at) { InterlockedExchange64(&m_LicenseVersionType, long long(at)); }
	public:
		int License(__int64 time = 0);

	};

	extern HINSTANCE global_hInstance;
	extern Dealer* global_Dealer;
	extern Dealer* DealerGet();

}///namespace dealer




/// /*新生联创（上海）**/
/// /*2020年05月19日 12:10:07|418**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__TAIL__
