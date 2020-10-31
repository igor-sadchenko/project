#include "stdafx.h"
#include "Core.h"
#include "resource.h"

namespace client {

	Core::Core()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Core::~Core()
	{
		UnInit();
	}

	int Core::Init()
	{
		return 0;
	}

	int Core::UnInit()
	{
		return 0;
	}
	int Core::Open() {
		if (m_IsOpen.load()) return 0;
		if (!extern_global_hinstance)
			return -1;
		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
		m_Threads.emplace_back([this]() {MsgBoxT(); });
		m_Threads.emplace_back(
			[this]()
			{
				LPBYTE pBufferRes = nullptr;
				CPaintManagerUI::SetInstance(extern_global_hinstance);
#if defined(_DEBUG)
#if 1
				//! 从资源加载
				HRSRC pResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_SKIN), _T("SKINRES"));
				if (pResource)
				{
					HGLOBAL pGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), pResource);
					if (pGlobal)
					{
						DWORD dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), pResource);
						pBufferRes = new BYTE[dwSize];
						memcpy(pBufferRes, (BYTE*)::LockResource(pGlobal), dwSize);
						CPaintManagerUI::SetResourceZip(pBufferRes, dwSize);
					}
					::FreeResource(pResource);
				}
#else
				//! 从ZIP文件加载资源
				CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
				CPaintManagerUI::SetResourceZip(_TEXT("follow.manager.skin"), false, _TEXT("www.skstu.com"));
#endif
#else
				//! 正常加载资源
				CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _TEXT("follow.manager.skin"));
#endif
				m_pClient = new UIClient();
				m_pClient->Create(NULL, _TEXT("新生联创"), UI_WNDSTYLE_FRAME, 0L);
				m_pClient->CenterWindow();
				CPaintManagerUI::MessageLoop();
				SK_DELETE_PTR_BUFFER(pBufferRes);
			});

		m_Timer.Open();
		m_Timer.TimerAppend(5000, this, OnTimer);

		m_IsOpen.store(true);
		return 0;
	}
	int Core::Close() {
		if (!m_IsOpen.load()) return 0;

		m_Timer.Close();

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_IsOpen.store(false);
		return 0;
	}

	void Core::ProcessT() {
		do {

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_IsOpenT.load()) break;
		} while (1);
	}

	void Core::MsgBoxT() {
		do {
			auto msg = m_MsgBoxPakQ.pop();
			if (msg)
			{
				MessageBox(msg->hWnd, msg->Text.c_str(), msg->CaptionText.c_str(), msg->uType);
			}
			if (!m_IsOpenT.load()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		} while (1);
	}

	int Core::OnTimer(const sk::int64& _interval, const sk::intptr& _core_ptr, const sk::int64& _cur_time) {
		auto pCore = reinterpret_cast<Core*>(_core_ptr);

		if (_interval == 5000)
		{
			if (pCore->TradeUpdateFlag())
			{
				pCore->GetClient()->OnUIFollowRecordUpdate();
				pCore->TradeUpdateFlag(false);
			}
		}

		return 0;
	}

}///namespace client