#include "stdafx.h"
#include "UIClient.h"
#include "UIProduct.h"
#include "UIFollowEdit.h"

namespace client {

	CControlUI* UIClient::CreateControl(LPCTSTR pstrClassName)
	{

		return nullptr;
	}

	LRESULT UIClient::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UINT iKey = (UINT)wParam;
		if (iKey == VK_RETURN)
		{
		}
		bHandled = true;
		return 0;
	}

	LRESULT UIClient::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		auto mminfo = (MINMAXINFO*)lParam;
		mminfo->ptMaxPosition.x = 0;
		mminfo->ptMaxPosition.y = 0;
		auto width = ::GetSystemMetrics(SM_CXSCREEN);
		auto hegth = ::GetSystemMetrics(SM_CYSCREEN);

		APPBARDATA ad;
		ZeroMemory(&ad, sizeof APPBARDATA);
		ad.cbSize = sizeof APPBARDATA;
		::SHAppBarMessage(ABM_GETTASKBARPOS, &ad);
		switch (ad.uEdge) {
		case 0:
		case 2: {
			mminfo->ptMaxSize.x = width - (ad.rc.right - ad.rc.left);
			mminfo->ptMaxSize.y = hegth - 0;
		}break;
		case 1:
		case 3: {
			mminfo->ptMaxSize.x = width - 0;
			mminfo->ptMaxSize.y = hegth - (ad.rc.bottom - ad.rc.top);
		}break;
		}

		bHandled = false;
		return 0;
	}


	LRESULT UIClient::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		if (wParam == SC_CLOSE)
		{
			OnUIClose(nullptr);
			bHandled = TRUE;
		}
		bHandled = FALSE;
		return 0L;
	}

	LRESULT UIClient::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	void UIClient::InitWindow()
	{
		auto pTitleUI = GetCtrl<CLabelUI>(_TEXT("DEF_E03A614697B57FA8FE7BE7FE6EAF5537"));
		if (pTitleUI)
		{
			auto version = sk::Helper::GetProgramVersionA(sk::Helper::GetCurrentProcessNameA());
			pTitleUI->SetText(sk::Log::Format(_TEXT("跟单系统管理器® [{}]"), sk::StringConvert::MBytesToTString(version)).c_str());
		}

		::SetWindowPos(GetHWND(), nullptr, 0, 0, 500, 350, SWP_NOACTIVATE);
		CenterWindow();
		GetCtrl<CTreeViewUI>(_TEXT("DEF_252C209091A2AC3D370447CFC125F4E3"))->SelectItem(0);
	}
























	void UIClient::OnUILogon(CControlUI*)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_100000070,
					sk::network::EnNetCmd::EN_NETCMD_100000070),
				[&]()->int
				{
					return NetApi<sk::network::INetworkApi>()->Start(this);
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (result && SKAPIRES_SUCCESS(head.Retcode) && head.Retcode)
					{
						MINMAXINFO MMI = { 0 };
						::SendMessage(GetHWND(), WM_GETMINMAXINFO, 0, (LPARAM)&MMI);
						::SetWindowPos(GetHWND(), HWND_TOP, 0, 0, MMI.ptMaxSize.x, MMI.ptMaxSize.y, SWP_NOACTIVATE);
						CenterWindow();
						GetCtrl<CTabLayoutUI>(_T("DEF_A70C424EF50CEE83AB773256B8C50D41"))->SelectItem(1);

						//NetFollowRecordRequet();
						NetFollowConsRequest();
						NetServerTimeRequest();
						NetSystemTradeRecordHead();
					}
					else
					{
						switch (SKAPIRES(head.Retcode))
						{
						case SKAPIRES::FOLLOW_RET_ERROR_LOGIN_PASSWORD:
						{
							MsgBox(GetHWND(), _TEXT("登录到服务器失败(不正确的密码)!"), _TEXT("错误"), MB_TOPMOST);
						}break;
						case SKAPIRES::FOLLOW_RET_ERROR_LOGIN_REPEAT:
						{
							MsgBox(GetHWND(), _TEXT("登录到服务器失败(该帐户已经登录)!"), _TEXT("错误"), MB_TOPMOST);
						}break;
						default:
						{
							MsgBox(GetHWND(), _TEXT("登录到服务器失败(与服务连接异常)!"), _TEXT("错误"), MB_TOPMOST);
						}break;
						}///switch
						NetApi<sk::network::INetworkApi>()->Stop();
					}
				}, 10000));
	}
	void UIClient::OnMinimized(CControlUI*)
	{
		::ShowWindow(GetHWND(), SW_SHOWMINIMIZED);
	}
	void UIClient::OnUIClose(CControlUI*)
	{
		NetApi<sk::network::INetworkApi>()->Stop();
		UIFrame::Close(IDOK);
		Global::SystemExit();
	}
	void UIClient::OnUILogout(CControlUI* pCtrlUI)
	{
		NetApi<sk::network::INetworkApi>()->Stop();

		::SetWindowPos(GetHWND(), nullptr, 0, 0, 500, 350, SWP_NOACTIVATE);
		CenterWindow();

		GetCtrl<CTabLayoutUI>(_TEXT("DEF_A70C424EF50CEE83AB773256B8C50D41"))->SelectItem(0);

		{//! 清理UI控件相关数据
			auto pList = GetCtrl<CListUI>(_TEXT("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
			if (pList) {
				pList->RemoveAll();
			}
		}

	}
	void UIClient::OnUIFollowRecordInitialize(CControlUI* pCtrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"));
		if (!pList)
			return;
		pList->RemoveAll();
		Global::SharedGet()->m_FollowLogQ.iterate(
			[&](const auto& timeUpdate, shared::FollowLogField& logField)
			{
				pList->SetTagUINT64(timeUpdate);
				auto pListNode = new CListTextElementUI();
				pList->Add(pListNode);
				{
					//!@ 交易时间
					pListNode->SetText(0 * 2, sk::StringConvert::MBytesToWString(sk::time::Time::TimestampToTimeStr(logField.TimeUpdate)).c_str());
					//!@ 任务类型
					pListNode->SetText(1 * 2, logField.ProcessTaskTypeStr());
					//!@ 跟单帐号
					pListNode->SetText(2 * 2, tostring(logField.FollowObj.Login).c_str());
					//!@ 信号帐号
					pListNode->SetText(3 * 2, tostring(logField.SignalObj.Login).c_str());
					//!@ 信号单号
					pListNode->SetText(4 * 2, tostring(logField.SignalObj.Position).c_str());
					//!@ 跟随单号
					pListNode->SetText(5 * 2, tostring(logField.FollowObj.Position).c_str());
					//!@ 信号品种
					pListNode->SetText(6 * 2, logField.SignalObj.Symbol);
					//!@ 跟随品种
					pListNode->SetText(7 * 2, logField.FollowObj.Symbol);
					//!@ 多空方向
					pListNode->SetText(8 * 2, shared::Business::DealActionStr(logField.FollowObj.Action));
					//!@ 开平方向
					pListNode->SetText(9 * 2, shared::Business::DealEntryStr(logField.FollowObj.Entry));
					//!@ 成交价格
					pListNode->SetText(10 * 2, sk::Helper::FloatToStr(logField.FollowObj.Price, 5).c_str());
					//!@ 成交数量
					pListNode->SetText(11 * 2, tostring(logField.FollowObj.Volume).c_str());
					//!@ 止损价格
					pListNode->SetText(12 * 2, sk::Helper::FloatToStr(logField.FollowObj.PriceSL, 5).c_str());
					//!@ 止盈价格
					pListNode->SetText(13 * 2, sk::Helper::FloatToStr(logField.FollowObj.PriceTP, 5).c_str());
					//!@ 完成状态
					pListNode->SetText(14 * 2, logField.ProcessResultStr());
					//!@ 失败原因
					pListNode->SetText(15 * 2, sk::StringConvert::MBytesToWString(logField.Reason).c_str());
				}

			});

		if (pList->GetCount()) {
			pList->SelectItem(0);
		}
	}
	void UIClient::OnUIFollowRecordClear(CControlUI* pCtrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"));
		if (!pList)
			return;
		pList->RemoveAll();
	}

	void UIClient::OnUIPlatfomSwitchFollow(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000031),
				[&]()->int
				{
					return Global::GetCore()->GetClient()->NetQuickFunctionSwitchFollow();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (SKAPIRES_FAILED(head.Retcode))
					{
						MsgBox(GetHWND(), _TEXT("命令发送失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("命令发送完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}

	void UIClient::OnUIPlatfomSwitchSLTP(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000033),
				[&]()->int
				{
					return Global::GetCore()->GetClient()->NetQuickFunctionSwitchSLTP();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (SKAPIRES_FAILED(head.Retcode))
					{
						MsgBox(GetHWND(), _TEXT("命令发送失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("命令发送完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}

	void UIClient::OnUIPlatfomSwitchPatch(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000032),
				[&]()->int
				{
					return Global::GetCore()->GetClient()->NetQuickFunctionSwitchPatch();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (SKAPIRES_FAILED(head.Retcode))
					{
						MsgBox(GetHWND(), _TEXT("命令发送失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("命令发送完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}

	void UIClient::OnUIPlatfomPatchPosition(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000510),
				[&]()->int
				{
					return Global::GetCore()->GetClient()->NetPlatfomPatchPositionRequest();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (SKAPIRES_FAILED(head.Retcode))
					{
						MsgBox(GetHWND(), _TEXT("命令发送失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("命令发送完成,请到对应平台查看执行结果!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 20000));
	}
	void UIClient::OnUIFollowRecordUpdate(CControlUI* pCtrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"));
		if (!pList) 
			return;
		auto updates = \
			Global::SharedGet()->m_FollowLogQ.search(
				[&](const shared::FollowLogField& logField)->bool
				{
					if (logField.TimeUpdate > (INT64)pList->GetTagUINT64())
					{
						return true;
					}
					return false;
				});

		if (updates)
		{
			for (const auto& logField : *updates)
			{
				pList->SetTagUINT64(logField.TimeUpdate);
				auto pListNode = new CListTextElementUI();
				pList->AddAt(pListNode, 0);
				{
					//!@ 交易时间
					pListNode->SetText(0 * 2, sk::StringConvert::MBytesToWString(sk::time::Time::TimestampToTimeStr(logField.TimeUpdate)).c_str());
					//!@ 任务类型
					pListNode->SetText(1 * 2, logField.ProcessTaskTypeStr());
					//!@ 信号帐号
					pListNode->SetText(2 * 2, tostring(logField.FollowObj.Login).c_str());
					//!@ 信号帐号
					pListNode->SetText(3 * 2, tostring(logField.SignalObj.Login).c_str());
					//!@ 信号单号
					pListNode->SetText(4 * 2, tostring(logField.SignalObj.Position).c_str());
					//!@ 跟随单号
					pListNode->SetText(5 * 2, tostring(logField.FollowObj.Position).c_str());
					//!@ 信号品种
					pListNode->SetText(6 * 2, logField.SignalObj.Symbol);
					//!@ 跟随品种
					pListNode->SetText(7 * 2, logField.FollowObj.Symbol);
					//!@ 多空方向
					pListNode->SetText(8 * 2, shared::Business::DealActionStr(logField.FollowObj.Action));
					//!@ 开平方向
					pListNode->SetText(9 * 2, shared::Business::DealEntryStr(logField.FollowObj.Entry));
					//!@ 成交价格
					pListNode->SetText(10 * 2, sk::Helper::FloatToStr(logField.FollowObj.Price, 5).c_str());
					//!@ 成交数量
					pListNode->SetText(11 * 2, tostring(logField.FollowObj.Volume).c_str());
					//!@ 止损价格
					pListNode->SetText(12 * 2, sk::Helper::FloatToStr(logField.FollowObj.PriceSL, 5).c_str());
					//!@ 止盈价格
					pListNode->SetText(13 * 2, sk::Helper::FloatToStr(logField.FollowObj.PriceTP, 5).c_str());
					//!@ 完成状态
					pListNode->SetText(14 * 2, logField.ProcessResultStr());
					//!@ 失败原因
					pListNode->SetText(15 * 2, sk::StringConvert::MBytesToWString(logField.Reason).c_str());
				}
			}
		}

		if (pList->GetCount()) {
			pList->SelectItem(0);
		}
	}
	void UIClient::OnUIFollowConsDelete(CControlUI* pCtrl)
	{
		auto pListEdit = GetCtrl<IFollowEditUI>(_T("DEF_770A32C71E22FBBDD2C8F330C683FD55"));
		if (!pListEdit)
			return;
		auto pak = pListEdit->Packet();
		if (pak.empty())
			return;
		auto pFollow = (shared::FollowField*)pak.data();
		if (IDYES == \
			MessageBox(NULL, sk::Log::Format(_TEXT("确认删除跟单配置({})?"), sk::StringConvert::MBytesToTString(pFollow->GetKey().Get())).c_str(), NULL, MB_TOPMOST | MB_YESNO))
		{
			Global::SharedGet()->m_NetSync.Request(
				shared::NetSyncSession(
					shared::NetSyncKey(
						sk::network::EnNetCmd::EN_NETCMD_000000000,
						sk::network::EnNetCmd::EN_NETCMD_200000130),
					[&]()->int
					{
						return Global::GetCore()->GetClient()->NetFollowConsDeleteRequest(sk::packet((char*)&pFollow->GetKey(), sizeof(shared::FollowKey)));
					},
					[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
					{
						if (!result || pak.empty() || SKAPIRES_FAILED(head.Retcode))
						{
							MsgBox(GetHWND(), _TEXT("删除跟单配置失败!"), _TEXT("错误"), MB_TOPMOST);
						}
						else
						{
							MsgBox(GetHWND(), _TEXT("删除跟单配置完成!"), _TEXT("提示"), MB_TOPMOST);
						}
					}, 10000));
		}
	}
	void UIClient::OnUIFollowConsCreate(CControlUI* pCtrl)
	{
		auto pListEdit = GetCtrl<IFollowEditUI>(_T("DEF_770A32C71E22FBBDD2C8F330C683FD55"));
		if (!pListEdit) return;
		auto followconPak = pListEdit->Packet();
		if (!followconPak.empty())
		{
			//!@ 配置检测
			auto pFollowcon = (shared::FollowField*)followconPak.data();

			if (!Global::SharedGet()->m_MTUserQ.search(pFollowcon->LoginSignal, [&](const auto&, auto&) {}))
			{
				MsgBox(GetHWND(), _TEXT("信号帐户不存在!"), _TEXT("错误"), MB_TOPMOST);
				return;
			}

			if (!Global::SharedGet()->m_MTUserQ.search(pFollowcon->LoginFollow, [&](const auto&, auto&) {}))
			{
				MsgBox(GetHWND(), _TEXT("跟单帐户不存在!"), _TEXT("错误"), MB_TOPMOST);
				return;
			}

			if (pFollowcon->Verify() && SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS == pFollowcon->Check())
			{
				auto msgText = sk::Log::Format("确认[创建/更新]跟单关系({})?", pFollowcon->GetKey().Get());
				if (IDYES == MessageBoxA(NULL, msgText.c_str(), NULL, MB_TOPMOST | MB_YESNO))
				{
					Global::SharedGet()->m_NetSync.Request(
						shared::NetSyncSession(
							shared::NetSyncKey(
								sk::network::EnNetCmd::EN_NETCMD_000000000,
								sk::network::EnNetCmd::EN_NETCMD_200000120),
							[&]()->int
							{
								return NetFollowConsUpdateRequest(followconPak);
							},
							[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
							{
								if (!result || pak.empty() || SKAPIRES_FAILED(head.Retcode))
								{									
									MsgBox(GetHWND(), _TEXT("创建跟单配置失败!"), _TEXT("错误"), MB_TOPMOST);
								}
								else
								{
									MsgBox(GetHWND(), _TEXT("创建跟单配置完成!"), _TEXT("提示"), MB_TOPMOST);
								}
							}, 10000));

				}
			}
			else
			{
				MsgBox(GetHWND(), _TEXT("不合法的跟单配置!"), _TEXT("错误"), MB_TOPMOST);
				return;
			}
		}
	}

	void UIClient::OnUIPlaformPatchPositions(const SKAPIRES& result)
	{
		if (SKAPIRES_SUCCESS(result))
		{
			MsgBox(GetHWND(), _T("命令被服务器接受!\n请在相应平台查看执行结果。"), _T("提示"), MB_OK);
		}
		else
		{
			switch (result)
			{
			case SKAPIRES::FOLLOW_RET_ERROR_SYSTEM_BUSY:
			{
				MsgBox(GetHWND(), _T("系统任务繁忙，请稍后再试！"), _T("提示"), MB_OK);
			}break;
			case SKAPIRES::FOLLOW_RET_ERROR_FOLLOWCON_EMPTY:
			{
				MsgBox(GetHWND(), _T("没有找到跟单配置信息！"), _T("错误"), MB_OK);
			}break;
			default:
			{
				MsgBox(GetHWND(), _T("系统错误！"), _T("错误"), MB_OK);
			}break;
			}///switch
		}
	}
	void UIClient::OnUISystemTradeRecordHead(const sk::packet& headStr)
	{
		if (headStr.empty()) { return; }
		auto pList = GetCtrl<CListUI>(_TEXT("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"));
		if (!pList) return;
		pList->RemoveAll();
		std::vector<sk::packet> heads;
		sk::Helper::OneUnknownParseA(headStr, '\t', heads);
		if (heads.empty()) return;
		CListHeaderUI* pHeader = pList->GetHeader();
		if (!pHeader) return;
		if (pHeader->GetCount() <= 0)
		{
			//!@ "时间", "任务类型", "帐户", "数量", "价格", "持仓", "品种", "多空", "开平", "SL", "TP", "信号源", "信号持仓", "信号SL", "信号TP", "运行时间", "返回代码", "原因"
			pHeader->SetFixedHeight(20);
			pHeader->SetBkColor(0xFF1E1E1E);
			pHeader->SetBorderSize(1);
			pHeader->SetBorderColor(0xFF3C3C3C);
			int index = 0;
			for (const auto& str : heads)
			{
				CListHeaderItemUI* pHeaderItem = new CListHeaderItemUI;
				pHeaderItem->SetText(sk::StringConvert::MBytesToWString(str).c_str());
				pHeaderItem->SetTextColor(0xFFFFFFFF);

				switch (index)
				{
				case 0://!时间
				{
					pHeaderItem->SetFixedWidth(180);
				}break;
				case 1://!任务类型
					[[fallthrough]];
				case 2://!帐户
					[[fallthrough]];
				case 3://!数量
					[[fallthrough]];
				case 4://!价格
					[[fallthrough]];
				case 5://!持仓
					[[fallthrough]];
				case 6://!品种
					[[fallthrough]];
				case 7://!多空
					[[fallthrough]];
				case 8://!开平
					[[fallthrough]];
				case 9://!SL
					[[fallthrough]];
				case 10://!TP
					[[fallthrough]];
				case 11://!信号源
					[[fallthrough]];
				case 12://!信号持仓
					[[fallthrough]];
				case 13://!信号SL
					[[fallthrough]];
				case 14://!信号TP
					[[fallthrough]];
				case 15://!运行时间
					[[fallthrough]];
				case 16://!返回代码
				{
					pHeaderItem->SetFixedWidth(70);
				}break;
				case 17://!原因
				{
					pHeaderItem->SetFixedWidth(200);
				}break;
				default:
				{
					pHeaderItem->SetFixedWidth(100);
				}break;
				}///switch
				pList->Add(pHeaderItem);
				++index;
			}
			NetSystemTradeRecordUpdate();
		}
	}
	void UIClient::OnUIServerTimeUpdate(const time_t& server_time)
	{
		auto pTargetUI = GetCtrl<CEditUI>(_TEXT("DEF_FC5908D5D9AC711A438DE33A445AE75E"));
		if (!pTargetUI|| server_time<=0)
			return;
		pTargetUI->SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimestampToTimeStr(server_time)).c_str());
	}
	void UIClient::OnUIServerTimeRefresh(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000610),
				[&]()->int
				{
					return NetServerTimeRequest();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (result && !pak.empty())
					{
						OnUIServerTimeUpdate(*(time_t*)pak.data());
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("请求服务器时间失败!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUIFollowRecordRefresh(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000410),
				[&]()->int
				{
					return NetFollowRecordRequet();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (result && !pak.empty())
					{
						MsgBox(GetHWND(), _TEXT("跟单记录刷新请求完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUIFollowConsRefresh(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000110),
				[&]()->int
				{
					return NetFollowConsRequest();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (result && !pak.empty())
					{
						OnUIFollowConsUpdate();
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("没有找到跟单配置!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}

	void UIClient::OnUIFollowConsUpdate(CControlUI* pCtrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
		if (!pList) return;
		pList->RemoveAll();

		std::multimap<time_t, shared::FollowField, std::greater<time_t>> sort;
		CDialogBuilder builder;
		Global::SharedGet()->m_FollowFieldQ.iterate(
			[&](const auto& followKey, auto& followObj) {
				sort.insert(std::make_pair(followObj.TimeUpdate, followObj));
			});
		for (auto it = sort.begin(); it != sort.end(); ++it)
		{
			auto pListLine = (IFollowTreeNodeUI*)builder.Create(_T("UIFollowTreeNode.xml"));
			pList->Add(pListLine);
			pListLine->Set(it->second);
			pListLine->SetUserData(sk::StringConvert::MBytesToWString(it->second.GetKey().Get()).c_str());
		}
		GetCtrl<CButtonUI>(_T("DEF_AA940C6CDB6BEFC36FB6D931F852BCE8"))->SetEnabled(true);

		if (pList->GetCount())
		{
			pList->SelectItem(0);
		}
	}

	void UIClient::UISystemSetupSet()
	{
		auto& SystemSetup = Global::SetupGet()->SystemSetup();

		//!@ 系统开关
		auto comboSystemSwitch = GetCtrl<CComboUI>(_T("DEF_525F750119D4369C3CC0CB23614783FB"));
		if (comboSystemSwitch)
		{
			for (int i = 0; i < comboSystemSwitch->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.SystemSwitch == \
					_tcstol(comboSystemSwitch->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboSystemSwitch->SelectItem(i);
					break;
				}
			}
		}

		//!@ 自动跟漏平开关
		auto comboAutoFollowPositionCloseSwitch = GetCtrl<CComboUI>(_T("DEF_F7072A66618D6F9440D8F3B14DEC3AF3"));
		if (comboAutoFollowPositionCloseSwitch)
		{
			for (int i = 0; i < comboAutoFollowPositionCloseSwitch->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.AutoPositionCloseSwitch == \
					_tcstol(comboAutoFollowPositionCloseSwitch->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboAutoFollowPositionCloseSwitch->SelectItem(i);
					break;
				}
			}
		}

		//!@ 漏平检测时间间隔
		auto comboTimeAutoPositionCloseCheck = GetCtrl<CComboUI>(_T("DEF_78F4500AC347454745F26BCF801D9ED1"));
		if (comboTimeAutoPositionCloseCheck)
		{
			for (int i = 0; i < comboTimeAutoPositionCloseCheck->GetCount(); ++i)
			{
				if (SystemSetup.TimeAutoPositionCloseCheck == \
					_tcstoll(comboTimeAutoPositionCloseCheck->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboTimeAutoPositionCloseCheck->SelectItem(i);
					break;
				}
			}
		}

		//!@ 补正SLTP
		auto comboCorrectionSLTPSwitch = GetCtrl<CComboUI>(_T("DEF_44F4F7666CA819A5035B9991760FA43A"));
		if (comboCorrectionSLTPSwitch)
		{
			for (int i = 0; i < comboCorrectionSLTPSwitch->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.CorrectionSLTPSwitch == \
					_tcstol(comboCorrectionSLTPSwitch->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboCorrectionSLTPSwitch->SelectItem(i);
					break;
				}
			}
		}

		//!@ 补正SLTP时间间隔
		auto comboTimeSLTPCorrectionInterval = GetCtrl<CComboUI>(_T("DEF_6C6F9EC80A8E862B187D96A89E78A57A"));
		if (comboTimeSLTPCorrectionInterval)
		{
			for (int i = 0; i < comboTimeSLTPCorrectionInterval->GetCount(); ++i)
			{
				if (SystemSetup.TimeSLTPCorrectionInterval == \
					_tcstoll(comboTimeSLTPCorrectionInterval->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboTimeSLTPCorrectionInterval->SelectItem(i);
					break;
				}
			}
		}

		//!@ 强跟选项
		auto comboFollowInPatchAction = GetCtrl<CComboUI>(_T("DEF_99A5A76BFDAF477B5A5A0500CFD9F0B1"));
		if (comboFollowInPatchAction)
		{
			for (int i = 0; i < comboFollowInPatchAction->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.PatchInAction == \
					_tcstol(comboFollowInPatchAction->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboFollowInPatchAction->SelectItem(i);
					break;
				}
			}
		}

	}
	void UIClient::UISystemSetupGet()
	{
		shared::FollowSystemSetup SystemSetup;

		//! 系统开关
		auto comboSystemSwitch = GetCtrl<CComboUI>(_T("DEF_525F750119D4369C3CC0CB23614783FB"));
		if (comboSystemSwitch)
		{
			SystemSetup.SystemSwitch = decltype(SystemSetup.SystemSwitch)(_tcstol(comboSystemSwitch->GetItemAt(comboSystemSwitch->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 自动跟漏平开关
		auto comboAutoPatchInSwitch = GetCtrl<CComboUI>(_T("DEF_F7072A66618D6F9440D8F3B14DEC3AF3"));
		if (comboAutoPatchInSwitch)
		{
			SystemSetup.AutoPositionCloseSwitch = decltype(SystemSetup.AutoPositionCloseSwitch)(_tcstol(comboAutoPatchInSwitch->GetItemAt(comboAutoPatchInSwitch->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 补正SLTP时间间隔
		auto comboTimeSLTPCorrectionInterval = GetCtrl<CComboUI>(_T("DEF_78F4500AC347454745F26BCF801D9ED1"));
		if (comboTimeSLTPCorrectionInterval)
		{
			SystemSetup.TimeSLTPCorrectionInterval =\
				decltype(SystemSetup.TimeSLTPCorrectionInterval)(_tcstoll(comboTimeSLTPCorrectionInterval->GetItemAt(comboTimeSLTPCorrectionInterval->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 补正SLTP开关
		auto comboCorrectionSLTPSwitch = GetCtrl<CComboUI>(_T("DEF_44F4F7666CA819A5035B9991760FA43A"));
		if (comboCorrectionSLTPSwitch)
		{
			SystemSetup.CorrectionSLTPSwitch = decltype(SystemSetup.CorrectionSLTPSwitch)(_tcstol(comboCorrectionSLTPSwitch->GetItemAt(comboCorrectionSLTPSwitch->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 漏平检测时间间隔
		auto comboTimeCheckAutoPatchInSwitch = GetCtrl<CComboUI>(_T("DEF_78F4500AC347454745F26BCF801D9ED1"));
		if (comboTimeCheckAutoPatchInSwitch)
		{
			SystemSetup.TimeAutoPositionCloseCheck = \
				decltype(SystemSetup.TimeAutoPositionCloseCheck)(_tcstoll(comboTimeCheckAutoPatchInSwitch->GetItemAt(comboTimeCheckAutoPatchInSwitch->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 强跟选项
		auto comboFollowPatchAction = GetCtrl<CComboUI>(_T("DEF_99A5A76BFDAF477B5A5A0500CFD9F0B1"));
		if (comboFollowPatchAction)
		{
			SystemSetup.PatchInAction = decltype(SystemSetup.PatchInAction)(_tcstol(comboFollowPatchAction->GetItemAt(comboFollowPatchAction->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_200000020),
				[&]()->int
				{
					return NetSystemSetupSend(SystemSetup);
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (!result)
					{
						MsgBox(GetHWND(), _TEXT("保存配置失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("保存配置完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 5000));

	}




	void UIClient::Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_HEADERCLICK)
		{
			OnUIClickListHead(msg.pSender);
		}
		else if (msg.sType == DUI_MSGTYPE_CLICK) {
			if (msg.pSender->GetName() == _TEXT("DEF_5367D1B574F2DA1A8BE2CC5A709DA388")) //! 最小化
			{
				OnMinimized(msg.pSender);
			}
			else if (msg.pSender->GetName() == _TEXT("DEF_C63FEB4ADE18128AE1C765D021677F64")) //! 关闭系统
			{
				OnUIClose(msg.pSender);
			}
			else if (msg.pSender->GetName() == _TEXT("DEF_37C0AEB747179B19C2ABC7320C215DD9")) //! 退出登录
			{
				OnUILogout(msg.pSender);
			}
			else if (msg.pSender->GetName() == _TEXT("DEF_95DCE79DA85B064019873074CC016F9D")) //! 登录
			{
				OnUILogon(msg.pSender);
			}
			else if (msg.pSender->GetName() == _TEXT("DEF_AA940C6CDB6BEFC36FB6D931F852BCE8")) //! 刷新跟单配置
			{
				OnUIFollowConsRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _TEXT("DEF_B4E29CD7BB8744ADF5238569204977C8"))//! 刷新服务器时间
			{
				OnUIServerTimeRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_6FC26413C6EC784AB8BAE49778F47FE5")) //! Home
			{
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(0);
			}
			else if (msg.pSender->GetName() == _T("DEF_08E2CE8F82F40074B1FDA0E93806ACD5"))
			{
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(1);
			}
			else if (msg.pSender->GetName() == _T("DEF_6640BDF922CB6D00D69CE3191C1F91A6"))
			{
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(2);
			}
			else if (msg.pSender->GetName() == _T("DEF_0FBE882C96DC1392F01FECD6E2B80DDB"))
			{
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(3);
			}
			else if (msg.pSender->GetName() == _T("DEF_657D081C484CAFAAB1A6364E4997F51E")) //! 创建/更新 跟单配置		
			{
				OnUIFollowConsCreate(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_3B74158014290D57E0911D478516EE25")) //! 删除跟单配置
			{
				OnUIFollowConsDelete(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_3D50BF1FD9671B156B42C81CB1C2FDD0")) {//! 清空日志
				OnUIFollowRecordClear(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_0F81DF2107882AAF10370507C22C698E")) {//! 刷新日志
				OnUIFollowRecordRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_D80A8B35D645F759822FFD3323A4E142")) {//! 平台强跟
				OnUIPlatfomPatchPosition(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_1A4AF87E3C86E48D86C21564C2745DD2")) {//! 批量跟单开关
				OnUIPlatfomSwitchFollow(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_677FA9F19D6F1E87713EBB22B9C0456C")) {//! 批量强跟开关
				OnUIPlatfomSwitchPatch(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_C4B66F52E3DF7AFEB306E763AF60C317")) {//! 批量止盈开关
				OnUIPlatfomSwitchSLTP(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_7C67F08158C0C564E126485C5427E9BE")) {//! 保存修改系统配置
				UISystemSetupGet();
			}
		}///DUI_MSGTYPE_CLICK
		else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
			if (msg.pSender->GetName() == _T("DEF_5B0F87A37DD07AC785551865A46C6158")) {
				//OnSearchSymbol();
			}
#if 0
			//! 关于点差的输入限制 -- 只允许输入正整数
			if (msg.pSender->GetName() == _T("DEF_A7FC9AACE8DCF522C6D377BAFAFBD376") ||
				msg.pSender->GetName() == _T("DEF_2D2CF54FB062AF57E4CEC73B1699CBB0") ||
				msg.pSender->GetName() == _T("DEF_A4B1DD4431B12657EF423BA68D84CCBC") ||
				msg.pSender->GetName() == _T("DEF_DF3DB43DBACF9CB65DE9D5383FF5CC53") ||
				msg.pSender->GetName() == _T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9") ||
				msg.pSender->GetName() == _T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::IntegerlsFix(InputStr, USHRT_MAX, 0)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
#endif

			//! 帐户
			if (msg.pSender->GetName() == _T("DEF_3F7E9179BC7A289837CAEC89B009C958") ||
				msg.pSender->GetName() == _T("DEF_6D9D4D263116926A47CA77ACB833BF1C")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!InputStr.empty())
				{
					if (!sk::Helper::IsDestStrCh(_T("1234567890"), InputStr[InputStr.length() - 1])) {
						InputStr.pop_back();
						pEdit->SetText(InputStr.c_str());
						pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
					}
				}
			}
			//! 注释信息
			if (msg.pSender->GetName() == _T("DEF_59E8CCC844803415E9EBA48164F665A9")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::EditInputLimit(InputStr, ARRAYSIZE(shared::FollowField::IMTComment))) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! 品种后缀限定
			if (msg.pSender->GetName() == _T("DEF_9D3826902B29C4BF7EF9553D4ADCE2D2")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::SymbolsSuffixFix(InputStr, ARRAYSIZE(shared::FollowField::SymbolSuffix))) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! 品种'Symbol'输入限定
			if (msg.pSender->GetName() == _T("DEF_4A4E04C8E1C1B0D6034A66CD53BE7716") ||
				msg.pSender->GetName() == _T("DEF_649419F876DFEF711D1DE21B94FB50DD")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::EditInputLimit(InputStr, ARRAYSIZE(shared::FollowField::SymbolAllow))) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}

				if (!GetCtrl<CEditUI>(_T("DEF_649419F876DFEF711D1DE21B94FB50DD"))->GetText().Compare(_T("*")) &&
					!GetCtrl<CEditUI>(_T("DEF_4A4E04C8E1C1B0D6034A66CD53BE7716"))->GetText().Compare(_T("*"))
					)
				{
					InputStr[InputStr.length() - 1] = 0;
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}

			//! 次数、个数沟通限定
			if (msg.pSender->GetName() == _T("DEF_901ADEC2BB6996595388B780C65E29CA")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::DecimalsFix(InputStr, MAXINT, 0)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! 数量值沟通限定
			if (msg.pSender->GetName() == _T("DEF_7F97CD1778B9D4BE9EC8E23FEE336B48") ||
				msg.pSender->GetName() == _T("DEF_5C46AAB7E6E548870319607A28AAD2D0") ||
				msg.pSender->GetName() == _T("DEF_8EF6A21FB9FE3B15581F1E45B1D2F58F") ||
				msg.pSender->GetName() == _T("DEF_067173C7CFD5F93445FF7114B87CA045") ||
				msg.pSender->GetName() == _T("DEF_D35797EA888634519D504A4FEC272437")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::DecimalsFix(InputStr, MAXINT, 3)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! 比例沟通限定
			if (msg.pSender->GetName() == _T("DEF_F82FECD30755D5E09C234BF051BA8F3D")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::DecimalsFix(InputStr, 10001, 2)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! 价格（点差）沟通限定 PriceTP / PriceTP
			if (msg.pSender->GetName() == _T("DEF_06E21E889EAEFBEA650B08E6E2B8E11E") ||
				msg.pSender->GetName() == _T("DEF_3F47DDE2AD43F44A64354A551B06FA47")) {
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::IntegerlsFix(InputStr, USHRT_MAX, 0)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}

			}
			//! 时间段沟通限定
			if (msg.pSender->GetName() == _T("DEF_ED60DB5B2FFCAAFC31AEDD998003BFC4") ||
				msg.pSender->GetName() == _T("DEF_18AF32058A212DAC49DD0B3EAEA01B98") ||
				msg.pSender->GetName() == _T("DEF_6ACBBEB43583AB333EA7F78CDFC83898") ||
				msg.pSender->GetName() == _T("DEF_6D79808AAE45F4C6A0576A98A48F87F9") ||
				msg.pSender->GetName() == _T("DEF_B4743DD2747F626BEDFB383B963A9C87") ||
				msg.pSender->GetName() == _T("DEF_53B6D4A8B02AC82700F95ABCF8ED69B5")) {

				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::TimeFrameFix(InputStr)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}



		}///DUI_MSGTYPE_TEXTCHANGED
		else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
			if (msg.pSender->GetName() == _T("DEF_6071F6A479D14DE184CC6BE2394A941C")) {
				auto pCombo = (CComboUI*)msg.pSender;
				auto sel = pCombo->GetCurSel();
				if (sel < 0) return;
				auto ctrl = pCombo->GetItemAt(sel);
				if (!ctrl) return;
				auto seltext = ctrl->GetText();
				GetCtrl<CEditUI>(_T("DEF_5B0F87A37DD07AC785551865A46C6158"))->SetText(seltext);

				auto pList = GetCtrl<CListUI>(_T("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
				if (pList) {
					auto pListNode = (IProductTreeNodeUI*)pList->GetItemAtByTagUINT64(ctrl->GetTagUINT64());
					if (pListNode) {
						pList->SelectItem((int)pListNode->GetTag());
					}
				}
				//ThemeEdit(ctrl->GetTag());
			}
			else if (msg.pSender->GetName() == _T("DEF_9CD12D041D24ED8D64E170F9A3A17391")) {
				auto pListTree = (CListUI*)msg.pSender;
				auto nCurSel = pListTree->GetCurSel();
				if (nCurSel < 0) return;
				auto pSel = pListTree->GetItemAt(nCurSel);
				auto followcon = Global::SharedGet()->m_FollowFieldQ.search(sk::StringConvert::WStringToMBytes(pSel->GetUserData().GetData()).c_str());
				if (!followcon) return;
				auto pListEdit = GetCtrl<IFollowEditUI>(_T("DEF_770A32C71E22FBBDD2C8F330C683FD55"));
				if (!pListEdit) return;
				pListEdit->RemoveAll();
				pListEdit->SetCache(sk::stringa((char*)followcon.get(), shared::LENFOLLOWFIELD));
				pListEdit->Parse();
				pListEdit->NeedParentUpdate();
			}
			else if (msg.pSender->GetName() == _T("DEF_252C209091A2AC3D370447CFC125F4E3")) {//! 树控
				auto pTreeView = (CTreeViewUI*)msg.pSender;
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(pTreeView->GetCurSel());
			}

		}



	}


}///namespace client