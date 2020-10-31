#if !defined(AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__)
#define AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__HEAD__

namespace lmax {

	enum class EnLocalMessageType : __int64 {
		EN_LOCAL_MESSAGE_TYPE_CLOSE_CORE,
		EN_LOCAL_MESSAGE_TYPE_CLOSE_SYSTEM,
	};
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
		static Setup* GetSetup();
		static void SystemExit(const long&);
		static bool SystemExit();
		static Shared* SharedGet();
		static HWND GetHwnd();
		static std::shared_ptr<spdlog::logger> Log(sk::EnLogType logType = sk::EnLogType::EN_LOG_TYPE_SYSTEM);
	private:
		sk::stringa m_LogKey;
		sk::Log* m_pLog = nullptr;
		Core* m_pCore = nullptr;
		shared::SharedApi* m_pApi = nullptr;
		Setup* m_pSetup = nullptr;
		volatile long m_SystemExitFlag = 0;
		Shared* m_pShared = nullptr;
		HWND m_hWnd = nullptr;
	};
	Global* GetGlobal();
}///namespace lmax




/// /*新生联创（上海）**/
/// /*2019年07月30日 14:7:07|120**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_GLOBAL_H__32B6A_1C619967_31A657A50DD5248_A3A5__TAIL__