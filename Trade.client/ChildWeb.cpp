#include "stdafx.h"
#include "ChildWeb.h"

namespace local {

	ChildWeb::ChildWeb(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_STD, Key, hInstance, hInstanceRes, AppName)
	{
	}
	ChildWeb::~ChildWeb()
	{
	}
	LRESULT ChildWeb::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		switch (message)
		{
		case WM_CREATE:
		{
#if ENABLE_CEF
			CefRefPtr<local::SimpleHandler> handler(new local::SimpleHandler(false));
			CefBrowserSettings browser_settings;
			std::string url = "http://cn.bing.com";
			CefWindowInfo window_info;
			RECT rtParentClient;
			rtParentClient.left = 0;
			rtParentClient.right = 800;
			rtParentClient.top = 0;
			rtParentClient.bottom = 800;
			window_info.SetAsChild(hWnd, rtParentClient);
			CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings, nullptr, nullptr);
#endif
		}break;
		case WM_ERASEBKGND:
		{
			bHandle = true;
			return 1;
		}break;
		case WM_PAINT:
		{

		}break;
		case WM_SIZE:
		{
#if ENABLE_CEF
			if (local::SimpleHandler::GetInstance())
			{
				auto hWeb = local::SimpleHandler::GetInstance()->GetHwnd();
				if (hWeb)
				{
					RECT rtClient;
					::GetClientRect(hWnd, &rtClient);
					::MoveWindow(hWeb, rtClient.left, rtClient.top, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, TRUE);
				}
			}
#endif///ENABLE_CEF
		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{
			for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
			{
				it->join();
			}
			m_Threads.clear();

#if ENABLE_CEF
			CefQuitMessageLoop();
#endif
		}break;
		}///switch
		return LRESULT(0);
	}

}///namespace local