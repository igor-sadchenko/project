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




	void UIClient::OnUIClickListHead(CControlUI* pControl)
	{
		if (!pControl)
			return;
		auto pThisList = (CListUI*)pControl->GetParent()->GetParent();
		if (!pThisList)
			return;
		pControl->SetUserData(pThisList->GetName());
		pControl->SetTagINT64(pThisList->GetItemIndex(pControl));
		if (pControl->GetTag() != 0)
			pControl->SetTag(0);
		else
			pControl->SetTag(1);
		
		pThisList->SortItems(
			[](auto param1, auto param2, auto param3)->BOOL
			{
				auto self = PtrToType(CControlUI, param3);

				if (self->GetUserData() == _T("DEF_9CD12D041D24ED8D64E170F9A3A17391"))
				{
					auto listAt1 = (CListContainerElementUI*)param1;
					auto listAt2 = (CListContainerElementUI*)param2;

					CDuiString text1, text2;
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
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_FA9A5A6CB1914A323C7D89C04876649A"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_FA9A5A6CB1914A323C7D89C04876649A"))->GetText();
					}break;
					case 3:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_6ECC31A5BDCB11AB5AA7738A6C6396B9"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_6ECC31A5BDCB11AB5AA7738A6C6396B9"))->GetText();
					}break;
					case 4:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_3F346FFEDFE756B92F5A03E57425BE26"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_3F346FFEDFE756B92F5A03E57425BE26"))->GetText();
					}break;
					case 5:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_53FA8DFCF070E33B7F97A3F997BB42BE"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_53FA8DFCF070E33B7F97A3F997BB42BE"))->GetText();
					}break;
					case 6:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_951A56DAB5F4544C403268BB1C2B26D6"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_951A56DAB5F4544C403268BB1C2B26D6"))->GetText();
					}break;
					case 7:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_4D8D63E4C550650B3F87ED63671E6AEB"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_4D8D63E4C550650B3F87ED63671E6AEB"))->GetText();
					}break;
					case 8:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_1175CA3BC615F601DA5A2639F080E8D5"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_1175CA3BC615F601DA5A2639F080E8D5"))->GetText();
					}break;
					case 9:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_F6695B34D484B2131BBFEFFE6E12CB67"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_F6695B34D484B2131BBFEFFE6E12CB67"))->GetText();
					}break;
					case 10:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_EAE26B56A97B758401B9062686536794"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_EAE26B56A97B758401B9062686536794"))->GetText();
					}break;
					case 11:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_B19FD83748084589C79EFF1A92DB7735"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_B19FD83748084589C79EFF1A92DB7735"))->GetText();
					}break;
					case 12:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_75D730AD3D0F0E56363C54565092ED6D"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_75D730AD3D0F0E56363C54565092ED6D"))->GetText();
					}break;
					case 13:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_F852E36B7F57C543456D12F2AE99ACF6"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_F852E36B7F57C543456D12F2AE99ACF6"))->GetText();
					}break;
					case 14:
					{
						text1 = listAt1->GetSubCtrl<CLabelUI>(_T("DEF_76F0E723BE7CC7B674C20C18DC652E9A"))->GetText();
						text2 = listAt2->GetSubCtrl<CLabelUI>(_T("DEF_76F0E723BE7CC7B674C20C18DC652E9A"))->GetText();
					}break;
					}

					if (self->GetTag() != 0)
						return text1.Compare(text2) > 0;
					return text1.Compare(text2) < 0;
				}
				else if(self->GetUserData() == _T("DEF_6FD5BAFC0E9AA55532059D7B42D18D86"))
				{
					auto listAt1 = (CListTextElementUI*)param1;
					auto listAt2 = (CListTextElementUI*)param2;
					
					CDuiString text1(listAt1->GetText((int)self->GetTagINT64())), text2(listAt2->GetText((int)self->GetTagINT64()));
					if (self->GetTag())
						return text1.Compare(text2) < 0;
					return text1.Compare(text2) > 0;
				}

				return FALSE;
			},
			TypeToPtr(pControl));


			if (pThisList->GetCount())
				pThisList->SelectItem(0);
	}












}///namespace client