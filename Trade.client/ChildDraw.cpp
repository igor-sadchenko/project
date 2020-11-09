#include "stdafx.h"
#include "ChildDraw.h"

namespace local {

	ChildDraw::ChildDraw(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_STD, Key, hInstance, hInstanceRes, AppName)
	{
		glutInit(&__argc, __argv);
#ifdef __FREEGLUT_EXT_H__
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	}
	ChildDraw::~ChildDraw()
	{
		glutExit();
	}
	LRESULT ChildDraw::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			static	PIXELFORMATDESCRIPTOR pfd =	// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
			{
				sizeof(PIXELFORMATDESCRIPTOR),		// 上述格式描述符的大小
				1,		// 版本号
				PFD_DRAW_TO_WINDOW |		// 格式支持窗口
				PFD_SUPPORT_OPENGL |		// 格式必须支持OpenGL
				PFD_DOUBLEBUFFER,			// 必须支持双缓冲
				PFD_TYPE_RGBA,			// 申请 RGBA 格式
				16,			// 选定色彩深度
				0, 0, 0, 0, 0, 0,	// 忽略的色彩位
				0,	// 无Alpha缓存
				0,	// 忽略Shift Bit
				0,	// 无累加缓存
				0, 0, 0, 0,		// 忽略聚集位
				16,	// 16位 Z-缓存 (深度缓存)
				0,	// 无蒙板缓存
				0,	// 无辅助缓存
				PFD_MAIN_PLANE,	// 主绘图层
				0,	// Reserved
				0, 0, 0		// 忽略层遮罩
			};

			m_hDC = ::GetDC(hWnd);
			auto PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
			SetPixelFormat(m_hDC, PixelFormat, &pfd);
			m_hRC = wglCreateContext(m_hDC);
			wglMakeCurrent(m_hDC, m_hRC);


			glShadeModel(GL_SMOOTH);	// 启用阴影平滑
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// 黑色背景
			glClearDepth(1.0f);		// 设置深度缓存
			glEnable(GL_DEPTH_TEST);	// 启用深度测试
			glDepthFunc(GL_LEQUAL);	// 所作深度测试的类型
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// 告诉系统对透视进行修正

		}break;
		case WM_ERASEBKGND:
		{
			bHandle = true;
			return 1;
		}break;
		case WM_PAINT:
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 清除屏幕和深度缓存
			glLoadIdentity();	// 重置当前的模型观察矩阵

			//添加绘图代码
			glTranslatef(-1.5f, 0.0f, -6.0f);	// 左移 1.5 单位，并移入屏幕 6.0
			glBegin(GL_TRIANGLES);	// 绘制三角形
			glVertex3f(0.0f, 1.0f, 0.0f);	// 上顶点
			glVertex3f(-1.0f, -1.0f, 0.0f);	// 左下
			glVertex3f(1.0f, -1.0f, 0.0f);	// 右下
			glEnd();

			glTranslatef(3.0f, 0.0f, 0.0f);	// 右移3单位
			glBegin(GL_QUADS);		//  绘制正方形
			glVertex3f(-1.0f, 1.0f, 0.0f);	// 左上
			glVertex3f(1.0f, 1.0f, 0.0f);	// 右上
			glVertex3f(1.0f, -1.0f, 0.0f);	// 左下
			glVertex3f(-1.0f, -1.0f, 0.0f);	// 右下
			glEnd();	// 正方形绘制结束

			SwapBuffers(m_hDC);
		}break;
		case WM_SIZE:
		{
			RECT rtClient;
			GetClientRect(hWnd, &rtClient);
			auto width = rtClient.right - rtClient.left;
			auto height = rtClient.bottom - rtClient.top;
			glViewport(0, 0, width, height);	// 重置当前的视口
			glMatrixMode(GL_PROJECTION);	// 选择投影矩阵
			glLoadIdentity();				// 重置投影矩阵
			// 设置视口的大小
			gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
			glMatrixMode(GL_MODELVIEW);	// 选择模型观察矩阵
			glLoadIdentity();	// 重置模型观察矩阵
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