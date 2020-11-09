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
#if 0
	int ChildWeb::InitCef()
	{
		//CefEnableHighDPISupport();
		//void* sandbox_info = NULL;

		//CefMainArgs main_args(m_hInstance);
		//CefRefPtr<App> app(new App);
		//auto exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
		//CefSettings settings;
		///*CefString(&settings.log_file).FromWString(L"why.log");*/
		//settings.no_sandbox = true;
		//settings.multi_threaded_message_loop = true;
		//CefInitialize(main_args, settings, app.get(), sandbox_info);
		return 0;
	}
#endif
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

			//m_pCefWnd->Open();
			//CefRefPtr<Client> g_handler(new Client());
			//CefBrowserSettings browser_settings;
			//CefWindowInfo window_info;
			//RECT rt;
			//GetWindowRect(m_hWnd, &rt);
			//window_info.SetAsChild(m_hWnd, rt);

			//CefBrowserHost::CreateBrowser(window_info, g_handler, _T("http://cn.bing.com"), browser_settings,nullptr, nullptr);
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


	CefChildWindow::CefChildWindow()
	{
		Init();
	}

	CefChildWindow::~CefChildWindow()
	{
		UnInit();
	}

	int CefChildWindow::Init()
	{
#if 0
		CefEnableHighDPISupport();
		void* sandbox_info = nullptr;
#if defined(CEF_USE_SANDBOX)
		// Manage the life span of the sandbox information object. This is necessary
		// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
		CefScopedSandboxInfo scoped_sandbox;
		sandbox_info = scoped_sandbox.sandbox_info();
#endif
		CefMainArgs main_args(ghInstance);
		int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
		if (exit_code >= 0) {
			return exit_code;
		}
		CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
		command_line->InitFromString(::GetCommandLineW());
		CefSettings settings;
		if (command_line->HasSwitch("enable-chrome-runtime")) {
			settings.chrome_runtime = true;
		}
#if !defined(CEF_USE_SANDBOX)
		settings.no_sandbox = true;
#endif
		CefRefPtr<SimpleApp> app(new SimpleApp);
		CefInitialize(main_args, settings, app.get(), sandbox_info);
#endif
		return 0;
	}

	int CefChildWindow::UnInit()
	{
		return 0;
	}

	int CefChildWindow::Open()
	{
		if (m_IsOpen.load()) return 0;

		m_IsOpen.store(true);
		return 0;
	}

	int CefChildWindow::Close()
	{
		if (!m_IsOpen.load()) return 0;

		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();

		m_IsOpen.store(false);
		return 0;
	}
}///namespace local