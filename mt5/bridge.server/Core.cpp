#include "stdafx.h"
#include "Core.h"

namespace proxy {
	Core::Core() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Core::~Core() {
		UnInit();
	}

	int Core::Init() {
		/*m_pGateway = new Gateway();*/
		m_pSystem = new System();
		return 0;
	}
	int Core::UnInit() {
		SK_DELETE_PTR(m_pSystem);
		/*SK_DELETE_PTR(m_pGateway);*/

		return 0;
	}

	int Core::Open() {
		if (m_IsOpen.load()) { return 0; }
		if (GetSystem()->Open()) return -1;
		/*GatewayGet()->Open();*/

		m_Timer.Open();
		m_Timer.TimerAppend(5 * sk::ms_second, this, OnTimer);
		m_Timer.TimerAppend(10 * sk::ms_second, this, OnTimer);
		m_Timer.TimerAppend(5 * sk::ms_minute, this, OnTimer);

		m_IsOpenT.store(true);

		m_IsOpen.store(true);
		return 0;
	}
	int Core::Close() {
		if (!m_IsOpen.load()) { return 0; }

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "Exiting the core module . . . " << std::endl;

		GetSystem()->Close();

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_Timer.Close();

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << "Core module exit is complete." << std::endl;

		m_IsOpen.store(false);
		return 0;
	}

	int Core::OnTimer(const sk::int64& interval, const sk::intptr& coreptr, const sk::int64& currtime)
	{
		auto pCore = reinterpret_cast<Core*>(coreptr);
		switch (interval)
		{
		case 5 * sk::ms_second:
		{
			if (pCore->GetSystem() && pCore->GetSystem()->RecordGet())
			{
				auto logs = pCore->GetSystem()->RecordGet()->PopNewestUpdate();
				if (!logs.empty())
				{
					if (pCore->GetSystem()->GetServer())
					{
						sk::packet updates;
						for (auto& log : logs)
						{
							updates.append(log).append("@");
						}
						if (!updates.empty())
							updates.pop_back();

						if (!updates.empty())
						{
							pCore->GetSystem()->GetServer()->m_ServiceObjQ.iterate(
								[&](const auto& flag, IServiceObj* pServiceObj)
								{
									if (pServiceObj->ServiceObjFlagGet() == shared::EnServiceObjectFlag::EN_SERVICE_OBJECT_FLAG_ADMIN)
									{
										ServiceObjType<AdminObj>(pServiceObj)->SendTradeRecordUpdate(updates);
									}
								});
						}
					}
				}
			}	
		}break;
		case 10 * sk::ms_second:
		{
			if (Global::SharedGet()->m_SystemProductFinalQ.empty())
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", "提示：<系统合约代码表>未初始化，系统无法开展业务 . . .");
			}
		}break;
		case 5 * sk::ms_minute:
		{

		}break;
		}///switch
		return 0;
	}
}///namespace proxy