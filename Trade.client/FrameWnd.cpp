#include "stdafx.h"
#include "FrameWnd.h"

#include <Richedit.h>
#include <commdlg.h>

namespace client {
	FrameWnd::FrameWnd(HINSTANCE hInstance, HINSTANCE hInstanceRes, sk::string AppName, sk::string ClassName) :
		sk::IWindowFrame(hInstance, hInstanceRes, AppName, ClassName)
	{

	}

	FrameWnd::~FrameWnd()
	{

	}

	LRESULT FrameWnd::OnFrameMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL bHandle)
	{
		LRESULT result = 0;
		switch (message)
		{
		case WM_CREATE:
		{
			m_Timer.Open();
			m_Timer.TimerAppend(m_ServerStatusUpdateTimeinterval, this, OnTimer);

			RECT rtMainWnd;
			::GetClientRect(m_hWnd, &rtMainWnd);
			int nStatusPartsWidths[4];
			int nCnt = 0;
			int i;
			for (i = 4 - 1; i >= 0; i--)
				nStatusPartsWidths[nCnt++] = (rtMainWnd.right - rtMainWnd.left) - (300 * i);
			SendMessage(m_hStatusBar, SB_SETPARTS, ARRAYSIZE(nStatusPartsWidths), (LPARAM)nStatusPartsWidths);

			auto pChildStd = new ChildWndStd(ChildID(), m_hInstance, m_hInstanceRes, TEXT("Child Window Std"));
			AppendChild(pChildStd);


			CreateTaskBar(TEXT("桥系统服务®"));

			CreateToolBar(
				{
					{IDB_PNG_START_16px,TEXT("PNG")},
					/*{IDB_PNG_PAUSE_16px,TEXT("PNG")},*/
					{IDB_PNG_STOP_16px,TEXT("PNG")},
					/*{IDB_PNG_SETTING_16px,TEXT("PNG")},*/
				});
			::SendMessage(m_hToolBar, TB_SETSTATE, (WPARAM)IDB_PNG_STOP_16px, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
		}break;
		case WM_COMMAND:
		{
			switch (wParam)
			{
			case IDB_PNG_START_16px:
				[[fallthrough]];
			case ID_COMMAND_40009:
				[[fallthrough]];
			case ID_COMMAND_40001:
			{//!@ 启动
				::EnableMenuItem(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40001, MF_GRAYED | MF_BYCOMMAND);
				::EnableMenuItem(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40002, MF_ENABLED | MF_BYCOMMAND);

				SendMessage(m_hToolBar, TB_SETSTATE, (WPARAM)IDB_PNG_START_16px, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
				SendMessage(m_hToolBar, TB_SETSTATE, (WPARAM)IDB_PNG_STOP_16px, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));

				bHandle = TRUE;
				result = 0;
			}break;
			case IDB_PNG_STOP_16px:
				[[fallthrough]];
			case ID_COMMAND_40012:
				[[fallthrough]];
			case ID_COMMAND_40002:
			{//!@ 停止
#if 0
				Global::SystemExit(1);
				GetGlobal()->GetCore()->Close();
				::EnableMenuItem(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40001, MF_ENABLED | MF_BYCOMMAND);
				::EnableMenuItem(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40002, MF_GRAYED | MF_BYCOMMAND);

				SendMessage(m_hToolBar, TB_SETSTATE, (WPARAM)IDB_PNG_STOP_16px, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
				SendMessage(m_hToolBar, TB_SETSTATE, (WPARAM)IDB_PNG_START_16px, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
				return LRESULT(0);
#else
				::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
#endif
			}break;
			case ID_COMMAND_40005:
			{//!@ 初始化Symbol

			}break;
			case ID_COMMAND_40006:
			{
				ChildTile();
			}break;
			case ID_COMMAND_40007:
			{
				ChildCascade();
			}break;
			case ID_COMMAND_40008:
			{//!@ About US
				m_ChildQ.iterate(
					[&](const auto&, sk::IWindowChild* pChild, auto& itbreak)
					{
						if (pChild->ChildType() == sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_LOG)
						{
							pChild->Function();
							itbreak = true;
						}
					});
			}break;
			case ID_COMMAND_40014:
			{//!@ 控制交易所模块的显示/隐藏

			}break;
			}///switch
		}break;
		case WM_NOTIFY:
		{

		}break;
		case WM_PAINT:
		{
			auto sk = 0;
		}break;
		case WM_SIZE:
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
			{
				::ShowWindow(hWnd, SW_HIDE);
			}break;
			case SIZE_RESTORED:
				[[fallthrough]];
			case SIZE_MAXIMIZED:
				[[fallthrough]];
			case SIZE_MAXSHOW:
				[[fallthrough]];
			case SIZE_MAXHIDE:
			{

			}break;
			}

			RECT rtMainWnd;
			::GetClientRect(m_hWnd, &rtMainWnd);
			int	nStatusPartsWidths[4];
			int nCnt = 0;
			int i;
			for (i = 4 - 1; i >= 0; i--)
				nStatusPartsWidths[nCnt++] = (rtMainWnd.right - rtMainWnd.left) - (300 * i);
			SendMessage(m_hStatusBar, SB_SETPARTS, ARRAYSIZE(nStatusPartsWidths), (LPARAM)nStatusPartsWidths);
		}break;
		case WM_CLOSE:
		{
			m_Timer.Close();
		}break;
		case WM_TASKBAR:
		{
			switch (lParam)
			{
			case WM_LBUTTONDOWN:
			{

			}break;
			case WM_LBUTTONDBLCLK:
			{
				if (::IsWindowVisible(hWnd))
				{
					::ShowWindow(hWnd, SW_HIDE);
					DWORD dwStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
					//::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					//if (!((dwStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST))
					//{
					//	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					//}
				}
				else
				{
					::ShowWindow(hWnd, SW_SHOWNORMAL);
					::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				}
			}break;
			case WM_RBUTTONDOWN:
			{
				POINT pt;
				GetCursorPos(&pt);
				m_hPopMenuTaskBar = LoadMenu(GetHinstanceRes(), MAKEINTRESOURCE(IDR_MENU_TASKBAR));
				m_hPopMenuTaskBar = ::GetSubMenu(m_hPopMenuTaskBar, 0);

				MENUITEMINFO menuInfo = { 0 };
				menuInfo.cbSize = sizeof(MENUITEMINFO);
				menuInfo.fMask = MIIM_STATE | MIIM_SUBMENU;
				if (::GetMenuItemInfo(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40001, FALSE, &menuInfo))
				{
					::EnableMenuItem(m_hPopMenuTaskBar, ID_COMMAND_40009, MF_BYCOMMAND | menuInfo.fState);
				}
				if (::GetMenuItemInfo(::GetSubMenu(m_hMenu, 0), ID_COMMAND_40002, FALSE, &menuInfo))
				{
					::EnableMenuItem(m_hPopMenuTaskBar, ID_COMMAND_40012, MF_BYCOMMAND | menuInfo.fState);
				}
				::SetForegroundWindow(m_hWnd);
				TrackPopupMenu(m_hPopMenuTaskBar, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hWnd, NULL);
			}break;
			}
		}break;
		case WM_DESTROY:
		{

			//m_pServerPipe->Close();
		}break;
		case WM_HOTKEY:
		{
			auto id = (int)wParam; // identifier of hot key
			auto fsModifiers = (UINT)LOWORD(lParam); // key-modifier flags
			auto vk = (UINT)HIWORD(lParam); // virtual-key code

			if ((vk == 'h' || vk == 'H') && (fsModifiers & MOD_ALT) && (fsModifiers & MOD_CONTROL))
			{
				if (::IsWindowVisible(hWnd))
				{
					::ShowWindow(hWnd, SW_HIDE);
					DWORD dwStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
					::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					if (!((dwStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST))
					{
						::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}
				}
				else
				{
					::ShowWindow(hWnd, SW_SHOWNORMAL);
				}
			}
			else if ((vk == 't' || vk == 'T') && (fsModifiers & MOD_ALT) && (fsModifiers & MOD_CONTROL))
			{
				ChildTile();
			}
			else if ((vk == 'e' || vk == 'E') && (fsModifiers & MOD_ALT) && (fsModifiers & MOD_CONTROL))
			{
				ChildCascade();
			}
			else if ((vk == 'c' || vk == 'C') && (fsModifiers & MOD_ALT) && (fsModifiers & MOD_CONTROL))
			{

			}
		}break;
		}///switch
		return result;
	}

	int FrameWnd::OnTimer(const sk::int64& interval, const sk::intptr& coreptr, const sk::int64& currtime)
	{
		return 0;
	}
}///namespace client