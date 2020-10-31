#if !defined(AFX_SK_H__5899C5C4_0383_4499_BD3D_5C0971C0DAFE__HEAD__)
#define AFX_SK_H__5899C5C4_0383_4499_BD3D_5C0971C0DAFE__HEAD__

namespace proxy {

	enum struct EnServerStatus : UINT
	{
		EN_SERVER_STATUS_STOP = 0,
		EN_SERVER_STATUS_PAUSE = 1,
		EN_SERVER_STATUS_READY = 2,
	};

	static const TCHAR* ServerStatusStr(const EnServerStatus& status)
	{
		switch (status)
		{
		case EnServerStatus::EN_SERVER_STATUS_STOP:
		{
			return TEXT("服务停止");
		}break;
		case EnServerStatus::EN_SERVER_STATUS_PAUSE:
		{
			return TEXT("服务暂停");
		}break;
		case EnServerStatus::EN_SERVER_STATUS_READY:
		{
			return TEXT("准备就绪");
		}break;
		}
		return TEXT("未知状态");
	}

	class ChildWndStd : public sk::IWindowChild
	{
	public:
		ChildWndStd(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWndStd();
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM,BOOL&) override final;
	};

	class ChildWndLog : public sk::IWindowChild
	{
		typedef struct tagITEMKey
		{
			LPARAM lParam;
			int Index;
			int IndexSub;
			tagITEMKey() { memset(this,0x00,sizeof(*this)); }
			tagITEMKey(const int& idx, const int& idxSub,const LPARAM& lp)
			{
				Index = idx;
				lParam = lp;
				IndexSub = idxSub;
			}
			bool operator<(const tagITEMKey& obj) const
			{
				if (lParam != obj.lParam)
					return lParam < obj.lParam;
				else if (IndexSub != obj.IndexSub)
					return IndexSub < obj.IndexSub;
				return Index < obj.Index;
			}
		}ITEMKey;
	public:
		ChildWndLog(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWndLog();
	private:
		HWND m_hListView = nullptr;
		void ProcessT();
		std::atomic_bool m_IsOpenT = false;
		std::vector<std::thread> m_Threads;
		sk::container::map<ITEMKey, sk::string> m_ListViewCacheQ;
	public:
		void Release() const override final { delete this; }
	protected:	
		LRESULT Function() override final;
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM,BOOL&) override final;
	};

	class ChildWndOnline : public sk::IWindowChild
	{
	public:
		ChildWndOnline(const long&, const HINSTANCE&, const HINSTANCE&, const sk::string&);
		~ChildWndOnline();
	private:
		HWND m_hListView = nullptr;
		void ProcessT();
		std::atomic_bool m_IsOpenT = false;
		std::vector<std::thread> m_Threads;
	public:
		void Release() const override final { delete this; }
	protected:
		LRESULT OnChildMessage(HWND, UINT, WPARAM, LPARAM,BOOL&) override final;
	};


	class FrameWnd : public sk::IWindowFrame
	{
	public:
		FrameWnd(HINSTANCE hInstance, HINSTANCE hInstanceRes,sk::string AppName = _T(""), sk::string ClassName = _T(""));
		~FrameWnd();
	protected:
		LRESULT OnFrameMessage(HWND, UINT, WPARAM, LPARAM, BOOL bHandle = false) override final;
	public:
		long Connention() { return ::_InterlockedExchangeAdd(&m_Connention, 0); }
		long ConnectionIncrement() { return ::_InterlockedIncrement(&m_Connention); }
		long ConnectionDecrement() { return ::_InterlockedDecrement(&m_Connention); }

		EnServerStatus ServerStatus() const { return m_ServerStatus.load(); }
		void ServerStatus(const EnServerStatus& status) { m_ServerStatus.store(status); }
		std::uint32_t OpenPort() const { return m_OpenPort.load(); }
		void OpenPort(const std::uint32_t& port) { m_OpenPort.store(port); }
	private:
		ServerPipe* m_pServerPipe = nullptr;
		std::atomic_uint32_t m_OpenPort;//! 开放端口
		volatile long m_Connention = 0;
		std::atomic<EnServerStatus> m_ServerStatus = EnServerStatus::EN_SERVER_STATUS_STOP;

		sk::Timer m_Timer;
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
		const int m_ServerStatusUpdateTimeinterval = 3000;
	};



}///namespace proxy


//!@ /*新生联创®（上海）*/
//!@ /*Fri Oct 9 15:01:31 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__5899C5C4_0383_4499_BD3D_5C0971C0DAFE__HEAD__*/
