#include "stdafx.h"
#include "FrameWnd.h"

#include <Richedit.h>
#include <commdlg.h>

namespace proxy {
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ChildWndLog::ChildWndLog(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_LOG, Key, hInstance, hInstanceRes, AppName)
	{
		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
	}
	ChildWndLog::~ChildWndLog()
	{
		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
			it->join();
		m_Threads.clear();
	}

	LRESULT ChildWndLog::Function()
	{
		if (m_hListView)
		{
	/*		RECT rtClient;
			GetClientRect(m_hListView, &rtClient);
			::InvalidateRect(m_hListView, &rtClient, TRUE);*/
		}

		if (m_hListView)
		{
			//RECT rtClient;
			//GetClientRect(m_hWnd, &rtClient);
			//::InvalidateRect(m_hWnd, &rtClient, TRUE);
		}

		{
			HDC hdc;
			hdc = ::GetDC(m_hWnd);
			WPARAM w = (WPARAM)hdc;
			LPARAM l = 0;
			DefMDIChildProc(m_hWnd, WM_ERASEBKGND, w, l);
		}


		return LRESULT{ 0 };
	}

	LRESULT ChildWndLog::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		LRESULT result = 0;
		HWND hWndClient = GetParent(hWnd);
		HWND hWndFrame = GetParent(hWndClient);

		sk::IWindowFrame* __pFrame = reinterpret_cast<sk::IWindowFrame*>(GetWindowLongPtr(hWndFrame, GWLP_USERDATA));

