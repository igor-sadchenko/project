#include "stdafx.h"

auto switch_to_new_thread(std::thread& out) {
    struct awaitable {
        std::thread* p_out;
        bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> h) {
            std::thread& out = *p_out;
            if (out.joinable())
                throw std::runtime_error("Output jthread parameter not empty");
            out = std::thread([h] { h.resume(); });
            // Potential undefined behavior: accessing potentially destroyed *this
            // std::cout << "New thread ID: " << p_out->get_id() << '\n';
            std::cout << "New thread ID: " << out.get_id() << '\n'; // this is OK
        }
        void await_resume() {}
    };
    return awaitable{ &out };
}

struct task {
    struct promise_type {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

task resuming_on_new_thread(std::thread& out) {
    std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << '\n';
    co_await switch_to_new_thread(out);
    // awaiter destroyed here
    std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << '\n';
}


int WINAPI wWinMain
(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    std::thread out;
    auto aldfkj = resuming_on_new_thread(out);

    auto sk = 0;









#if ENABLE_CEF
	CefEnableHighDPISupport();
	void* sandbox_info = nullptr;
#if defined(CEF_USE_SANDBOX)
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif
	CefMainArgs main_args(hInstance);
	int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
	if (exit_code >= 0) {
		return exit_code;
	}
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());
	CefSettings settings;
	if (command_line->HasSwitch("enable-chrome-runtime")) { settings.chrome_runtime = true; }
#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif
    local::gpCefApp = new local::SimpleApp;
	//settings.multi_threaded_message_loop = true;
	CefInitialize(main_args, settings, local::gpCefApp.get(), sandbox_info);
#endif


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

    HINSTANCE hInstanceRes = nullptr;
#if defined(_DEBUG)
     hInstanceRes = ::LoadLibraryA(R"(..\..\..\..\shared\x64\debug\shared.resource.dll)");
#else
   // hInstanceRes = ::LoadLibraryA((local::Global::GetSetup()->GetApiPath() + "shared.resource.dll").c_str());
#endif
    local::ghInstance = hInstance;

    local::Frame* pFrameWnd = new local::Frame(
        hInstance,
        hInstanceRes,
        sk::Log::Format(L"XS® Bridge Server ***Build - [{}]", \
            sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())));

    pFrameWnd->CreateFrame(
        {
            {::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('H'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
            {::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('T'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
            {::GlobalAddAtom(sk::StringConvert::MBytesToTString(sk::UUID().uuid()).c_str()) - 0xC000,MAKELPARAM(INT('E'),MOD_ALT | MOD_CONTROL | MOD_NOREPEAT)},
        }

    );

#if ENABLE_CEF
    CefRunMessageLoop();
    CefShutdown();
#else
    pFrameWnd->MessageLoop();
#endif

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
