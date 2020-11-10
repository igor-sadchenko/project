#include "stdafx.h"
#if 0
// Our state
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void my_display_code()
{
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

void glut_display_func()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    my_display_code();

    // Rendering
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}
#endif

#if 0
int main(int argc, char** argv)
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

    local::gpGlobal = new local::Global();

	std::thread __mian_t__(
		[&]() {
			do {
				sk::packet _input;
				char c;
				while (std::cin >> std::noskipws >> c) { if ('\n' == c) break; _input.push_back(c); }
				std::cin >> std::skipws;
				sk::StringConvert::ToLowerA(_input);
                if (_input == "open")
                {
                    local::Global::WindowGet()->Open();
                }
                else if (_input == "set")
                {
                    //local::Global::WindowGet()->m_DisplayCb = 
                    //    []() 
                    //{
                    //    ImGui_ImplOpenGL2_NewFrame();
                    //    ImGui_ImplGLUT_NewFrame();
                    //    //my_display_code();
                    //    ImGui::Render();
                    //    ImGuiIO& io = ImGui::GetIO();
                    //    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
                    //    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
                    //    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                    //    glClear(GL_COLOR_BUFFER_BIT);
                    //    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
                    //    glutSwapBuffers();
                    //    //glutPostRedisplay();
                    //};
                }
				else if (_input == "q" || _input == "quit" || _input == "exit")
				{
                    local::Global::SystemExit(true);
                    local::Global::WindowGet()->Close();
					break;
				}
				//!@ 系统运行时命令、业务命令、客户命令
				else
				{
					std::cout << "本系统不支持的命令行." << std::endl;
				}
			} while (1);
		});
    __mian_t__.join();
    SK_DELETE_PTR(local::gpGlobal);
    return 0;
}
#else

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

#endif