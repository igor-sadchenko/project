#if !defined(AFX_SKSTU_PROCESS_H__F235E_C7ED8587_C0BD3E32C4CF854_6F10__HEAD__)
#define AFX_SKSTU_PROCESS_H__F235E_C7ED8587_C0BD3E32C4CF854_6F10__HEAD__

namespace proxy {

	class LPProcess final : sk::object::IObject {
		friend class System;
	public:
		LPProcess();
		virtual ~LPProcess();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
	public:
		const shared::SystemID& ID() const { return m_ProcessID; }
		void ID(const shared::SystemID& processID) { m_ProcessID = processID; }
		bool Show() const { return m_IsShow.load(); }
		void Show(const bool& show) { m_IsShow.store(show); }
		bool Enable() const { return m_IsEnable.load(); }
		void Enable(const bool& enable) { m_IsEnable.store(enable); }
		bool Status() const { return m_ProcessOpenStatus.load(); }
		const sk::stringa Identify() const { return m_Identify; }
		void Identify(const char* identify) { m_Identify = identify; }
		void ObjPathname(const sk::stringa& pathname) 
		{ 
			m_ObjPathname = pathname; 
			m_ObjExec = sk::Helper::GetFileNameByFullPath(m_ObjPathname);
		}
		const sk::stringa ObjPathname() const { return m_ObjPathname; }
		const PROCESS_INFORMATION& ProcessInfo() const { return m_ProcessInfo; }
	private:
		void ProcessT();
		bool ProcessStatus() const;
	private:
		shared::SystemID m_ProcessID = 0;
		std::atomic_bool m_IsEnable = false;
		std::atomic_bool m_IsShow = false;
		sk::stringa m_Identify;
		sk::stringa m_ObjPathname;
		sk::stringa m_ObjExec;//! ProcessName
		std::atomic_bool m_ProcessOpenStatus = false;
		PROCESS_INFORMATION m_ProcessInfo = { 0 };
	};




}///namespace proxy





/// /*新生联创（上海）**/
/// /*2020年01月5日 9:9:20|247**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_PROCESS_H__F235E_C7ED8587_C0BD3E32C4CF854_6F10__TAIL__
