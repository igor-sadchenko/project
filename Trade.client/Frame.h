#if !defined(AFX_SK_H__C637EF28_721E_48B8_91B4_727DD5C800D9__HEAD__)
#define AFX_SK_H__C637EF28_721E_48B8_91B4_727DD5C800D9__HEAD__

namespace local {

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

	class Frame : public sk::IWindowFrame
	{
	public:
		Frame(HINSTANCE hInstance, HINSTANCE hInstanceRes, sk::string AppName = _T(""), sk::string ClassName = _T(""));
		~Frame();
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
		std::atomic_uint32_t m_OpenPort;//! 开放端口
		volatile long m_Connention = 0;
		std::atomic<EnServerStatus> m_ServerStatus = EnServerStatus::EN_SERVER_STATUS_STOP;
	};
}


//!@ /*新生联创®（上海）*/
//!@ /*Sun Nov 8 06:34:40 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__C637EF28_721E_48B8_91B4_727DD5C800D9__HEAD__*/