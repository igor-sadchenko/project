#include "stdafx.h"
#include "ChildWork.h"

namespace local {

	ChildWork::ChildWork(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_STD, Key, hInstance, hInstanceRes, AppName)
	{

	}
	ChildWork::~ChildWork()
	{

	}
	LRESULT ChildWork::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			m_Threads.emplace_back(
				[&]() 
				{
					CPaintManagerUI::SetInstance(m_hInstance);
					CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _TEXT("\\ui.skin.trade\\ui.work"));
					m_pUIWork = new UIWork();
					auto hChild = m_pUIWork->Create(m_hWnd, _TEXT("新生联创"), UI_WNDSTYLE_FRAME, 0L);
					::SetParent(hChild, m_hWnd);
					m_pUIWork->CenterWindow();
					CPaintManagerUI::MessageLoop();
				});
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
			int height = HIWORD(lParam);
			int width = LOWORD(lParam);

			RECT rtClient;
			::GetClientRect(hWnd, &rtClient);
			if (m_pUIWork)
			{
				m_pUIWork->AdjustSize(rtClient.left, rtClient.top,width, height);
			}

		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{
			m_pUIWork->Close();
			for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
				it->join();
			m_Threads.clear();
		}break;
		}///switch
		return LRESULT(0);
	}


	UIWork::UIWork()
	{

	}
	UIWork::~UIWork()
	{

	}
	CControlUI* UIWork::CreateControl(LPCTSTR pstrClassName)
	{
		return nullptr;
	}
	void UIWork::AdjustSize(const int& left, const int& top, const int& width, const int& height)
	{
		::MoveWindow(m_hWnd, left, top, width, height, TRUE);
	}
	LRESULT UIWork::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
	LRESULT UIWork::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(GetHWND());
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(sk::string(GetWindowClassName()).append(_TEXT(".xml")).c_str(), (LPCTSTR)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse 'ui.xml'");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		bHandled = TRUE;
		return 0;
	}
	void UIWork::InitWindow()
	{

	}
	LRESULT UIWork::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		int height = HIWORD(lParam);
		int width = LOWORD(lParam);
		bHandled = FALSE;
		return 0;
	}
	void UIWork::Notify(TNotifyUI& msg)
	{
	}
}///namespace local