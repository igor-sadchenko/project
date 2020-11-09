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

	int ChildWeb::InitCef()
	{
		return 0;
	}

	void ChildWeb::UnInitCef()
	{

	}

	LRESULT ChildWeb::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			RECT rect;
			::GetClientRect(hWnd, &rect);
			int window_width = rect.right - rect.left;
			int window_beight = rect.bottom - rect.top;
#if 0
			CefEnableHighDPISupport();
			void* sandbox_info = NULL;

			CefMainArgs main_args(m_hInstance);
			CefRefPtr<SimpleApp> app(new SimpleApp);
			auto exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
			CefSettings settings;
			CefString(&settings.log_file).FromWString(L"why.log");
			settings.no_sandbox = true;
			settings.multi_threaded_message_loop = true;
			CefInitialize(main_args, settings, app.get(), sandbox_info);

			CefRefPtr<SimpleHandler> handler(new SimpleHandler(false));
			CefBrowserSettings browser_settings;
			CefWindowInfo window_info;
			rect.top = rect.top + window_beight / 20;
			window_info.SetAsChild(hWnd, rect);
			CefBrowserHost::CreateBrowser(window_info, handler, _T("http://cn.bing.com"), browser_settings, NULL,NULL);
#endif
			auto sk = 0;
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

		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{

		}break;
		}///switch
		return LRESULT(0);
	}




}///namespace local