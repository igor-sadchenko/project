#include "stdafx.h"
#include "UIClient.h"
#include "UIProduct.h"

namespace client {


	CControlUI* UIClient::CreateControl(LPCTSTR pstrClassName)
	{

		return nullptr;
	}
	LRESULT UIClient::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_1F39EEE70D997FA56E9855028409F4B8"));
		auto pEdit = GetCtrl<CEditUI>(_T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"));

		if (wParam == VK_DOWN || wParam == VK_UP)
		{

		}
		else if(wParam == VK_RETURN)
		{

		}

		bHandled = false;
		return 0;
	}
	LRESULT UIClient::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UINT iKey = (UINT)wParam;
		if (iKey == VK_RETURN)
		{
			auto sk = 0;
		}
		bHandled = false;
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
			pTitleUI->SetText(sk::Log::Format(_TEXT("桥系统管理器® [{}]"), sk::StringConvert::MBytesToTString(version)).c_str());
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

						NetSystemProductConsRequest();
						NetSystemProductsRequest();
						NetSystemTradeRecordHead();
						NetSystemExchangeOnline();
						NetSystemMDCacheRequest();
						NetSystemMTUsersRequest();

						this->Open();
					}
					else
					{
						NetApi<sk::network::INetworkApi>()->Stop();
						MsgBox(GetHWND(), _TEXT("登录到服务器失败!"), _TEXT("错误"), MB_TOPMOST);
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
		this->Close();
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

	void UIClient::OnUISystemTimeRefresh(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000010),
				[&]()->int
				{
					return NetSystemTimeRequest();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (!result)
					{
						MsgBox(GetHWND(), _TEXT("请求刷新失败!"), _TEXT("错误"), MB_TOPMOST);
					}
				}, 5000));
	}
	void UIClient::OnUISystemTimeUpdate()
	{
		auto editTime = GetCtrl<CEditUI>(_T("DEF_FC5908D5D9AC711A438DE33A445AE75E"));
		if (editTime)
		{
			auto serverTime = Global::SetupGet()->BridgeSystemTime();
			struct tm _tm;
			gmtime_s(&_tm, &serverTime);
			TCHAR timestr[100] = { 0 };
			_tcsftime(timestr, 100, sk::global_sk_ftime_utc_format, &_tm);
			editTime->SetText(timestr);
			editTime->NeedParentUpdate();
		}
	}













	void UIClient::Notify(TNotifyUI& msg)
	{
		if (msg.sType == DUI_MSGTYPE_HEADERCLICK)
		{
			OnUIListSort(msg.pSender);
		}///DUI_MSGTYPE_HEADERCLICK
		else if (msg.sType == DUI_MSGTYPE_CLICK)
		{
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
			else if (msg.pSender->GetName() == _T("DEF_B4E29CD7BB8744ADF5238569204977C8")) //! 刷新服务器系统时间
			{
				OnUISystemTimeRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_7C67F08158C0C564E126485C5427E9BE")) //! 确认修改系统配置
			{
				OnUISystemSetupUpdate(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_657D081C484CAFAAB1A6364E4997F51E")) //! 保存修改产品配置		
			{
				OnUISystemProductConUpdate(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_AA940C6CDB6BEFC36FB6D931F852BCE8")) //! 刷新产品列表
			{
				OnUISystemProductsRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_10A646A84BC45A631367B3280462E820"))//! 刷新在线交易所
			{
				OnUISystemExchangeOnlineRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_CF98D901196F2C350DA8FF335C1C0125"))//! 刷新交易所持仓
			{
				//OnUISystemExchangePositionRefresh(msg.pSender);
				OnUISystemPositionRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_A238AC461A665414D74F8BCEB3F764E9"))//! 清理日志
			{
				OnUISystemTradeRecordClear(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_7777D51FF50D1365FF42A3D2BE1DC77A"))//! 添加指定MT帐户
			{
				OnUISystemAssignLoginsPostOrderAppend();
			}
			else if (msg.pSender->GetName() == _T("DEF_F8E6EB00B563D548AC4286653993EA35"))//! 移除指定MT帐户
			{
				OnUISystemAssignLoginsPostOrderRemove();
			}
		}///DUI_MSGTYPE_CLICK
		else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		{
			if (msg.pSender->GetName() == _T("DEF_8628E28E9EF8CFA1307E0C8DE875A5A1")) //! 抛单价格类型
			{
				auto pComboOrdType = (CComboUI*)msg.pSender;
				shared::tradeorder::EnOrdType ordType = shared::tradeorder::EnOrdType(sk::StringConvert::WStringToMBytes(pComboOrdType->GetItemAt(pComboOrdType->GetCurSel())->GetUserData().GetData()).c_str()[0]);

				switch (ordType)
				{
				case shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
				{
					GetCtrl<CComboUI>(_T("DEF_89052B592B0691BFBD292D4E210BCB1E"))->SetVisible(true);
					GetCtrl<CComboUI>(_T("DEF_7749E33DB5DD08290AF3CE2862426735"))->SetVisible(false);
				}break;
				case shared::tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT:
				{
					GetCtrl<CComboUI>(_T("DEF_89052B592B0691BFBD292D4E210BCB1E"))->SetVisible(false);
					GetCtrl<CComboUI>(_T("DEF_7749E33DB5DD08290AF3CE2862426735"))->SetVisible(true);
				}break;
				}

				msg.pSender->GetParent()->NeedParentUpdate();
			}
			else if (msg.pSender->GetName() == _T("DEF_6071F6A479D14DE184CC6BE2394A941C"))
			{
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
			else if (msg.pSender->GetName() == _T("DEF_9CD12D041D24ED8D64E170F9A3A17391"))
			{
				OnUISystemProductConRefresh(msg.pSender);
			}
			else if (msg.pSender->GetName() == _T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"))
			{
				OnUISystemExchangePositionRefresh();
				OnUISystemAssignLoginsPostOrderRefresh();
			}
			else if (msg.pSender->GetName() == _T("DEF_252C209091A2AC3D370447CFC125F4E3")) //! 树控
			{
				auto pTreeView = (CTreeViewUI*)msg.pSender;
				GetCtrl<CTabLayoutUI>(_T("DEF_CA9A1E7FE99314C4693E3920565131F5"))->SelectItem(pTreeView->GetCurSel());
			}
			else if (msg.pSender->GetName() == _T("DEF_1F39EEE70D997FA56E9855028409F4B8")) //! SearchList
			{
				auto pList = (CListUI*)msg.pSender;
				auto sel = pList->GetCurSel();
				if (sel >= 0)
				{
					auto the = pList->GetItemAt(sel);
					if (the)
					{
						auto pAssignLoginsEdit = GetCtrl<CEditUI>(_T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"));
						if (pAssignLoginsEdit)
						{
							pAssignLoginsEdit->SetText(the->GetText());
						}
					}
				}
			}
			else if (msg.pSender->GetName() == _T("DEF_988F50A8522653EF50682E196F1EF500")) //! LoginsList
			{
				auto pList = (CListUI*)msg.pSender;
				auto sel = pList->GetCurSel();
				if (sel >= 0)
				{
					auto the = (CListTextElementUI*)pList->GetItemAt(sel);
					auto pAssignLoginsEdit = GetCtrl<CEditUI>(_T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"));
					if (pAssignLoginsEdit && the)
					{
						pAssignLoginsEdit->SetText(tostring(the->GetTagUINT64()).c_str());
					}
				}
			}
		}///DUI_MSGTYPE_ITEMSELECT
		else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		{
			//! 关于点差的输入限制 -- 只允许输入正整数
			if (msg.pSender->GetName() == _T("DEF_A7FC9AACE8DCF522C6D377BAFAFBD376") ||
				msg.pSender->GetName() == _T("DEF_2D2CF54FB062AF57E4CEC73B1699CBB0") ||
				msg.pSender->GetName() == _T("DEF_A4B1DD4431B12657EF423BA68D84CCBC") ||
				msg.pSender->GetName() == _T("DEF_DF3DB43DBACF9CB65DE9D5383FF5CC53") ||
				msg.pSender->GetName() == _T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9") ||
				msg.pSender->GetName() == _T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC") ||
				msg.pSender->GetName() == _T("DEF_DD0152AC9693302127869DFDB845EACD") ||
				msg.pSender->GetName() == _T("DEF_F63588E183AF8A06F66118F7C1ECE253"))
			{
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::IntegerlsFix(InputStr, USHRT_MAX, 0))
				{
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
			//! MT帐户
			if (msg.pSender->GetName() == _T("DEF_27AEAC0F9FAB35DA6E71E6B6A1210779") ||
				msg.pSender->GetName() == _T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"))
			{
				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::LoginsFix(InputStr))
				{
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}

				if (msg.pSender->GetName() == _T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"))
				{
					auto pList = GetCtrl<CListUI>(_T("DEF_1F39EEE70D997FA56E9855028409F4B8"));
					if (pList)
					{
						if (pList->GetCount() <= 0 || pList->GetCurSel() <= 0)
						{
						}
						OnUISystemAssignLoginsPostOrderSearch(msg.pSender);
					}
				}
			}

			//! 时间段沟通限定
			if (msg.pSender->GetName() == _T("DEF_F15D715FF21F3992F459BD4AC7C135A0") ||
				msg.pSender->GetName() == _T("DEF_C9AA0A94D77F635F41CAD104C7C8FCA7") ||
				msg.pSender->GetName() == _T("DEF_2F50FE7DB9BACC7F35CA6926C0EDE13C") ||
				msg.pSender->GetName() == _T("DEF_C6465BB70F9EEB9A41DA4ED9119A573A") ||
				msg.pSender->GetName() == _T("DEF_C6F46D1C4AE3828848DDFE312DEAFA28") ||
				msg.pSender->GetName() == _T("DEF_C78F35DDA841A64330346613C34274EF") ||
				msg.pSender->GetName() == _T("DEF_6FC1B276BA7CC92309C0E8E2E90F3FAA") ||
				msg.pSender->GetName() == _T("DEF_D877E89FFE9A73B20C7DA51400BFF14F") ||
				msg.pSender->GetName() == _T("DEF_C26809B5D882985AC69F2DD2543B7406") ||
				msg.pSender->GetName() == _T("DEF_14D9DB7539860A13FFD0D50DF42B690F") ||
				msg.pSender->GetName() == _T("DEF_05D9388B14343824F5DF71B204D875DA") ||
				msg.pSender->GetName() == _T("DEF_55F0125614A9FBCF5D062CBBAE16EF02") ||
				msg.pSender->GetName() == _T("DEF_DC396A58E7D0CA95E010A49BE1370AE3") ||
				msg.pSender->GetName() == _T("DEF_1024DBA72F584A4B9B667E97354CD117"))
			{

				auto pEdit = (CEditUI*)msg.pSender;
				sk::string InputStr = pEdit->GetText().GetData();
				if (!shared::InputCheck::TimeFrameFix(InputStr)) {
					pEdit->SetText(InputStr.c_str());
					pEdit->SetSel((long)InputStr.length(), (long)InputStr.length());
				}
			}
		}///DUI_MSGTYPE_TEXTCHANGED


	}






	void UIClient::OnUISystemProductsRefresh(CControlUI* pCtrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000120),
				[&]()->int
				{
					return NetSystemProductsRequest();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (!result)
					{
						MsgBox(GetHWND(), _TEXT("请求系统产品失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("系统产品刷新完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUINetworkStatusUpdate(sk::network::EnNetStatus& NetStatusPrev) const
	{
		auto pTargetUI = GetCtrl<CLabelUI>(_TEXT("DEF_3E1929AD9E83A81950BDDE6FC5E79507"));
		if (!pTargetUI || !NetApi<sk::network::INetworkApi>() || \
			NetApi<sk::network::INetworkApi>()->NetStatus() == NetStatusPrev)
			return;
		NetStatusPrev = NetApi<sk::network::INetworkApi>()->NetStatus();
		switch (NetStatusPrev)
		{
		case sk::network::EnNetStatus::EN_NETSTATUS_CONNECTED:
		{
			pTargetUI->SetText(LR"(<c #00FF00><b>与系统服务器建立连接...</b></c>)");
		}break;
		case sk::network::EnNetStatus::EN_NETSTATUS_CONNECTING:
		{
			pTargetUI->SetText(LR"(<c #FF0000><b>正在连接到服务器...</b></c>)");
		}break;
		case sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED:
		{
			pTargetUI->SetText(LR"(<c #FFD700><b>准备就绪.</b></c>)");
		}break;
		default:
		{
			pTargetUI->SetText(LR"(<c #FF0000><b>与系统服务器断开连接...</b></c>)");
		}break;
		}///switch
	}
	void UIClient::OnUISystemMDUpdate(const shared::marketdata::MarketData& md)
	{
		if (!md.Verify())
			return;
		auto pList = GetCtrl<CListUI>(_T("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
		if (!pList)
			return;

		auto pListNode = (IProductTreeNodeUI*)pList->GetItemAtByTagUINT64(shared::SystemIDType::SystemProductType(md.ID));
		if (pListNode)
		{
			Global::SharedGet()->m_SystemProductQ.search(
				shared::SystemIDType::SystemProductType(md.ID),
				[&](const auto&, auto& obj)
				{
					pListNode->SetQuotes(md, obj.Decimals);
				});
		}
	}
	void UIClient::OnUISystemProductConUpdate(CControlUI* pCtrl)
	{
		auto pListEdit = GetCtrl<IProductEditUI>(_T("DEF_770A32C71E22FBBDD2C8F330C683FD55"));
		if (!pListEdit)
			return;
		auto pak = pListEdit->Packet();
		if (pak.empty() || pListEdit->GetTagUINT64() <= 0)
			return;

		auto result = Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000210),
				[&]()->int
				{
					return NetSystemProductConsRequest(pListEdit->GetTagUINT64(), sk::network::Header::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_UPDATE, pak);
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (!result || SKAPIRES_FAILED(head.Retcode))
					{
						MsgBox(GetHWND(), _TEXT("修改产品配置失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("修改产品配置完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUISystemTradeRecordInit(const sk::packet& headStr)
	{
		if (headStr.empty())
		{

		}
		auto pList = GetCtrl<CListUI>(_TEXT("DEF_69F087E67D2D28F7B24D16DB549F9D7A"));
		if (!pList) return;
		pList->RemoveAll();
		std::vector<sk::packet> heads;
		sk::Helper::OneUnknownParseA(headStr, '\t', heads);
		if (heads.empty()) return;
		CListHeaderUI* pHeader = pList->GetHeader();
		if (!pHeader) return;
		if (pHeader->GetCount() <= 0)
		{
			//!@ { "时间", "MT帐户","系统品种", "MT品种","LP品种","交易所","系统单号","订单类型","订单量","成交量","订单价","成交均价","最后成交价","处理时间(毫秒)","循环抛单(次数)","MT状态码","处理结果" }
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
				case 1://!MT帐户
					[[fallthrough]];
				case 2://!系统品种
					[[fallthrough]];
				case 3://!MT品种
					[[fallthrough]];
				case 4://!LP品种
				{
					pHeaderItem->SetFixedWidth(70);
				}break;
				case 5://!交易所
				{
					pHeaderItem->SetFixedWidth(90);
				}break;
				case 6://!系统单号
				{
					pHeaderItem->SetFixedWidth(120);
				}break;
				case 7://!订单类型
					[[fallthrough]];
				case 8://!订单量
					[[fallthrough]];
				case 9://!成交量
					[[fallthrough]];
				case 10://!订单价
					[[fallthrough]];
				case 11://!成交均价
					[[fallthrough]];
				case 12://!最后成交价
				{
					pHeaderItem->SetFixedWidth(70);
				}break;
				case 13://!循环抛单(次数)
					[[fallthrough]];
				case 14://!处理时间(毫秒)
				{
					pHeaderItem->SetFixedWidth(100);
				}break;
				case 15://!MT状态码
				{
					pHeaderItem->SetFixedWidth(70);
				}break;
				case 16://!处理结果
				{
					pHeaderItem->SetFixedWidth(400);
				}break;
				default:
				{
					pHeaderItem->SetFixedWidth(60);
				}break;
				}///switch
				pList->Add(pHeaderItem);
				++index;
			}
			NetSystemTradeRecordUpdate();
		}
	}
	void UIClient::OnUISystemTradeRecordUpdate(const sk::packet& records)
	{
		if (records.empty()) return;
		auto pList = GetCtrl<CListUI>(_TEXT("DEF_69F087E67D2D28F7B24D16DB549F9D7A"));
		if (!pList) return;

		if (pList->GetCount() > 1000)
			pList->RemoveAll();

		std::vector<sk::packet> logs;
		sk::Helper::OneUnknownParseA(records, '@', logs);

		for (auto rit = logs.rbegin(); rit != logs.rend(); ++rit)
		{
			std::vector<sk::packet> log;
			sk::Helper::OneUnknownParseA(*rit, '\t', log);


			auto pListElem = new CListTextElementUI();
			pList->AddAt(pListElem, 0);

			int pos = 0;
			for (const auto& item : log)
			{
				pListElem->SetText(pos, sk::StringConvert::MBytesToWString(item).c_str());
				++pos;
			}
		}
		pList->NeedParentUpdate();
	}
	void UIClient::OnUISystemTradeRecordRefresh(CControlUI* ctrl)
	{

	}
	void UIClient::OnUISystemExchangePositionUpdate()
	{
		auto pListPosition = GetCtrl<CListUI>(_T("DEF_487153CB7FF3A366B05C7801089E7192"));
		if (!pListPosition)
			return;

		pListPosition->RemoveAll();
		//!@ 按合约名字排序（为了用眼睛好比对）
		Global::SharedGet()->m_SystemPositionQ.search(
			shared::SystemIDType::ExchangeType(pListPosition->GetTagUINT64()),
			[&](const auto&, auto& positions)
			{
				std::multimap<sk::packet, shared::SimplePositionField> sort;

				positions.iterate(
					[&](const auto& exhangeProdID, shared::SimplePositionField& position)
					{
						sort.insert({ {position.Symbol.symbol,position} });
					});
#if 0
				< ListHeaderItem text = "合约名字" textcolor = "#FFFFFFFF" width = "100" align = "center" textpadding = "0,0,0,0" / >
					<ListHeaderItem text = "多空类型" textcolor = "#FFFFFFFF" width = "70" align = "center" textpadding = "0,0,0,0" / >
					<ListHeaderItem text = "持仓总量" textcolor = "#FFFFFFFF" width = "70" align = "center" textpadding = "0,0,0,0" / >
					<ListHeaderItem text = "开仓价格" textcolor = "#FFFFFFFF" width = "70" align = "center" textpadding = "0,0,0,0" / >
					<ListHeaderItem text = "交易单号" textcolor = "#FFFFFFFF" width = "200" align = "center" textpadding = "0,0,0,0" / >
#endif
					for (const auto& node : sort)
					{
						auto pListElem = new CListTextElementUI();
						pListPosition->Add(pListElem);
						pListElem->SetText(0, sk::StringConvert::MBytesToWString(node.first).c_str());
						pListElem->SetText(1, shared::tradeorder::Misc::StrOrdSide(node.second.Side));
						pListElem->SetText(2, tostring(node.second.ExtTotalQty).c_str());
						pListElem->SetText(3, sk::Helper::FloatToStr(node.second.PriceOpen, node.second.Digits, false).c_str());
						pListElem->SetText(4, sk::Log::Format(L"{:X}", node.second.ExtOrderID).c_str());
						pListElem->SetTagUINT64(shared::SystemIDType::SystemProductType(node.second.ExtGatewayPID));
					}
			});

		pListPosition->NeedParentUpdate();
	}
	void UIClient::OnUISystemPositionRefresh(CControlUI* ctrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"));
		if (!pList)
			return;
		auto sel = pList->GetCurSel();
		if (sel < 0)
			return;
		auto pCurSel = pList->GetItemAt(sel);
		auto id = pCurSel->GetTagUINT64();
		if (id <= 0)
			return;

		auto pListPosition = GetCtrl<CListUI>(_T("DEF_487153CB7FF3A366B05C7801089E7192"));
		if (!pListPosition)
			return;
		pListPosition->SetTagUINT64(id);

		OnUISystemExchangePositionUpdate();
		NetSystemPositionRequest(pListPosition->GetTagUINT64());
	}
	void UIClient::OnUISystemExchangePositionRefresh(CControlUI* ctrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"));
		if (!pList)
			return;
		auto sel = pList->GetCurSel();
		if (sel < 0)
			return;
		auto pCurSel = pList->GetItemAt(sel);
		auto id = pCurSel->GetTagUINT64();
		if (id <= 0)
			return;

		auto pListPosition = GetCtrl<CListUI>(_T("DEF_487153CB7FF3A366B05C7801089E7192"));
		if (!pListPosition)
			return;
		pListPosition->SetTagUINT64(id);
		NetSystemExchangePositionRequest(id);
	}
	void UIClient::OnUISystemExchangeOnlineRefresh(CControlUI* ctrl)
	{
		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000040),
				[&]()->int
				{
					return NetSystemExchangeOnline();
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (!result)
					{
						MsgBox(GetHWND(), _TEXT("请求在线交易所失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("请求在线交易所完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUISystemAssignLoginsPostOrderAppend()
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_988F50A8522653EF50682E196F1EF500"));
		if (!pList)
			return;
		auto pAssignLogin = GetCtrl<CEditUI>(_T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"));
		if (!pAssignLogin)
			return;
		if (pAssignLogin->GetText().IsEmpty())
			return;
		UINT64 Login = _tcstoull(pAssignLogin->GetText().GetData(), nullptr, 10);
		if (Login <= 0)
			return;
		shared::UserSimpleField userField;
		if (!Global::SharedGet()->m_MTUserSimpleQ.search(Login, [&](const auto&, auto& at) {userField = at; }))
		{
			MsgBox(GetHWND(), _T("不存在此帐户!"), _T("错误"), MB_TOPMOST);
			return;
		}
		auto pTheCtrl = pList->GetItemAtByTagUINT64(Login);
		if (pTheCtrl)
		{
			MsgBox(GetHWND(), _T("该帐户已经存在列表中!"), _T("提示"), MB_TOPMOST);
			pList->SelectItem(pList->GetItemIndex(pTheCtrl));
			return;
		}

		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000060),
				[&]()->int
				{
					return NetSystemAssignLoginsPostOrder(pList->GetTagUINT64(), sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_INSERT, sk::packet((char*)&Login, sizeof(Login)));
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (pak.empty())
					{
						MsgBox(GetHWND(), _TEXT("添加失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("添加完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUISystemAssignLoginsPostOrderRemove()
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_988F50A8522653EF50682E196F1EF500"));
		if (!pList)
			return;
		auto pAssignLogin = GetCtrl<CEditUI>(_T("DEF_EDBF99831BF47D7A5A5A8745A0C1A911"));
		if (!pAssignLogin)
			return;
		if (pAssignLogin->GetText().IsEmpty())
		{
			MsgBox(GetHWND(), _T("请正确指定要移除的帐户!"), _T("错误"), MB_TOPMOST);
			return;
		}
		UINT64 Login = _tcstoull(pAssignLogin->GetText().GetData(), nullptr, 10);
		if (Login <= 0)
			return;
		shared::UserSimpleField userField;
		if (!Global::SharedGet()->m_MTUserSimpleQ.search(Login, [&](const auto&, auto& at) {userField = at; }))
		{
			MsgBox(GetHWND(), _T("不存在此帐户!"), _T("错误"), MB_TOPMOST);
			return;
		}
		auto pTheCtrl = pList->GetItemAtByTagUINT64(Login);
		if (!pTheCtrl)
		{
			MsgBox(GetHWND(), _T("该帐户不在列表中!"), _T("错误"), MB_TOPMOST);
			return;
		}

		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000060),
				[&]()->int
				{
					return NetSystemAssignLoginsPostOrder(pList->GetTagUINT64(), sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_DELETE, sk::packet((char*)&Login, sizeof(Login)));
				},
				[&](const bool result, const sk::network::Header& head, const sk::packet& pak)
				{
					if (pak.empty())
					{
						MsgBox(GetHWND(), _TEXT("移除失败!"), _TEXT("错误"), MB_TOPMOST);
					}
					else
					{
						MsgBox(GetHWND(), _TEXT("移除完成!"), _TEXT("提示"), MB_TOPMOST);
					}
				}, 10000));
	}
	void UIClient::OnUISystemAssignLoginsPostOrderRefresh()
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"));
		if (!pList)
			return;
		auto sel = pList->GetCurSel();
		if (sel < 0)
			return;
		auto pCurSel = pList->GetItemAt(sel);
		auto id = pCurSel->GetTagUINT64();
		if (id <= 0)
			return;
		auto pListLogins = GetCtrl<CListUI>(_T("DEF_988F50A8522653EF50682E196F1EF500"));
		if (!pListLogins)
			return;
		pListLogins->SetTagUINT64(shared::SystemIDType::ExchangeType(id));
		OnUISystemAssignLoginsPostOrderUpdate(pListLogins->GetTagUINT64());
		NetSystemAssignLoginsPostOrder(pListLogins->GetTagUINT64(), sk::network::HEADER::EnDataProcessFlag::EN_DATA_PROCESS_FLAG_SELECT, "");
	}
	void UIClient::OnUISystemAssignLoginsPostOrderUpdate(const shared::SystemID& exchange)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_988F50A8522653EF50682E196F1EF500"));
		if (!pList)
			return;
		if (pList->GetTagUINT64() <= 0)
			return;
		if (exchange != pList->GetTagUINT64())
			return;
		pList->RemoveAll();
		Global::SharedGet()->m_SystemAssignLoginsPostOrderQ.search(
			pList->GetTagUINT64(),
			[&](const auto&, auto& logins)
			{
				logins.iterate(
					[&](auto& login)
					{
						Global::SharedGet()->m_MTUserSimpleQ.search(
							login,
							[&](const auto&, auto& userField)
							{
								CListTextElementUI* pElemLogin = new CListTextElementUI();
								pList->Add(pElemLogin);
								pElemLogin->SetText(0, tostring(login).c_str());
								pElemLogin->SetText(1, sk::StringConvert::MBytesToWString(userField.Group).c_str());
								pElemLogin->SetTagUINT64(login);
								pElemLogin->SetCache(userField.Group);
								pElemLogin->NeedUpdate();
								pElemLogin->NeedParentUpdate();
							});
					});
			});
		pList->NeedUpdate();
		pList->NeedParentUpdate();
		pList->GetParent()->NeedParentUpdate();
	}
	void UIClient::OnUISystemAssignLoginsPostOrderSearch(CControlUI* ctrl /*= nullptr*/)
	{
		if (!ctrl)
			return;
		CDuiString theText = ctrl->GetText();
		if (theText.IsEmpty())
			return;
		auto pList = GetCtrl<CListUI>(_T("DEF_1F39EEE70D997FA56E9855028409F4B8"));
		if (!pList)
			return;
		pList->RemoveAll();
		Global::SharedGet()->m_MTUserSimpleQ.iterate(
			[&](const auto& login, auto& userField)
			{
				if (!sk::SafeCmpW(theText.GetData(), tostring(login).c_str(), theText.GetLength()))
				{
					CListLabelElementUI* pElem = new CListLabelElementUI();
					pList->Add(pElem);
					pElem->SetText(tostring(login).c_str());
				}
			});
		pList->NeedParentUpdate();
		if (pList->GetCount() > 0)
		{
			pList->Activate();
		}
	}
	void UIClient::OnUISystemExchangeOnlineUpdate()
	{
		if (Global::SharedGet()->m_SystemProductQ.empty())
			return;
		auto pList = GetCtrl<CListUI>(_T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"));
		if (!pList)
			return;
		pList->RemoveAll();
		CDialogBuilder builder;
		Global::SharedGet()->m_ExchangeOnlineQ.iterate(
			[&](const auto& flag, shared::BridgeLoginField& exchange)
			{
				auto pListLine = (IExchangeTreeNodeUI*)builder.Create(_T("UIExchangeTreeNode.xml"));
				pList->Add(pListLine);
				pListLine->SetTagUINT64(flag);
				pListLine->SetID(flag);
				pListLine->SetExchange(exchange.Username);
				pListLine->SetModule(exchange.Identify);
			});
		if (pList->GetCount() && pList->GetCurSel() < 0)
		{
			pList->SelectItem(0);
		}
	}
	void UIClient::OnUISystemTradeRecordClear(CControlUI* ctrl)
	{
		auto pList = GetCtrl<CListUI>(_T("DEF_69F087E67D2D28F7B24D16DB549F9D7A"));
		if (!pList) return;
		pList->RemoveAll();
	}
	void UIClient::OnUISystemProductConRefresh(CControlUI* pCtrl)
	{
		auto pListTree = GetCtrl<CListUI>(_T("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
		if (!pListTree)
			return;
		auto nCurSel = pListTree->GetCurSel();
		if (nCurSel < 0) return;
		auto pSel = pListTree->GetItemAt(nCurSel);
		auto con = Global::SharedGet()->m_SystemSymbolConQ.search(pSel->GetTagUINT64());
		if (!con) return;
		auto pListEdit = GetCtrl<IProductEditUI>(_T("DEF_770A32C71E22FBBDD2C8F330C683FD55"));
		if (!pListEdit) return;
		pListEdit->RemoveAll();
		pListEdit->SetCache(*con);
		pListEdit->Parse();
		pListEdit->NeedParentUpdate();
	}
	void UIClient::OnUISystemProductsUpdate()
	{
		if (Global::SharedGet()->m_SystemProductQ.empty())
			return;
		auto pList = GetCtrl<CListUI>(_T("DEF_9CD12D041D24ED8D64E170F9A3A17391"));
		if (!pList)
			return;
		pList->RemoveAll();
		CDialogBuilder builder;
		Global::SharedGet()->m_SystemProductQ.iterate(
			[&](const auto& sysProdID, shared::SystemProduct& sysProdObj)
			{
				auto pListLine = (IProductTreeNodeUI*)builder.Create(_T("UIProductTreeNode.xml"));
				if (pListLine)
				{
					pList->Add(pListLine);
					pListLine->SetTag((int)sysProdObj.Decimals);
					pListLine->SetTagUINT64(sysProdID);
					pListLine->SetSymbol(sk::StringConvert::MBytesToTString(sysProdObj.Symbol.symbol));
				}
				else
				{
					auto sk = 0;
				}
			});
		if (pList->GetCount() && pList->GetCurSel() < 0)
		{
			pList->SelectItem(0);
		}
	}



	void UIClient::OnUISystemSetupRefresh(CControlUI* crtl /*= nullptr*/)
	{
		auto& SystemSetup = Global::SetupGet()->BridgeSystemSetup();

		//!@ 系统平台模式
		auto comboSystemPlatformMode = GetCtrl<CComboUI>(_T("DEF_8949506871A4AF501EEA44A91D231E72"));
		if (comboSystemPlatformMode)
		{
			for (int i = 0; i < comboSystemPlatformMode->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.SystemPlatformMode == \
					_tcstol(comboSystemPlatformMode->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboSystemPlatformMode->SelectItem(i);
					break;
				}
			}
		}

		//!@ 交易定单超时间间隔
		auto comboTimeoutDealerOrder = GetCtrl<CComboUI>(_T("DEF_B82C33159C313613E3BBEA7E7AAA59AE"));
		if (comboTimeoutDealerOrder)
		{
			for (int i = 0; i < comboTimeoutDealerOrder->GetCount(); ++i)
			{
				if (SystemSetup.TimeoutDealerOrder == \
					_tcstoll(comboTimeoutDealerOrder->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboTimeoutDealerOrder->SelectItem(i);
					break;
				}
			}
		}

		//!@ 交易定单超时间间隔
		auto comboTimeoutQuoteActive = GetCtrl<CComboUI>(_T("DEF_C120AFB5960DFC72F1253071D90AAE90"));
		if (comboTimeoutQuoteActive)
		{
			for (int i = 0; i < comboTimeoutQuoteActive->GetCount(); ++i)
			{
				if (SystemSetup.TimeoutQuoteActive == \
					_tcstoll(comboTimeoutQuoteActive->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboTimeoutQuoteActive->SelectItem(i);
					break;
				}
			}
		}

		//!@ 定单类型
		auto comboDealerOrdType = GetCtrl<CComboUI>(_T("DEF_8628E28E9EF8CFA1307E0C8DE875A5A1"));
		if (comboDealerOrdType)
		{
			for (int i = 0; i < comboDealerOrdType->GetCount(); ++i)
			{
				auto userdata = comboDealerOrdType->GetItemAt(i)->GetUserData().GetData();
				if (SystemSetup.DealerOrdType == \
					decltype(SystemSetup.DealerOrdType)(sk::StringConvert::WStringToMBytes(userdata)[0]))
				{
					comboDealerOrdType->SelectItem(i);
					break;
				}
			}
		}

		//!@ 有效期定单类型
		auto pComboOrdTypeMarket = GetCtrl<CComboUI>(_T("DEF_89052B592B0691BFBD292D4E210BCB1E"));
		auto pComboOrdTypeLimit = GetCtrl<CComboUI>(_T("DEF_7749E33DB5DD08290AF3CE2862426735"));
		auto pComboOrdTypeStop = GetCtrl<CComboUI>(_T("DEF_2972D164E258EDC8633348BE244E297D"));
		switch (SystemSetup.DealerOrdType)
		{
		case shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
		{
			if (pComboOrdTypeMarket)
			{
				pComboOrdTypeMarket->SetVisible(true);

				for (int i = 0; i < pComboOrdTypeMarket->GetCount(); ++i)
				{
					auto userdata = pComboOrdTypeMarket->GetItemAt(i)->GetUserData().GetData();
					if (SystemSetup.DealerOrdTimeInForce == \
						decltype(SystemSetup.DealerOrdTimeInForce)(sk::StringConvert::WStringToMBytes(userdata)[0]))
					{
						pComboOrdTypeMarket->SelectItem(i);
						break;
					}
				}

			}
			if (pComboOrdTypeLimit)
				pComboOrdTypeLimit->SetVisible(false);
			if (pComboOrdTypeStop)
				pComboOrdTypeStop->SetVisible(false);
		}break;
		case shared::tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT:
		{
			if (pComboOrdTypeLimit)
			{
				pComboOrdTypeLimit->SetVisible(true);

				for (int i = 0; i < pComboOrdTypeLimit->GetCount(); ++i)
				{
					auto userdata = pComboOrdTypeLimit->GetItemAt(i)->GetUserData().GetData();
					if (SystemSetup.DealerOrdTimeInForce == \
						decltype(SystemSetup.DealerOrdTimeInForce)(sk::StringConvert::WStringToMBytes(userdata)[0]))
					{
						pComboOrdTypeLimit->SelectItem(i);
						break;
					}
				}
			}
			if (pComboOrdTypeMarket)
				pComboOrdTypeMarket->SetVisible(false);
			if (pComboOrdTypeStop)
				pComboOrdTypeStop->SetVisible(false);
		}break;
		default:
		{
			if (pComboOrdTypeStop)
			{
				pComboOrdTypeStop->SetVisible(true);

				for (int i = 0; i < pComboOrdTypeStop->GetCount(); ++i)
				{
					auto userdata = pComboOrdTypeStop->GetItemAt(i)->GetUserData().GetData();
					if (SystemSetup.DealerOrdTimeInForce == \
						decltype(SystemSetup.DealerOrdTimeInForce)(sk::StringConvert::WStringToMBytes(userdata)[0]))
					{
						pComboOrdTypeStop->SelectItem(i);
						break;
					}
				}
			}
			if (pComboOrdTypeMarket)
				pComboOrdTypeMarket->SetVisible(false);
			if (pComboOrdTypeLimit)
				pComboOrdTypeLimit->SetVisible(false);
		}break;
		}
		//!@ 聚合报价开关
		if (SystemSetup.AggregateQuoteSwitch == decltype(SystemSetup.AggregateQuoteSwitch)::EN_AGGREGATE_QUOTE_FALG_ENABLE)
		{
			GetCtrl<COptionUI>(_T("DEF_C7AE6101D6A5DAB2E0DB07649862FF88"))->Selected(true);//!启用
		}
		else
		{
			GetCtrl<COptionUI>(_T("DEF_F2B04474B2C0AA89BFCC9782FFDB0055"))->Selected(true);//!禁用
		}

		//!@ 数据源点差处理模式
		auto comboFeederSpreadMode = GetCtrl<CComboUI>(_T("DEF_24F52A776278EC8486924DA8868D797A"));
		if (comboFeederSpreadMode)
		{
			for (int i = 0; i < comboFeederSpreadMode->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.FeederSpreadMode == \
					_tcstol(comboFeederSpreadMode->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboFeederSpreadMode->SelectItem(i);
					break;
				}
			}
		}
		//!@ 自动调仓开关
		if (SystemSetup.AutoAdjustPositionSwitch == decltype(SystemSetup.AutoAdjustPositionSwitch)::EN_AUTO_ADJUST_POSITION_SWITCH_ENABLE)
		{
			GetCtrl<COptionUI>(_T("DEF_6B742269FFB2B7574EA76412968B1BC7"))->Selected(true);//!启用
		}
		else
		{
			GetCtrl<COptionUI>(_T("DEF_1AB4A38CE223585E8EDF5DF50DC0AE33"))->Selected(true);//!禁用
		}
		//!@ 成交价格类型
		auto comboDealerTradePxType = GetCtrl<CComboUI>(_T("DEF_5BE420660E68327B1CB45B6D68B66E0A"));
		if (comboDealerTradePxType)
		{
			for (int i = 0; i < comboDealerTradePxType->GetCount(); ++i)
			{
				auto userdata = comboDealerTradePxType->GetItemAt(i)->GetUserData().GetData();
				if (SystemSetup.DealerTradePxType == \
					decltype(SystemSetup.DealerTradePxType)(_tcstol(userdata, nullptr, 10)))
				{
					comboDealerTradePxType->SelectItem(i);
					break;
				}
			}
		}

		//!@ 指定帐户过桥定单组合类型
		auto comboSystemOrderMode = GetCtrl<CComboUI>(_T("DEF_21F2213FFDB0B20283BB2BFDF76DB275"));
		if (comboSystemOrderMode)
		{
			for (int i = 0; i < comboSystemOrderMode->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.SystemOrderMode == \
					_tcstol(comboSystemOrderMode->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboSystemOrderMode->SelectItem(i);
					break;
				}
			}
		}
		//!@ 指定抛弹类型帐户
		auto editOneByOneOrderAssignLogin = GetCtrl<CEditUI>(_T("DEF_27AEAC0F9FAB35DA6E71E6B6A1210779"));
		if (editOneByOneOrderAssignLogin)
		{
			sk::stringw strAssignLogins;
			for (size_t i = 0; i < _countof(SystemSetup.OneByOneOrderAssignLogin); ++i)
			{
				if (SystemSetup.OneByOneOrderAssignLogin[i] > 0)
				{
					strAssignLogins.append(tostringw(SystemSetup.OneByOneOrderAssignLogin[i])).append(L"\\");
				}
			}
			if (!strAssignLogins.empty())
			{
				editOneByOneOrderAssignLogin->SetText(strAssignLogins.c_str());
			}
		}

		//! 部分成交订单处理选项
		auto comboDealerContinueToRequestSwitch = GetCtrl<CComboUI>(_T("DEF_3F5761025416D17C5803F4107B5F8E87"));
		if (comboDealerContinueToRequestSwitch)
		{
			for (int i = 0; i < comboDealerContinueToRequestSwitch->GetCount(); ++i)
			{
				if ((UINT)SystemSetup.DealerContinueToRequestSwitch == \
					_tcstol(comboDealerContinueToRequestSwitch->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboDealerContinueToRequestSwitch->SelectItem(i);
					break;
				}
			}
		}

		//! 延迟继续请求时间间隔
		auto comboDealerContinueToRequestDelay = GetCtrl<CComboUI>(_T("DEF_8751D5C2DCCF52E6CBD2AB49DB77B602"));
		if (comboDealerContinueToRequestDelay)
		{
			for (int i = 0; i < comboDealerContinueToRequestDelay->GetCount(); ++i)
			{
				if (SystemSetup.DealerContinueToRequestDelay == \
					_tcstoll(comboDealerContinueToRequestDelay->GetItemAt(i)->GetUserData().GetData(), nullptr, 10))
				{
					comboDealerContinueToRequestDelay->SelectItem(i);
					break;
				}
			}
		}

		//! 交易工作时间段
		for (size_t wday = 0; wday < _countof(shared::BridgeSystemSetup::SystemOperatingTimePeriod); ++wday)
		{
			switch (wday)
			{
			case 1:
			{
				GetCtrl<CEditUI>(_T("DEF_F15D715FF21F3992F459BD4AC7C135A0"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_C9AA0A94D77F635F41CAD104C7C8FCA7"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 2:
			{
				GetCtrl<CEditUI>(_T("DEF_C6F46D1C4AE3828848DDFE312DEAFA28"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_C78F35DDA841A64330346613C34274EF"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 3:
			{
				GetCtrl<CEditUI>(_T("DEF_6FC1B276BA7CC92309C0E8E2E90F3FAA"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_D877E89FFE9A73B20C7DA51400BFF14F"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 4:
			{
				GetCtrl<CEditUI>(_T("DEF_C26809B5D882985AC69F2DD2543B7406"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_14D9DB7539860A13FFD0D50DF42B690F"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 5:
			{
				GetCtrl<CEditUI>(_T("DEF_05D9388B14343824F5DF71B204D875DA"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_55F0125614A9FBCF5D062CBBAE16EF02"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 6:
			{
				GetCtrl<CEditUI>(_T("DEF_DC396A58E7D0CA95E010A49BE1370AE3"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_1024DBA72F584A4B9B667E97354CD117"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			case 0:
			{
				GetCtrl<CEditUI>(_T("DEF_2F50FE7DB9BACC7F35CA6926C0EDE13C"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeBegin)).c_str());
				GetCtrl<CEditUI>(_T("DEF_C6465BB70F9EEB9A41DA4ED9119A573A"))->\
					SetText(sk::StringConvert::MBytesToTString(sk::time::Time::TimePeriodUnMade(SystemSetup.SystemOperatingTimePeriod[wday].TimeEnd)).c_str());
			}break;
			}
		}

		//! 开发调试模式
		if (SystemSetup.DevelopDebugSwitch == decltype(SystemSetup.DevelopDebugSwitch)::EN_DEVELOP_DEBUG_MODE_ENABLE)
		{
			GetCtrl<COptionUI>(_T("DEF_6377042096DC8814BC20775A000A0559"))->Selected(true);//!启用
		}
		else
		{
			GetCtrl<COptionUI>(_T("DEF_0D56334536ED73507BDCCAA975D24F51"))->Selected(true);//!禁用
		}
	}
	void UIClient::OnUISystemSetupUpdate(CControlUI* crtl /*= nullptr*/)
	{
		shared::BridgeSystemSetup SystemSetup;

		//! 系统平台模式
		auto comboSystemPlatformMode = GetCtrl<CComboUI>(_T("DEF_8949506871A4AF501EEA44A91D231E72"));
		if (comboSystemPlatformMode)
		{
			SystemSetup.SystemPlatformMode = decltype(SystemSetup.SystemPlatformMode)(_tcstol(comboSystemPlatformMode->GetItemAt(comboSystemPlatformMode->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}

		//! 指定帐户过桥定单组合类型
		auto comboSystemOrderMode = GetCtrl<CComboUI>(_T("DEF_21F2213FFDB0B20283BB2BFDF76DB275"));
		if (comboSystemOrderMode)
		{
			SystemSetup.SystemOrderMode = decltype(SystemSetup.SystemOrderMode)(_tcstol(comboSystemOrderMode->GetItemAt(comboSystemOrderMode->GetCurSel())->GetUserData().GetData(), nullptr, 10));
		}
		auto editAssignLogins = GetCtrl<CEditUI>(_T("DEF_27AEAC0F9FAB35DA6E71E6B6A1210779"));
		if (editAssignLogins)
		{
			std::vector<sk::stringw> logins;
			sk::Helper::OneUnknownParseW(editAssignLogins->GetText().GetData(), '\\', logins);
			for (const auto& login : logins)
			{
				SystemSetup.OneByOneOrderAssignLoginAppend(wcstoull(login.c_str(), nullptr, 10));
			}
		}

		//! 定单任务超时间间隔
		auto comboTimeoutDealerOrder = GetCtrl<CComboUI>(_T("DEF_B82C33159C313613E3BBEA7E7AAA59AE"));
		if (comboTimeoutDealerOrder)
		{
			SystemSetup.TimeoutDealerOrder = _tcstoll(comboTimeoutDealerOrder->GetItemAt(comboTimeoutDealerOrder->GetCurSel())->GetUserData().GetData(), nullptr, 10);
		}

		//! 定单任务超时间间隔
		auto comboTimeoutQuoteActive = GetCtrl<CComboUI>(_T("DEF_C120AFB5960DFC72F1253071D90AAE90"));
		if (comboTimeoutQuoteActive)
		{
			SystemSetup.TimeoutQuoteActive = _tcstoll(comboTimeoutQuoteActive->GetItemAt(comboTimeoutQuoteActive->GetCurSel())->GetUserData().GetData(), nullptr, 10);
		}

		//! 网关定单价格类型有效期类型
		auto comboDealerOrdType = GetCtrl<CComboUI>(_T("DEF_8628E28E9EF8CFA1307E0C8DE875A5A1"));
		if (comboDealerOrdType)
		{
			auto userdata = comboDealerOrdType->GetItemAt(comboDealerOrdType->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.DealerOrdType = decltype(SystemSetup.DealerOrdType)(sk::StringConvert::WStringToMBytes(userdata)[0]);
			}

		}
		//! 交易定单价格时间类型
		CComboUI* pComboOrdType = nullptr;
		switch (SystemSetup.DealerOrdType)
		{
		case shared::tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT:
		{
			pComboOrdType = GetCtrl<CComboUI>(_T("DEF_7749E33DB5DD08290AF3CE2862426735"));
		}break;
		case shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
		{
			pComboOrdType = GetCtrl<CComboUI>(_T("DEF_89052B592B0691BFBD292D4E210BCB1E"));
		}break;
		}
		if (pComboOrdType)
		{
			auto userdata = pComboOrdType->GetItemAt(pComboOrdType->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.DealerOrdTimeInForce = decltype(SystemSetup.DealerOrdTimeInForce)(sk::StringConvert::WStringToMBytes(userdata)[0]);
			}
		}

		//! 网关定单成交价格类型
		auto comboDealerTradePxType = GetCtrl<CComboUI>(_T("DEF_5BE420660E68327B1CB45B6D68B66E0A"));
		if (comboDealerTradePxType)
		{
			auto userdata = comboDealerTradePxType->GetItemAt(comboDealerTradePxType->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.DealerTradePxType = decltype(SystemSetup.DealerTradePxType)(_tcstol(userdata, nullptr, 10));
			}
		}

		//! 部分成交订单处理选项
		auto comboDealerContinueToRequestSwitch = GetCtrl<CComboUI>(_T("DEF_3F5761025416D17C5803F4107B5F8E87"));
		if (comboDealerContinueToRequestSwitch)
		{
			auto userdata = comboDealerContinueToRequestSwitch->GetItemAt(comboDealerContinueToRequestSwitch->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.DealerContinueToRequestSwitch = decltype(SystemSetup.DealerContinueToRequestSwitch)(_tcstol(userdata, nullptr, 10));
			}
		}

		//! 延迟继续请求时间间隔
		auto comboDealerContinueToRequestDelay = GetCtrl<CComboUI>(_T("DEF_8751D5C2DCCF52E6CBD2AB49DB77B602"));
		if (comboDealerContinueToRequestDelay)
		{
			auto userdata = comboDealerContinueToRequestDelay->GetItemAt(comboDealerContinueToRequestDelay->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.DealerContinueToRequestDelay = decltype(SystemSetup.DealerContinueToRequestDelay)(_tcstoll(userdata, nullptr, 10));
			}
		}

		//! 聚合报价开关
		if (GetCtrl<COptionUI>(_T("DEF_C7AE6101D6A5DAB2E0DB07649862FF88"))->IsSelected())
		{
			SystemSetup.AggregateQuoteSwitch = decltype(SystemSetup.AggregateQuoteSwitch)::EN_AGGREGATE_QUOTE_FALG_ENABLE;
		}
		else
		{
			SystemSetup.AggregateQuoteSwitch = decltype(SystemSetup.AggregateQuoteSwitch)::EN_AGGREGATE_QUOTE_FALG_DISABLE;
		}

		//!@ 数据源点差处理模式
		auto comboFeederSpreadMode = GetCtrl<CComboUI>(_T("DEF_24F52A776278EC8486924DA8868D797A"));
		if (comboFeederSpreadMode)
		{
			auto userdata = comboFeederSpreadMode->GetItemAt(comboFeederSpreadMode->GetCurSel())->GetUserData().GetData();
			if (userdata)
			{
				SystemSetup.FeederSpreadMode = decltype(SystemSetup.FeederSpreadMode)(_tcstol(userdata, nullptr, 10));
			}
		}

		//! 自动调仓开关
		if (GetCtrl<COptionUI>(_T("DEF_6B742269FFB2B7574EA76412968B1BC7"))->IsSelected())
		{
			SystemSetup.AutoAdjustPositionSwitch = decltype(SystemSetup.AutoAdjustPositionSwitch)::EN_AUTO_ADJUST_POSITION_SWITCH_ENABLE;
		}
		else
		{
			SystemSetup.AutoAdjustPositionSwitch = decltype(SystemSetup.AutoAdjustPositionSwitch)::EN_AUTO_ADJUST_POSITION_SWITCH_DISABLE;
		}

		//! 开发者调试模式
		if (GetCtrl<COptionUI>(_T("DEF_6377042096DC8814BC20775A000A0559"))->IsSelected())
		{
			SystemSetup.DevelopDebugSwitch = decltype(SystemSetup.DevelopDebugSwitch)::EN_DEVELOP_DEBUG_MODE_ENABLE;
		}
		else
		{
			SystemSetup.DevelopDebugSwitch = decltype(SystemSetup.DevelopDebugSwitch)::EN_DEVELOP_DEBUG_MODE_DISABLE;
		}

		//! 交易工作时间段
		SystemSetup.SystemOperatingTimePeriod[1].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_F15D715FF21F3992F459BD4AC7C135A0"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[1].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_C9AA0A94D77F635F41CAD104C7C8FCA7"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[2].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_C6F46D1C4AE3828848DDFE312DEAFA28"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[2].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_C78F35DDA841A64330346613C34274EF"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[3].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_6FC1B276BA7CC92309C0E8E2E90F3FAA"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[3].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_D877E89FFE9A73B20C7DA51400BFF14F"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[4].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_C26809B5D882985AC69F2DD2543B7406"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[4].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_14D9DB7539860A13FFD0D50DF42B690F"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[5].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_05D9388B14343824F5DF71B204D875DA"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[5].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_55F0125614A9FBCF5D062CBBAE16EF02"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[6].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_DC396A58E7D0CA95E010A49BE1370AE3"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[6].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_1024DBA72F584A4B9B667E97354CD117"))->GetText().GetData()), true);
		SystemSetup.SystemOperatingTimePeriod[0].TimeBegin = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_2F50FE7DB9BACC7F35CA6926C0EDE13C"))->GetText().GetData()), false);
		SystemSetup.SystemOperatingTimePeriod[0].TimeEnd = sk::time::Time::TimePeriodMade(sk::StringConvert::WStringToMBytes(GetCtrl<CEditUI>(_T("DEF_C6465BB70F9EEB9A41DA4ED9119A573A"))->GetText().GetData()), true);

		for (size_t i = 0; i < _countof(SystemSetup.SystemOperatingTimePeriod); ++i)
		{
			if (SystemSetup.SystemOperatingTimePeriod[i].TimeBegin > SystemSetup.SystemOperatingTimePeriod[i].TimeEnd)
			{
				MsgBox(GetHWND(), _T("交易时间段设置不允许开始时间大于结束时间情况."), _T("错误"), MB_TOPMOST);
				return;
			}
		}


		Global::SharedGet()->m_NetSync.Request(
			shared::NetSyncSession(
				shared::NetSyncKey(
					sk::network::EnNetCmd::EN_NETCMD_000000000,
					sk::network::EnNetCmd::EN_NETCMD_300000020),
				[&]()->int
				{
					return NetSystemSetupUpdateRequest(SystemSetup);
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





}///namespace client