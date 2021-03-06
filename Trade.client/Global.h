﻿#if !defined(AFX_SK_H__CFD7E368_A334_492B_B84F_D5D00C05D5C2__HEAD__)
#define AFX_SK_H__CFD7E368_A334_492B_B84F_D5D00C05D5C2__HEAD__

namespace local {
	class Global final : public sk::object::IGlobal
	{
	public:
		Global();
		~Global();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		static bool SystemExit();
		static void SystemExit(const bool&);
		static Core* CoreGet();
		static Setup* SetupGet();
		static shared::SharedApi* SharedApi();
	private:
		std::vector<std::thread> m_Threads;
		std::atomic_bool m_SystemExit = false;
		Core* m_pCore = nullptr;
		Setup* m_pSetup = nullptr;
		shared::SharedApi* m_pSharedApi = nullptr;
	};

	extern HINSTANCE ghInstance;
	extern Global* GlobalGet();
#if ENABLE_CEF
	extern CefRefPtr<local::SimpleApp> gpCefApp;
#endif
}///namespace local


//!@ /*新生联创®（上海）*/
//!@ /*Sat Nov 7 18:42:31 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__CFD7E368_A334_492B_B84F_D5D00C05D5C2__HEAD__*/
