#include "stdafx.h"
#include "Process.h"

namespace proxy {

	LPProcess::LPProcess()
	{

	}

	LPProcess::~LPProcess() 
	{

	}

	int LPProcess::Init()
	{
		return 0;
	}

	int LPProcess::UnInit() 
	{
		return 0;
	}

	int LPProcess::Open() 
	{
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });

		m_IsOpen.store(true);
		return 0;
	}

	int LPProcess::Close() 
	{
		if (!m_IsOpen.load()) return 0;

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << sk::Log::Format("Closing the exchange({:X}) module . . . ", ID()) << std::endl;

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) 
		{
			it->join();
		}
		m_Threads.clear();

		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << sk::Log::Format("Exchange({:X}) closing module completed .", ID()) << std::endl;
		m_IsOpen.store(false);
		return 0;
	}
	bool LPProcess::ProcessStatus() const 
	{ 
		return sk::Helper::GetProcessidFromName(sk::StringConvert::MBytesToTString(m_ObjExec).c_str()); 
	}

	void LPProcess::ProcessT() {
		do {
			if (!ProcessStatus() && !Global::SystemExit()) 
			{
				if (!sk::Helper::OpenProcess(ObjPathname(), GetGlobal()->GetCurrentProcessName(), m_ProcessInfo, Show()))
				{
					m_ProcessOpenStatus.store(false);
					break;
				}
				else
				{
					m_ProcessOpenStatus.store(true);
				}
			}
			if (!m_IsOpenT.load()) 
			{
				if (Status()) 
				{
					auto retcode = ::WaitForSingleObject(m_ProcessInfo.hProcess, 20000L);
					switch (retcode)
					{
					case WAIT_OBJECT_0:
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("({:X}) Process close success.", m_ProcessID);
					}break;
					default:
					{
						Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn("({:X}) Process close failed , system rape terminate it . . .", m_ProcessID);
						UINT swExitcode = 0;
						::TerminateProcess(m_ProcessInfo.hProcess, swExitcode);
					}
					}
					::CloseHandle(m_ProcessInfo.hProcess);
					::CloseHandle(m_ProcessInfo.hThread);
				}
				m_ProcessOpenStatus.store(false);
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		} while (1);
	}
}///namespace proxy