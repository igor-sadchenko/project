#include "stdafx.h"
#include "UIClient.h"

namespace client {

	UIClient::UIClient()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	UIClient::~UIClient()
	{
		UnInit();
	}

	int UIClient::Init()
	{
		sk::intptr out = 0;
		Global::GetApi()->GetFunction<shared::tf_api_network_tcp_client_setup>(shared::api_network, shared::api_network_tcp_client_setup)(
			Global::SetupGet()->ServerAddr().c_str(),
			out);
		NetApi<sk::network::INetworkApi>(out);
		return out > 0 ? 0 : -1;
	}

	int UIClient::UnInit()
	{
		NetApi<sk::network::INetworkApi>()->Release();
		return 0;
	}

	int UIClient::Open()
	{
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);
		m_Threads.emplace_back([&]() {UIProcessT(); });
		m_Threads.emplace_back([&]() {MDUpdateT(); });

		m_IsOpen.store(true);
		return 0;
	}

	int UIClient::Close()
	{
		if (!m_IsOpen.load()) return 0;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();

		m_IsOpen.store(false);
		return 0;
	}

	void UIClient::MsgBox(HWND hWnd, const sk::string& Text, const sk::string& CaptionText, const UINT& uType)
	{
		Global::GetCore()->m_MsgBoxPakQ.push(sk::MsgBoxPak(Text, CaptionText, uType, hWnd));
	}

	void UIClient::MDUpdateT()
	{
		do {
			auto quote = Global::SharedGet()->m_MarketDataUpdateQ.pop();
			if (quote)
			{
				Global::SharedGet()->m_MarketDataQ.push(quote->ID, *quote);
				OnUISystemMDUpdate(*quote);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) break;
		} while (1);
	}

	void UIClient::UIProcessT() 
	{
		sk::network::EnNetStatus NetStatusPrev = sk::network::EnNetStatus::EN_NETSTATUS_DISCONNECTED;
		time_t TimeintervalNetworkStatusPrev = 0;
		const time_t TimeintervalNetworkStatus = 3000;

		do {
			auto TimeNow = sk::Helper::TickCountGet<std::chrono::milliseconds>();

			if (TimeintervalNetworkStatusPrev == 0)
			{
				TimeintervalNetworkStatusPrev = TimeNow;
			}

			if (TimeNow - TimeintervalNetworkStatusPrev >= TimeintervalNetworkStatus)
			{
				TimeintervalNetworkStatusPrev = TimeNow;
				OnUINetworkStatusUpdate(NetStatusPrev);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!m_IsOpenT.load()) break;
		} while (1);
	}

	void UIClient::OnUIListSort(CControlUI* target)
	{
		if (!target) return;
		auto pThisList = (CListUI*)target->GetParent()->GetParent();
		if (!pThisList)
			return;

		target->SetUserData(pThisList->GetName());
		target->SetTagINT64(pThisList->GetItemIndex(target));
		target->GetTag() ? target->SetTag(0) : target->SetTag(1);

		pThisList->SortItems(
			[](auto param1, auto param2, auto param3)->BOOL
			{
				auto self = (CControlUI*)param3;
				if (self->GetUserData() == _T("DEF_9CD12D041D24ED8D64E170F9A3A17391"))
				{
					auto listAt1 = (CListContainerElementUI*)param1;
					auto listAt2 = (CListContainerElementUI*)param2;

					sk::string text1, text2;
					switch (self->GetTagINT64())
					{
					case 0:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"))->GetText();
					}break;
					case 1:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"))->GetText();
					}break;
					case 2:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_E2C2A58989612AE0F43D81B979A3BE98"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_E2C2A58989612AE0F43D81B979A3BE98"))->GetText();
					}break;
					case 3:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_5599F20AF76F15A37DAFCAEE5C402466"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_5599F20AF76F15A37DAFCAEE5C402466"))->GetText();
					}break;
					}
					if (sk::Helper::IsNumberStr(text1.c_str()) && sk::Helper::IsNumberStr(text2.c_str()))
					{
						double num1 = _tcstod(text1.c_str(), nullptr);
						double num2 = _tcstod(text2.c_str(), nullptr);
						if (self->GetTag())
							return num1 < num2;
						return num1 > num2;
					}
					else
					{
						if (self->GetTag())
						{
							return _tcsicmp(text1.c_str(),text2.c_str())<0;
						}
						return _tcsicmp(text1.c_str(), text2.c_str()) > 0;
					}
				}
				else if (self->GetUserData() == _T("DEF_C46E4DFFE3D9E37D1F48119C82C3A3E5"))
				{
					auto listAt1 = (CListContainerElementUI*)param1;
					auto listAt2 = (CListContainerElementUI*)param2;
					sk::string text1, text2;
					switch (self->GetTagINT64())
					{
					case 0:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_0270F9078DCD0BA2228FF73351465A0D"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_0270F9078DCD0BA2228FF73351465A0D"))->GetText();
					}break;
					case 1:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_7D2D332BCF41A3DB5A067E9299848AF1"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_7D2D332BCF41A3DB5A067E9299848AF1"))->GetText();
					}break;
					case 2:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_9E21158473D2AA536B269348425BFEC8"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_9E21158473D2AA536B269348425BFEC8"))->GetText();
					}break;
					case 3:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_C8BEEF69753EE21B7C646D22C942B65B"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_C8BEEF69753EE21B7C646D22C942B65B"))->GetText();
					}break;
					}
					if (self->GetTag())
						return text1.compare(text2) > 0;
					return text1.compare(text2) < 0;
				}
				else if (self->GetUserData() == _T("DEF_69F087E67D2D28F7B24D16DB549F9D7A"))
				{
					auto listAt1 = (CListTextElementUI*)param1;
					auto listAt2 = (CListTextElementUI*)param2;

					sk::string text1(listAt1->GetText((int)self->GetTagINT64())), text2(listAt2->GetText((int)self->GetTagINT64()));
					if (sk::Helper::IsNumberStr(text1.c_str()) && sk::Helper::IsNumberStr(text2.c_str()))
					{
						double num1 = _tcstod(text1.c_str(), nullptr);
						double num2 = _tcstod(text2.c_str(), nullptr);
						if (self->GetTag())
							return num1 < num2;
						return num1 > num2;
					}
					else
					{
						if (self->GetTag())
							return text1.compare(text2) < 0;
						return text1.compare(text2) > 0;
					}
				}

				return FALSE;
			},
			TypeToPtr(target));

		if (pThisList->GetCount())
			pThisList->SelectItem(0);



	}
}///namespace client