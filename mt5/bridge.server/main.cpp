#include "stdafx.h"

#if 1
int WINAPI wWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*_CrtSetBreakAlloc(743045);*/
#else
	sk::Dump __Dump;
#endif
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

#if defined(_DEBUG)
	HINSTANCE hInstanceRes = ::LoadLibraryA(R"(..\..\..\..\shared\x64\debug\shared.resource.dll)");
#else
	HINSTANCE hInstanceRes = ::LoadLibraryA((proxy::Global::GetSetup()->GetApiPath() + "shared.resource.dll").c_str());
#endif
	proxy::FrameWnd* pFrameWnd = new proxy::FrameWnd(
		hInstance,
		hInstanceRes,
		sk::Log::Format(L"清算桥系统® Proxy ***内部版本 - [{}]", \
			sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())));

	pFrameWnd->CreateFrame(
		{
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('H'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('T'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
			{::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('E'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
		}

	);

	pFrameWnd->MessageLoop();

	{
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << "System exit is complete ." << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "新生联创 www.skstu.com" << std::endl;
		std::cout << std::endl;
		std::cout << "___ 安全、稳定、声誉 ___" << std::endl;
		
		::FreeConsole();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(3888));
	SK_DELETE_PTR(pFrameWnd);
	return 0;
}
#else

BOOL ConsoleEventHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:// handle the ctrl-c signal
	{
		printf("ctrl-c event\n\n");
		return TRUE;
	}
	case CTRL_CLOSE_EVENT:// ctrl-close: confirm that the user wants to exit.
	{
		printf("ctrl-close event\n\n");
		return TRUE;
	}
	case CTRL_BREAK_EVENT:// pass other signals to the next handler.
	{
		printf("ctrl-break event\n\n");
		return TRUE;
	}
	case CTRL_LOGOFF_EVENT:
	{
		printf("ctrl-logoff event\n\n");
		return FALSE;
	}
	case CTRL_SHUTDOWN_EVENT:
	{
		printf("ctrl-shutdown event\n\n");
		return FALSE;
	}
	default:
	{
		return FALSE;
	}
	}
}

int _tmain(int argc, TCHAR* argv[])
{

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, true);
#if 0
	//!@ 本程序逻辑流程在同一台硬件系统仅支持一个实例运行.
	if (!sk::Helper::ProgramInstanceMark(sk::Helper::GetCurrentProcessNameA()))
	{
		std::cout << "本程序逻辑流程在同一台硬件系统,仅支持一个实例运行." << std::endl;
		std::cout << "This program logic flow in the same hardware system to support only one instance to run" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return 0;
	}
#endif
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*_CrtSetBreakAlloc(743045);*/
#else
	sk::Dump __Dump;
#endif

	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}
#if 0
	if (!AllocConsole())
	{
		FreeConsole();
	}	

	FILE* pCout = nullptr;
	if (AllocConsole())
	{
		HANDLE hOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);//获得控制台输出句柄
		freopen_s(&pCout, "conout$", "w", stdout);
	}
#endif

	::SetConsoleTitleW(sk::Log::Format(L"清算桥系统® Proxy ***内部版本 - [{}]", sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())).c_str());

	proxy::GetGlobal()->GetCore()->Open();

	std::thread process(
		[&]() {
			do {
				sk::stringa _input;
				char c;
				while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
				std::cin >> std::skipws;
				sk::StringConvert::ToLowerA(_input);

				if (_input == "q" || _input == "quit" || _input == "exit")
				{
					proxy::GetGlobal()->GetCore()->Close();
					proxy::Global::SystemExit(1);
					break;
				}
				//!@ 系统运行时命令、业务命令、客户命令
				else
				{
					auto execResult = proxy::Global::GetCore()->GetSystem()->ExecSystemCommand(_input);
					if (execResult != 0)
					{
						std::cout << "本系统不支持的命令行." << std::endl;
					}
				}
			} while (1);
		});



	HWND hWnd = nullptr;
	typedef HWND(WINAPI* PROCGETCONSOLEWINDOW)();
	PROCGETCONSOLEWINDOW GetConsoleWindow;
	HMODULE hKernel32 = GetModuleHandle(L"kernel32");
	if (hKernel32)
	{
		GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");
		if (GetConsoleWindow)
		{
			hWnd = GetConsoleWindow();
			if (hWnd)
				ShowWindow(hWnd, SW_SHOW);
		}
	}
	
	if (RegisterHotKey(
		NULL,
		1,
		MOD_ALT | MOD_CONTROL | MOD_NOREPEAT,
		int('H')))  //0x42 is 'b'
	{
		_tprintf(_T("Hotkey 'CTRL+ALT+h' registered, using MOD_NOREPEAT flag\n"));
		proxy::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("{}", "Hotkey 'CTRL+ALT+h' registered, using MOD_NOREPEAT flag");
	}
	else
	{
		proxy::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("{}({})", "Hotkey 'CTRL+ALT+h' registered, using MOD_NOREPEAT flag",GetLastError());
	}

	do {
		MSG msg = { 0 };
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_HOTKEY)
			{
				if (hWnd)
				{
					if (IsWindowVisible(hWnd))
						ShowWindow(hWnd, SW_HIDE);
					else
						ShowWindow(hWnd, SW_SHOW);
				}
			}
		}
		if (proxy::Global::SystemExit()) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (1);

	sk::Helper::WriteConsoleInputQ();
	process.join();
#if 0
	if (pCout)fclose(pCout);
	FreeConsole();
#endif
	return 0;
}
#endif