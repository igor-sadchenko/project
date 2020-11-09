#include "stdafx.h"
#include "Window.h"

namespace local
{

	Window::Window()
	{
		Init();
	}

	Window::~Window()
	{
		UnInit();
	}
#if 1
	void Window::Init() const
	{
		glutInit(&__argc, __argv);
#ifdef __FREEGLUT_EXT_H__
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	}

	void Window::UnInit() const
	{
		glutExit();
	}

	void Window::Open()
	{
		if (m_IsOpen.load()) return;

		m_Threads.emplace_back(
			[&]() 
			{
				do {
					glutInitWindowSize(500, 500);
					//glutInitWindowPosition(0, 0);
					glutCreateWindow("新生联创（上海）");

					glutDisplayFunc(
						[]()
						{
							ImGui_ImplOpenGL2_NewFrame();
							ImGui_ImplGLUT_NewFrame();
							//my_display_code();
							{
								ImGui::Begin("Hello, world!");



								ImGui::End();
							}
							ImGui::Render();
							ImGuiIO& io = ImGui::GetIO();
							glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
							ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
							glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
							glClear(GL_COLOR_BUFFER_BIT);
							ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
							glutSwapBuffers();
							glutPostRedisplay();
						});

					IMGUI_CHECKVERSION();
					ImGui::CreateContext();
					ImGui::StyleColorsDark();
					ImGui_ImplGLUT_Init();
					ImGui_ImplGLUT_InstallFuncs();
					ImGui_ImplOpenGL2_Init();
					glutMainLoop();
					ImGui_ImplOpenGL2_Shutdown();
					ImGui_ImplGLUT_Shutdown();
					ImGui::DestroyContext();
					break;
				} while (1);		
			});
		m_IsOpen.store(true);
	}

	void Window::PostRedisplay() const
	{
		if (m_IsOpen.load())
		{
			glutPostRedisplay();
		}
	}

	void Window::Close()
	{
		if (!m_IsOpen.load()) return;
		glutLeaveMainLoop();
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
			it->join();
		m_Threads.clear();
		m_IsOpen.store(false);
	}
#endif
}///namespace local