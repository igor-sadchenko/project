#if !defined(AFX_SKSTU_GLOBAL_H__2F0CB_6C427F28_4C8964BE6074CE7_0AA1__HEAD__)
#define AFX_SKSTU_GLOBAL_H__2F0CB_6C427F28_4C8964BE6074CE7_0AA1__HEAD__

namespace follow {
	extern HINSTANCE extern_global_hinstance;
	class Global final : public sk::object::IGlobal {
	public:
		Global();
		~Global();
	public:
		int Init() override final;
		int UnInit() override final;
		int License(__int64 time = 0) override final;
	public:
		static void SystemPath(const sk::stringa&);
		static sk::stringa SystemPath();
		static Core* GetCore();
		static shared::SKAPI* GetApi();
		static Setup* SetupGet();
		static const bool EnableRuntimeLog();
		static Shared* SharedGet();
		static void SendLogToView(const sk::packet&);
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		sk::stringa m_SystemPath;
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;
		Core* m_pCore = nullptr;
		shared::SKAPI* m_pApi = nullptr;
		Setup* m_pSetup = nullptr;
		Shared* m_pShared = nullptr;
	};
	Global* GetGlobal();
	extern Global* static_global_Global;
}///namespace follow




/// /*新生联创（上海）**/
/// /*2019年09月10日 9:29:01|270**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__2F0CB_6C427F28_4C8964BE6074CE7_0AA1__TAIL__
