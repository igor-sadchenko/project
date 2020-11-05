#if !defined(AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__HEAD__)
#define AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__HEAD__

#include "ChildWndStd.h"

namespace client {
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

	class FrameWnd : public sk::IWindowFrame
	{
	public:
		FrameWnd(HINSTANCE hInstance, HINSTANCE hInstanceRes, sk::string AppName = _T(""), sk::string ClassName = _T(""));
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
		std::atomic_uint32_t m_OpenPort;//! 开放端口
		volatile long m_Connention = 0;
		std::atomic<EnServerStatus> m_ServerStatus = EnServerStatus::EN_SERVER_STATUS_STOP;

		sk::Timer m_Timer;
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
		const int m_ServerStatusUpdateTimeinterval = 3000;
	};
}///namespace client



/// /*新生联创（上海）**/
/// /*2020年11月5日 10:9:53|628**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__TAIL__