#if !defined(AFX_SK_H__CFD7E368_A334_492B_B84F_D5D00C05D5C2__HEAD__)
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
		static Window* WindowGet();
		static bool SystemExit();
		static void SystemExit(const bool&);
	private:
		Window* m_pWindow = nullptr;
		std::vector<std::thread> m_Threads;
		std::atomic_bool m_SystemExit = false;
	};

	extern HINSTANCE ghInstance;
	extern Global* gpGlobal;
	Global* GlobalGet();
}///namespace local


//!@ /*新生联创®（上海）*/
//!@ /*Sat Nov 7 18:42:31 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__CFD7E368_A334_492B_B84F_D5D00C05D5C2__HEAD__*/
