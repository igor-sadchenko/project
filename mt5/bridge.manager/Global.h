#if !defined(AFX_SKSTU_GLOBAL_H__EAC70_E4B8C13C_8AA5C25DDB0A29A_7ED2__HEAD__)
#define AFX_SKSTU_GLOBAL_H__EAC70_E4B8C13C_8AA5C25DDB0A29A_7ED2__HEAD__

namespace client {
	extern HINSTANCE extern_global_hinstance;

	class Global final : public sk::object::IGlobal {
	public:
		Global();
		~Global();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		static Core* GetCore();
		static shared::SharedApi* GetApi();
		static Shared* SharedGet();
		static Setup* SetupGet();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
		static bool IsSystemExit();
		static void SystemExit();
	private:
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;
		Core* m_pCore = nullptr;
		shared::SharedApi* m_pApi = nullptr;
		Shared* m_pShared = nullptr;
		Setup* m_pSetup = nullptr;
		std::atomic<bool> m_SystemExit = false;
	};
	Global* GetGlobal();

}///namespace client




/// /*新生联创（上海）**/
/// /*2019年12月28日 18:24:18|674**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__EAC70_E4B8C13C_8AA5C25DDB0A29A_7ED2__TAIL__