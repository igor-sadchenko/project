#if !defined(AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__HEAD__)
#define AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__HEAD__

namespace feeder {

	class Feeder final :
		public IMTTickSink,
		public IMTConTimeSink,
		public IMTServerPlugin
	{
	public:
		Feeder();
		~Feeder();
	private:
		sk::stringa m_PathObj = "";

		sk::stringa m_LogKey = "";
		sk::Log* m_pLog = nullptr;
		Setup* m_pSetup = nullptr;
		DataSource* m_pDataSource = nullptr;
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
		static DataSource* DataSourceGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		void Release() override final;
		MTServerInfo m_ServerInfo;
		IMTServerAPI* m_pServerApi = nullptr;
		std::atomic_bool m_PluginReady = false;
	public:
		MTAPIRES Start(IMTServerAPI* server) override final;
		MTAPIRES Stop() override final;
	private:
		MTAPIRES ParamentersGet();
		MTAPIRES MTSynchronize();
		MTAPIRES MTServerTimeSync();
		MTAPIRES MTSymbolSync();
	private:
		MTAPIRES HookTick(const int, MTTick&) override final;
		MTAPIRES HookTickStat(const int, MTTickStat&) override final;
		void OnTick(LPCWSTR, const MTTickShort&) override final;
		void OnTickStat(const MTTickStat&) override final;
		void OnTick(const int, const MTTick&) override final;
		void OnTickStat(const int, const MTTickStat&) override final;
		void OnTimeUpdate(const IMTConTime*) override final;
	private:
		void ProcessT();
	private:
		sk::LicenseInfo m_LicenseInfo;
		long long m_LicenseVersionType = 0;
		long long LicenseGet() { return InterlockedExchangeAdd64(&m_LicenseVersionType, 0); }
		void LicenseSet(const sk::LicenseInfo::EnProductVersionType& at) { InterlockedExchange64(&m_LicenseVersionType, long long(at)); }
	public:
		int License(__int64 time = 0);

	};

	extern HINSTANCE global_hInstance;
	extern Feeder* global_Feeder;
	extern Feeder* FeederGet();

}///namespace feeder




/// /*新生联创（上海）**/
/// /*2020年05月19日 12:10:07|418**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_FOLLOW_H__0C416_881A78CB_5C44734285BC218_545F__TAIL__
