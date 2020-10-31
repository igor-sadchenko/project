#include "stdafx.h"
#include "Core.h"

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
	int Core::Open() 
	{
		if (m_IsOpen.load()) return 0;
		if (!extern_global_hinstance)
			return -1;
		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });
		m_Threads.emplace_back([this]() {MsgBoxT(); });
		m_Threads.emplace_back(
			[this]()
			{
				CPaintManagerUI::SetInstance(extern_global_hinstance);
				CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _TEXT("bridge.manager.skin"));
				m_pClient = new UIClient();
				m_pClient->Create(NULL, _TEXT("新生联创"), UI_WNDSTYLE_FRAME, 0L);
				m_pClient->CenterWindow();
				CPaintManagerUI::MessageLoop();
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

	void Core::ProcessT()
	{
		do {

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (!m_IsOpenT.load()) break;
		} while (1);
	}

	int Core::OnTimer(const sk::int64& _interval, const sk::intptr& _core_ptr, const sk::int64& _cur_time) {
		auto pCore = reinterpret_cast<Core*>(_core_ptr);

		if (_interval == 5000)
		{
			if (pCore->TradeUpdateFlag())
			{
				//Global::GetObjectNotify()->Push(sk::NotifyPacket((long long)EnLocalMessage::EN_LOCAL_MESSAGE_NETWORK_TRADERECORD_UPDATE));
				pCore->TradeUpdateFlag(false);
			}

			if (pCore->PositionUpdateFlag() && pCore->GetClient())
			{
				//pCore->GetClient()->OnUISystemExchangePositionUpdate();
				//pCore->PositionUpdateFlag(false);
			}
		}

		return 0;
	}
}///namespace client