		switch (message)
		{
		case WM_CREATE:
		{
			HMENU hmenu = GetSystemMenu(hWnd, false);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
			auto style = GetWindowLongPtr(hWnd, GWL_STYLE);
			style &= ~(WS_MINIMIZEBOX);
			style &= ~(WS_MAXIMIZEBOX);
			SetWindowLongPtr(hWnd, GWL_STYLE, style);

			m_hListView = \
				::CreateWindowEx(
					WS_EX_CLIENTEDGE,
					WC_LISTVIEW,
					_TEXT(""),
					WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDRAWFIXED,
					0,
					0,
					0,
					0,
					hWnd,
					HMENU(1),
					GetHinstance(),
					this);
			if (!m_hListView)
				break;
			ListView_SetExtendedListViewStyleEx(m_hListView, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

			//auto styleListView = ::GetWindowLongPtr(m_hListView, GWL_STYLE);
			//styleListView &= ~CS_HREDRAW;
			//styleListView &= ~CS_VREDRAW;
			//::SetWindowLongPtr(m_hListView, GWL_STYLE, styleListView);

			LV_COLUMN lvc = { 0 };
			TCHAR szText[64] = { 0 };
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt = LVCFMT_CENTER;

			lvc.cx = 0;
			_stprintf_s(szText, TEXT("%s"), TEXT("Reserve"));
			lvc.pszText = szText;
			lvc.iSubItem = 0;
			ListView_InsertColumn(m_hListView, 0, &lvc);

			lvc.cx = 250;
			_stprintf_s(szText, TEXT("%s"), TEXT("DateTime"));
			lvc.pszText = szText;
			lvc.iSubItem = 1;
			ListView_InsertColumn(m_hListView, 1, &lvc);

			lvc.cx = 500;
			lvc.fmt = LVCFMT_LEFT;
			_stprintf_s(szText, TEXT("%s"), TEXT("Message"));
			lvc.pszText = szText;
			lvc.iSubItem = 2;
			ListView_InsertColumn(m_hListView, 2, &lvc);

			ListView_DeleteColumn(m_hListView, 0);

			if (m_ListViewCacheQ.size() > 10000)
			{
				ListView_DeleteAllItems(m_hListView);
				m_ListViewCacheQ.clear();
			}

			Global::LogGet()->MsgCb(
				[&](const spdlog::level::level_enum& level, const std::chrono::time_point<std::chrono::system_clock>& time, const spdlog::string_view_t& msg)
				{
					std::time_t tt = std::chrono::system_clock::to_time_t(time);
					tm logtime;
					localtime_s(&logtime, &tt);
					std::wstringstream ss;
					ss << std::put_time(&logtime, L"%F %T");
					sk::string strLogTime = ss.str();
					strLogTime.append(L".").append(tostringw(std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count() % 1000));
#if 0
					int nCount = ListView_GetItemCount(m_hListView);
					if (nCount >= 50)
					{
						ListView_DeleteItem(m_hListView, 0);
						nCount--;
					}
#endif
					std::vector<sk::string> parse;
					sk::Helper::OneUnknownParse(sk::StringConvert::MBytesToTString(sk::stringa(msg.data(), msg.size())), '\n', parse);

					for (const auto& node : parse)
					{
						DWORD_PTR dwResult = 0;
						::SendMessageTimeout(m_hListView, LVM_GETITEMCOUNT, 0, 0L, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
						if (dwResult < 0)
							break;

						LV_ITEM lvi = { 0 };
						lvi.mask = LVIF_PARAM;
						lvi.iItem = static_cast<decltype(lvi.iItem)>(dwResult);
						lvi.lParam = level;

						::SendMessageTimeout(m_hListView, LVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
						if (dwResult != -1)
						{
							m_ListViewCacheQ.push(ITEMKey(lvi.iItem, 0, lvi.lParam), strLogTime);
							m_ListViewCacheQ.push(ITEMKey(lvi.iItem, 1, lvi.lParam), node);
						}
					}
				});
		}break;
		case WM_MEASUREITEM:
		{
			MEASUREITEMSTRUCT* m = (MEASUREITEMSTRUCT*)lParam;
			m->itemHeight = 20;
		}break;
#if 0
		case WM_ERASEBKGND:
		{
			HDC hDC = reinterpret_cast<HDC>(wParam);

			//bHandle = TRUE;
			//result = 1;
		}break;

		case WM_PAINT:
		{
			PAINTSTRUCT pt;
			HDC hDC = ::BeginPaint(hWnd, &pt);
#if 0
			HDC hDCMem = ::CreateCompatibleDC(hDC);

			HBRUSH hBrush = ::CreateSolidBrush(RGB(255, 255, 255));
			RECT rtClient = { 0 };
			::GetClientRect(hWnd, &rtClient);
			HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
			::SelectObject(hDCMem, hBitmap);
			::FillRect(hDCMem, &rtClient, hBrush);

			::BitBlt(hDC, 0, 0, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hDCMem, 0, 0, SRCCOPY);
			::DeleteDC(hDCMem);
			::DeleteObject(hBrush);
			::DeleteObject(hBitmap);
#endif
			::EndPaint(hWnd, &pt);
		}break;
#endif
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)(lParam);

			if (pDIS->hwndItem == m_hListView)
			{
				int lvc0Width{ ListView_GetColumnWidth(pDIS->hwndItem, 0) }, lvc1Width{ ListView_GetColumnWidth(pDIS->hwndItem, 1) };

				LV_ITEM lvi = { 0 };
				lvi.mask = LVIF_TEXT | LVIF_PARAM | LVCF_SUBITEM;
				lvi.iSubItem = 0;
				lvi.iItem = pDIS->itemID;
				if (ListView_GetItem(pDIS->hwndItem, &lvi))
				{
					m_ListViewCacheQ.search(
						ITEMKey(lvi.iItem, lvi.iSubItem, lvi.lParam),
						[&](const auto&, auto& text)
						{
							/*HBRUSH bg = (HBRUSH)(::GetStockObject(LTGRAY_BRUSH));*/
							/*::SelectObject(hDC, bg);*/
							/*::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);*/
							HDC hDC = pDIS->hDC;
							RECT rt = pDIS->rcItem;
							rt.right = rt.left + lvc0Width;
							HPEN pen = (HPEN)(::GetStockObject(BLACK_PEN));
							::SelectObject(hDC, pen);
							::SelectObject(hDC, m_hFont);
							::SetTextColor(hDC, RGB(54, 54, 54));
							::DrawText(hDC, text.c_str(), (int)text.size(), &rt, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
						});
				}

				lvi.iSubItem = 1;
				if (ListView_GetItem(pDIS->hwndItem, &lvi))
				{
					m_ListViewCacheQ.search(
						ITEMKey(lvi.iItem, lvi.iSubItem, lvi.lParam),
						[&](const auto&, auto& text)
						{
							HDC hDC = pDIS->hDC;
							RECT rt = pDIS->rcItem;
							rt.left = lvc0Width;
							rt.right = rt.left + lvc1Width;
							HPEN pen = (HPEN)(::GetStockObject(BLACK_PEN));
							::SelectObject(hDC, pen);
							::SelectObject(hDC, m_hFont);

							switch (spdlog::level::level_enum(lvi.lParam))
							{
							case spdlog::level::level_enum::warn:
							{
								::SetTextColor(hDC, RGB(139, 105, 20));
							}break;
							case spdlog::level::level_enum::err:
							{
								::SetTextColor(hDC, RGB(205, 51, 51));
							}break;
							default:
							{
								::SetTextColor(hDC, RGB(79, 79, 79));
							}break;
							}
							::DrawText(hDC, text.c_str(), (int)text.size(), &rt, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
						});
				}

			}


		}break;
		case WM_SIZE:
		{
			RECT rtClientFrame;
			::GetClientRect(hWndFrame, &rtClientFrame);

			switch (wParam)
			{
			case SIZE_MAXIMIZED:
			{
				::MoveWindow(m_hWnd, 0, 0, rtClientFrame.right - rtClientFrame.left, rtClientFrame.bottom - rtClientFrame.top, TRUE);
			}break;
			case SIZE_RESTORED:
				[[fallthrough]];
			case SIZE_MINIMIZED:
				[[fallthrough]];
			case SIZE_MAXSHOW:
				[[fallthrough]];
			case SIZE_MAXHIDE:
			{

			}break;
			}

			if (m_hListView)
			{
				::MoveWindow(m_hListView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
				ListView_SetColumnWidth(m_hListView, 1, (LONG)LOWORD(lParam) - (LONG)190);
			}

			return LRESULT(0);
		}break;
		case WM_GETMINMAXINFO:
		{
#if 1
			RECT rtFrame;
			::GetClientRect(hWndFrame, &rtFrame);

			MINMAXINFO* mminfo = (PMINMAXINFO)lParam;
			mminfo->ptMinTrackSize.x = 400;
			mminfo->ptMinTrackSize.y = 400;
			mminfo->ptMaxSize.x = rtFrame.right - rtFrame.left;
			mminfo->ptMaxSize.y = rtFrame.bottom - rtFrame.top;
			mminfo->ptMaxPosition.x = 0;
			mminfo->ptMaxPosition.y = 0;
			mminfo->ptMaxTrackSize.x = mminfo->ptMaxSize.x;
			mminfo->ptMaxTrackSize.y = mminfo->ptMaxSize.y;

			return LRESULT(0);
#endif
		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{
			Global::LogGet()->MsgCb(nullptr);
		}break;
		}///switch
		return result;
	}

	void ChildWndLog::ProcessT()
	{
		time_t TimeIntervalScrollPrev = 0;
		const time_t TimeIntervalScroll = 2000;

		time_t TimeIntervalRefreshPrev = 0;
		const time_t TimeIntervalRefresh = 5000;

		do {

			time_t TimeNow = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			if (TimeIntervalScrollPrev <= 0)
				TimeIntervalScrollPrev = TimeNow;
			if (TimeIntervalRefreshPrev <= 0)
				TimeIntervalRefreshPrev = TimeNow;

			if (m_hListView)
			{
				if (TimeNow - TimeIntervalScrollPrev >= TimeIntervalScroll)
				{
					TimeIntervalScrollPrev = TimeNow;
					DWORD_PTR dwResult = 0;
					::SendMessageTimeout(m_hListView, LVM_GETITEMCOUNT, 0, 0L, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
					if (dwResult > 0)
					{
						::SendMessageTimeout(m_hListView, LVM_ENSUREVISIBLE, (WPARAM)(int)(dwResult - 1), MAKELPARAM(FALSE, 0), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
					}
				}

				if (TimeNow - TimeIntervalRefreshPrev >= TimeIntervalRefresh)
				{
					RECT rtListView = { 0 };
					if (::GetClientRect(m_hListView, &rtListView))
					{
						//::InvalidateRect(m_hListView, &rtListView, TRUE);
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!m_IsOpenT.load())
				break;
		} while (1);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ChildWndOnline::ChildWndOnline(const long& Key, const HINSTANCE& hInstance, const HINSTANCE& hInstanceRes, const sk::string& AppName) :
		sk::IWindowChild(sk::EnWindowChildType::EN_WINDOW_CHILD_TYPE_LOG, Key, hInstance, hInstanceRes, AppName)
	{
		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
	}
	ChildWndOnline::~ChildWndOnline()
	{
		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
			it->join();
		m_Threads.clear();
	}

	LRESULT ChildWndOnline::OnChildMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,BOOL& bHandle)
	{
		HWND hWndClient = GetParent(hWnd);
		HWND hWndFrame = GetParent(hWndClient);

		sk::IWindowFrame* __pFrame = reinterpret_cast<sk::IWindowFrame*>(GetWindowLongPtr(hWndFrame, GWLP_USERDATA));

		switch (message)
		{
		case WM_CREATE:
		{
			HMENU hmenu = GetSystemMenu(hWnd, false);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
			auto style = GetWindowLongPtr(hWnd, GWL_STYLE);
			style &= ~(WS_MINIMIZEBOX);
			style &= ~(WS_MAXIMIZEBOX);
			SetWindowLongPtr(hWnd, GWL_STYLE, style);

			m_hListView = \
				::CreateWindowEx(
					WS_EX_CLIENTEDGE,
					WC_LISTVIEW,
					_TEXT(""),
					WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT,
					0,
					0,
					0,
					0,
					hWnd,
					HMENU(1),
					GetHinstance(),
					this);
			if (!m_hListView)
				break;
			ListView_SetExtendedListViewStyleEx(m_hListView, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
			LV_COLUMN lvc = { 0 };
			TCHAR szText[64] = { 0 };
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt = LVCFMT_CENTER;

			lvc.cx = 0;
			_stprintf_s(szText, TEXT("%s"), TEXT("Reserve"));
			lvc.pszText = szText;
			lvc.iSubItem = 0;
			ListView_InsertColumn(m_hListView, 0, &lvc);

			lvc.cx = 120;
			_stprintf_s(szText, TEXT("%s"), TEXT("ID"));
			lvc.pszText = szText;
			lvc.iSubItem = 1;
			ListView_InsertColumn(m_hListView, 1, &lvc);

			lvc.cx = 180;
			_stprintf_s(szText, TEXT("%s"), TEXT("Address"));
			lvc.pszText = szText;
			lvc.iSubItem = 2;
			ListView_InsertColumn(m_hListView, 2, &lvc);

			lvc.cx = 180;
			_stprintf_s(szText, TEXT("%s"), TEXT("ActiveTime"));
			lvc.pszText = szText;
			lvc.iSubItem = 3;
			ListView_InsertColumn(m_hListView, 3, &lvc);

			lvc.cx = 180;
			_stprintf_s(szText, TEXT("%s"), TEXT("Username"));
			lvc.pszText = szText;
			lvc.iSubItem = 4;
			ListView_InsertColumn(m_hListView, 4, &lvc);

			lvc.cx = 180;
			_stprintf_s(szText, TEXT("%s"), TEXT("Identify"));
			lvc.pszText = szText;
			lvc.iSubItem = 5;
			ListView_InsertColumn(m_hListView, 4, &lvc);

			ListView_DeleteColumn(m_hListView, 0);
		}break;
		case WM_MEASUREITEM:
		{
			MEASUREITEMSTRUCT* m = (MEASUREITEMSTRUCT*)lParam;
			m->itemHeight = 20;
		}break;
		case WM_SIZE:
		{
			RECT rtClientFrame;
			::GetClientRect(hWndFrame, &rtClientFrame);

			switch (wParam)
			{
			case SIZE_MAXIMIZED:
			{
				::MoveWindow(m_hWnd, 0, 0, rtClientFrame.right - rtClientFrame.left, rtClientFrame.bottom - rtClientFrame.top, TRUE);
			}break;
			case SIZE_RESTORED:
				[[fallthrough]];
			case SIZE_MINIMIZED:
				[[fallthrough]];
			case SIZE_MAXSHOW:
				[[fallthrough]];
			case SIZE_MAXHIDE:
			{

			}break;
			}

			if (m_hListView)
			{
				::MoveWindow(m_hListView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
				//ListView_SetColumnWidth(m_hListView, 1, (LONG)LOWORD(lParam) - (LONG)190);
			}

			return LRESULT(0);
		}break;
		case WM_GETMINMAXINFO:
		{
#if 1
			RECT rtFrame;
			::GetClientRect(hWndFrame, &rtFrame);

			MINMAXINFO* mminfo = (PMINMAXINFO)lParam;
			mminfo->ptMinTrackSize.x = 400;
			mminfo->ptMinTrackSize.y = 400;
			mminfo->ptMaxSize.x = rtFrame.right - rtFrame.left;
			mminfo->ptMaxSize.y = rtFrame.bottom - rtFrame.top;
			mminfo->ptMaxPosition.x = 0;
			mminfo->ptMaxPosition.y = 0;
			mminfo->ptMaxTrackSize.x = mminfo->ptMaxSize.x;
			mminfo->ptMaxTrackSize.y = mminfo->ptMaxSize.y;

			return LRESULT(0);
#endif
		}break;
		case WM_QUERYENDSESSION:
			[[fallthrough]];
		case WM_CLOSE:
		{

		}break;
		}///switch
		return LRESULT(0);
	}

	void ChildWndOnline::ProcessT()
	{
		time_t TimeIntervalRefreshPrev = 0;
		const time_t TimeIntervalRefresh = 5000;

		do {

			time_t TimeNow = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			if (TimeIntervalRefreshPrev <= 0)
				TimeIntervalRefreshPrev = TimeNow;

			if (m_hListView)
			{
				if (TimeNow - TimeIntervalRefreshPrev >= TimeIntervalRefresh)
				{
					TimeIntervalRefreshPrev = TimeNow;
					DWORD_PTR dwResult = 0;

					::SendMessageTimeout(m_hListView, LVM_DELETEALLITEMS, 0, 0L, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);

					if (Global::GetCore() && Global::GetCore()->GetSystem() && Global::GetCore()->GetSystem()->GetServer())
					{
						auto mapSerivce = Global::GetCore()->GetSystem()->GetServer()->m_ServiceObjQ.src();
						if (!mapSerivce || mapSerivce->empty())
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(100));
							if (!m_IsOpenT.load())
								break;
							continue;
						}
						Global::GetCore()->GetSystem()->GetServer()->m_ServiceObjQ.iterate(
							[&](const auto& login, IServiceObj* pServiceObj)
							{
								DWORD_PTR dwResult = 0;
								::SendMessageTimeout(m_hListView, LVFF_ITEMCOUNT, 0, 0L, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);

								auto pLogin = Global::GetSetup()->m_ClientsMapQ.search(login);

								LV_ITEM _lvi = { 0 };
								_lvi.mask = LVIF_TEXT;
								_lvi.iItem = static_cast<decltype(_lvi.iItem)>(dwResult);
								::SendMessageTimeout(m_hListView, LVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
								if (dwResult != -1)
								{
									sk::string temp = sk::Log::Format(L"{:X}", login >> 4 * 8);
									_lvi.iSubItem = 0;
									_lvi.pszText = LPWSTR(temp.c_str());
									::SendMessageTimeout(m_hListView, LVM_SETITEM, 0, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);

									temp = sk::Log::Format(L"{}", sk::StringConvert::MBytesToWString(pServiceObj->Address()));
									_lvi.iSubItem = 1;
									_lvi.pszText = LPWSTR(temp.c_str());
									::SendMessageTimeout(m_hListView, LVM_SETITEM, 1, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);

									sk::string strActiveTime = sk::StringConvert::MBytesToWString(sk::time::Time::TimestampToTimeStr(pServiceObj->ActiveTime(), true));
									_lvi.iSubItem = 2;
									_lvi.pszText = LPWSTR(strActiveTime.c_str());
									::SendMessageTimeout(m_hListView, LVM_SETITEM, 2, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);


									if (pLogin)
									{
										temp = sk::Log::Format(L"{}", sk::StringConvert::MBytesToWString(pLogin->Username));
										_lvi.iSubItem = 3;
										_lvi.pszText = LPWSTR(temp.c_str());
										::SendMessageTimeout(m_hListView, LVM_SETITEM, 3, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);


										temp = sk::Log::Format(L"{}", sk::StringConvert::MBytesToWString(pLogin->Identify));
										_lvi.iSubItem = 4;
										_lvi.pszText = LPWSTR(temp.c_str());
										::SendMessageTimeout(m_hListView, LVM_SETITEM, 4, reinterpret_cast<LPARAM>(&_lvi), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
									}
								}
							});



					}

				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!m_IsOpenT.load())
				break;
		} while (1);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FrameWnd::FrameWnd(HINSTANCE hInstance, HINSTANCE hInstanceRes, sk::string AppName, sk::string ClassName) :
		sk::IWindowFrame(hInstance, hInstanceRes, AppName, ClassName)
	{
		//m_pServerPipe = new ServerPipe();
	}

	FrameWnd::~FrameWnd()
	{
		//SK_DELETE_PTR(m_pServerPipe);
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
			int	nStatusPartsWidths[4];
			int nCnt = 0;
			int i;
			for (i = 4 - 1; i >= 0; i--)
				nStatusPartsWidths[nCnt++] = (rtMainWnd.right - rtMainWnd.left) - (300 * i);
			SendMessage(m_hStatusBar, SB_SETPARTS, ARRAYSIZE(nStatusPartsWidths), (LPARAM)nStatusPartsWidths);

			auto pChildLog = new ChildWndLog(ChildID(), m_hInstance, m_hInstanceRes, TEXT("Server Log"));
			AppendChild(pChildLog);
			//auto pChildOnline = new ChildWndOnline(ChildID(), m_hInstance, m_hInstanceRes, TEXT("Service Object Online"));
			//AppendChild(pChildOnline);

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
				GetGlobal()->GetCore()->Open();
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
				if (Global::GetCore() && Global::GetCore()->GetSystem() && Global::GetCore()->OpenIs())
				{
					if (IDOK != MessageBox(hWnd, _T("The current system is running. Are you sure to continue <Initialize> ?"), _T("Tip"), MB_ICONQUESTION | MB_OKCANCEL))
						break;
					Global::GetCore()->GetSystem()->ExecSystemProductsInit();
				}
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
				if (Global::GetCore() && Global::GetCore()->GetSystem())
				{
					Global::GetCore()->GetSystem()->ShowLPProcess();
				}
			}break;
			}///switch
		}break;
		case WM_NOTIFY:
		{

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
			proxy::GetGlobal()->GetCore()->Close();
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
					::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					if (!((dwStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST))
					{
						::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}
				}
				else
				{
					::ShowWindow(hWnd, SW_SHOWNORMAL);
					::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
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
		auto pFrameWnd = reinterpret_cast<FrameWnd*>(coreptr);
		if (pFrameWnd->m_ServerStatusUpdateTimeinterval == interval && pFrameWnd->m_hStatusBar)
		{
			DWORD_PTR dwResult = 0;
			::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)(sk::string(TEXT("状态: ")) + ServerStatusStr(pFrameWnd->ServerStatus())).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
			if (Global::GetCore() && Global::GetCore()->GetSystem() && Global::GetCore()->GetSystem()->GetServer() && Global::GetCore()->GetSystem()->GetServer()->NetApi<sk::network::INetworkApi>())
			{
				::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)(TEXT("连接数: ") + tostring(Global::GetCore()->GetSystem()->GetServer()->NetApi<sk::network::INetworkApi>()->Connection())).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
				::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)(TEXT("开放端口: ") + tostring(Global::GetCore()->GetSystem()->GetServer()->NetApi<sk::network::INetworkApi>()->ServerPort())).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
				if (Global::GetCore()->GetSystem()->GetServer()->NetApi<sk::network::INetworkApi>()->ServerPort() > 0)
				{
					pFrameWnd->ServerStatus(EnServerStatus::EN_SERVER_STATUS_READY);
				}
				else
				{
					pFrameWnd->ServerStatus(EnServerStatus::EN_SERVER_STATUS_STOP);
				}
			}
			else
			{
				::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)(TEXT("开放端口: ") + tostring(0)).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
				::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)(TEXT("连接数: ") + tostring(0)).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
			}

			::SendMessageTimeout(pFrameWnd->m_hStatusBar, SB_SETTEXT, MAKEWORD(3, 0), (LPARAM)(TEXT("系统版本 : ") + sk::Helper::GetProgramVersion(sk::Helper::GetCurrentProcessName())).c_str(), SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &dwResult);
		}

		return 0;
	}
}///namespace proxy