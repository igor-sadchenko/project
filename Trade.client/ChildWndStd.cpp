#include "stdafx.h"
#include "ChildWndStd.h"
#include <gl/GL.h>

namespace client {
	ChildWndStd::ChildWndStd(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_STD, Key, hInstance, hInstanceRes, AppName)
	{

	}
	ChildWndStd::~ChildWndStd()
	{

	}
	LRESULT ChildWndStd::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{	
		switch (message)
		{
		case WM_CREATE:
		{
			m_hDC = ::GetDC(hWnd);
			GLUTSetupPixelFormat(m_hDC);
			m_hGLRC = ::wglCreateContext(m_hDC);
			::wglMakeCurrent(m_hDC, m_hGLRC);
			glutInit(&__argc, __argv);
			GLUTInit();
		}break;
		case WM_SIZE:
		{
			//GLUTChangeSize(LOWORD(lParam), HIWORD(lParam));
		}break;
		case WM_ERASEBKGND:
		{
			return 1;
		}break;
		case WM_PAINT:
		{
			PAINTSTRUCT     ps;
			

			RECT rtClient;
			GetClientRect(hWnd, &rtClient);
			HDC hdc = BeginPaint(hWnd, &ps);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);//rtΪRECT����;
			SelectObject(hMemDC, hBitmap);

			HBRUSH hbrush, hbrushold;

			hbrush = CreateSolidBrush(RGB(255, 0, 0));
			hbrushold = (HBRUSH)SelectObject(hMemDC, hbrush);

			MoveToEx(hMemDC, 100, 100, NULL);
			LineTo(hMemDC, 150, 230);
			Rectangle(hMemDC, 200, 200, 300, 300);
			Ellipse(hMemDC, 15, 15, 130, 130);

			SelectObject(hMemDC, hbrushold);

			DeleteObject(hbrush);


			//FillRect(hMemDC, &rtClient, WHITE_BRUSH);


			BitBlt(hdc, 0, 0, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top,
				hMemDC, 0, 0, SRCCOPY);
			DeleteDC(hMemDC);
			DeleteObject(hBitmap);



			::EndPaint(hWnd, &ps);

			//DoPaint();
			//::SwapBuffers(m_hDC);
			//::ValidateRect(hWnd, NULL);
			////����ͼƬ
			//hOrgBitmap = (HBITMAP)::LoadImage(m_hInstance, _T("d:\\1.bmp"), IMAGE_BITMAP, 884, 618, LR_LOADFROMFILE);
			//HDC memDc = ::CreateCompatibleDC(dc); //������ǰ�����ĵļ���dc(�ڴ�DC)
			//hOldBitmap = (HBITMAP)::SelectObject(memDc, hOrgBitmap); //��λͼ���ص��ڴ�DC
			////�����ڴ�DC���ݿ鵽��ǰDC���Զ�����
			//::StretchBlt(dc, 0, 0, disWidth, disHeight, memDc, 0, 0, 884, 618, SRCCOPY);
			////�ָ��ڴ�ԭʼ����
			//::SelectObject(memDc, hOldBitmap);
			////ɾ����Դ����ֹй©
			//::DeleteObject(hOrgBitmap);
			//::DeleteDC(memDc);
			bHandle = FALSE;
		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{
			::wglMakeCurrent(m_hDC, NULL);
			::wglDeleteContext(m_hGLRC);
			::ReleaseDC(hWnd, m_hDC);
		}break;
		}///switch
		return LRESULT(0);
	}
	void ChildWndStd::GLUTChangeSize(const int& width, const int& height)
	{
		glViewport(0, 0, width, height);         // ���õ�ǰ���ӿ�glMatrixMode(GL_PROJECTION);  
		glMatrixMode(GL_PROJECTION);             // ѡ��ͶӰ����  
		glLoadIdentity();                        // ����ͶӰ����  
		// ����ͶӰģʽΪ͸��ͶӰ  
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	BOOL ChildWndStd::GLUTInit()
	{
		glShadeModel(GL_SMOOTH);                 // ������Ӱƽ��  
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);    // ��ɫ����  
		glClearDepth(1.0f);                      // ������Ȼ���  
		glEnable(GL_DEPTH_TEST);                 // ������Ȳ���  
		glDepthFunc(GL_LEQUAL);                  // ������Ȳ��Ե�����  
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// ����ϵͳ��͸�ӽ�������  
		return TRUE;                             // ��ʼ�� OK  
	}
	BOOL ChildWndStd::GLUTSetupPixelFormat(const HDC& hdc)
	{
		PIXELFORMATDESCRIPTOR PixelFormatDescRiptor = { 0 };
		PixelFormatDescRiptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		PixelFormatDescRiptor.nVersion = 1;
		PixelFormatDescRiptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		PixelFormatDescRiptor.dwLayerMask = PFD_MAIN_PLANE;
		PixelFormatDescRiptor.iPixelType = PFD_TYPE_RGBA;
		PixelFormatDescRiptor.cColorBits = 8;
		PixelFormatDescRiptor.cDepthBits = 32;
		PixelFormatDescRiptor.cAccumBits = 0;
		PixelFormatDescRiptor.cStencilBits = 0;
		int pixelformat = ::ChoosePixelFormat(hdc, &PixelFormatDescRiptor);
		if (0 == pixelformat)
			return FALSE;
		if (::SetPixelFormat(hdc, pixelformat, &PixelFormatDescRiptor) == FALSE)
			return FALSE;
		return TRUE;
	}

	void ChildWndStd::DoPaint()
	{
#if 0
		glClearColor(0, 1.0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(100, 100, 0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(25, 100, 0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0);
		glEnd();
		glFinish();
#endif
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����������Ȼ�����  
		glLoadIdentity();        // ���õ�ǰ��ģ�͹۲����  
		glTranslatef(-1.5f, 0.0f, -26.0f);       // ���� 1.5 ��λ����������Ļ 6.0  
		glBegin(GL_TRIANGLES);        // ����������  
		glVertex3f(0.0f, 1.0f, 0.0f);      // �϶���  
		glVertex3f(-1.0f, -1.0f, 0.0f);      // ����  
		glVertex3f(1.0f, -1.0f, 0.0f);      // ����  
		glEnd();         // �����λ��ƽ���  
		glTranslatef(3.0f, 0.0f, 0.0f);       // ����3��λ  
		glRotatef(45, 1, 1, 0);
		//glutWireCube(2.0);
		//�˴���ӻ�ͼ����  
		//glutSwapBuffers();  
		glFlush();  
	}

}///namespace client