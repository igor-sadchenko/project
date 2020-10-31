#include "stdafx.h"
#include "Core.h"

namespace manager {

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
		m_pSystem = new System();
		return 0;
	}

	int Core::UnInit() 
	{
		SK_DELETE_PTR(m_pSystem);
		return 0;
	}

	int Core::Open() 
	{
		if (m_IsOpen.load()) { return 0; }

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ManagerT(); });
		m_Threads.emplace_back([this]() {ManagerProcessT(); });

		m_Timer.Open();
		m_Timer.TimerAppend(10000, this, OnTimer);

		m_IsOpen.store(true);
		return 0;
	}

	int Core::Close() 
	{
		if (!m_IsOpen.load()) { return 0; }

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			it->join();
		}
		m_Threads.clear();

		m_Timer.Close();
		m_IsOpen.store(false);
		return 0;
	}

	int Core::OnTimer(const sk::int64& interval, const sk::intptr& coreptr, const sk::int64& currtime)
	{
		auto pCore = reinterpret_cast<Core*>(coreptr);

		switch (interval)
		{
		case 10000:
		{
			if (pCore->PositionCloseWhile())
			{
				manager::Global::CoreGet()->ManagerGet()->PositionCloseAt(10001);
			}
		}break;
		}///switch
		return 0;
	}


	void Core::ManagerProcessT()
	{
		do {

			if (!m_pManager || !m_pManager->Api() || StatusManager() < sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				if (!m_IsOpenT.load()) break;
				continue;
			}

			if (Global::SharedGet()->m_RequestCacheQ.size() < 127)
			{
				auto req = Global::SharedGet()->m_RequestQ.pop();
				if (req)
				{
					IMTRequest* request = m_pManager->Api()->RequestCreate();
					request->Clear();
					request->Login(req->Login);
					request->Position(req->Position);
					request->Symbol(req->Symbol);
					request->Type(req->Type);
					request->Action(req->Action);
					request->Volume(req->Volume);
					request->PriceOrder(req->PriceOrder);
					request->TimeExpiration(req->TimeExpiration);
					UINT reqid = 0;
					auto mtret = m_pManager->Api()->DealerSend(request, m_pManager, reqid);
					if (MT_RET_OK == mtret)
					{
						Global::SharedGet()->m_RequestCacheQ.push(reqid, *req);
					}
					else
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("Dealer Send Error({})", mtret);
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!m_IsOpenT.load()) break;
		} while (1);
	}


	void Core::ManagerT()
	{
		do {
			if (m_StatusManager.load() < sk::network::EnNetStatus::EN_NETSTATUS_CONNECTING)
			{
				m_StatusManager.store(sk::network::EnNetStatus::EN_NETSTATUS_CONNECTING);
				if (m_pManager)
				{
					m_pManager->Close();
					SK_DELETE_PTR(m_pManager);
				}
				m_pManager = new Manager();
				m_pManager->Open();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (!m_IsOpenT.load())
			{
				if (m_pManager)
				{
					m_pManager->Close();
					SK_DELETE_PTR(m_pManager);
				}
				break;
			}
		} while (1);
	}

}///namespace manager