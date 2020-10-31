#if !defined(AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__)
#define AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__

namespace proxy {
	class Global final : public sk::object::IGlobal {
	public:
		Global();
		~Global();
	private:
		virtual int Init() override final;
		virtual int UnInit() override final;
		virtual int License(__int64 time = 0) override final;
	public:
		static Core* GetCore();
		static shared::SharedApi* GetApi();
		static Shared* SharedGet();
		static Setup* GetSetup();
		static HANDLE GetProcessHandle();
		static void SystemExit(const long&);
		static bool SystemExit();
		static sk::Log* LogGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;
		Setup* m_pSetup = nullptr;
		Core* m_pCore = nullptr;
		Shared* m_pShared = nullptr;
		shared::SharedApi* m_pApi = nullptr;
		HANDLE m_hProcess = nullptr;
		volatile long m_SystemExitFlag = 0;
	};
	Global* GetGlobal();
}///namespace proxy




/// /*新生联创（上海）**/
/// /*2019年07月30日 14:7:07|120**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__TAIL__