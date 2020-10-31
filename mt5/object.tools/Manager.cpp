#include "stdafx.h"
#include "Manager.h"

namespace manager {

	Manager::Manager()
	{
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}

	Manager::~Manager()
	{
		UnInit();
	}

	int Manager::Init()
	{
		MTAPIRES result = MT_RET_OK_NONE;
		std::wstring strMT5ApiDirectory = sk::Helper::GetCurrentProcessDirectoryW().get();
		strMT5ApiDirectory.append(L"MT5API\\");
		if ((result = m_ApiFactory.Initialize(strMT5ApiDirectory.c_str())) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("MT5: loading manager API failed {}", result);
			return -1;
		}
		unsigned int version = 0;
		if ((result = m_ApiFactory.Version(version)) != MT_RET_OK) {

			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("MT5: getting version failed {}", result);
			return -1;
		}
		else {
			//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("系统业务接口版本 [{}]", version);
		}
		if (version < MTManagerAPIVersion) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->info("MT5: wrong Manager API version, version {} required", MTManagerAPIVersion);
			return -1;
		}
		if ((result = m_ApiFactory.CreateManager(MTManagerAPIVersion, &m_pApi)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("MT5: creating manager interface failed {}", result);
			return -1;
		}
		if ((result = m_pApi->Subscribe(this)) != MT_RET_OK) {
			Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("MT5: subscribe for notifications failed {}", result);
			return -1;
		}

		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->info("MT5接口初始化完成,接口版本({})", version);
		m_ServerTimeMsc = 0;
		return 0;
	}
	int Manager::UnInit()
	{
		if (m_pApi) {
			m_pApi->DealerStop();
			m_pApi->Unsubscribe(this);
			m_pApi->Disconnect();
			m_pApi->Release();
			m_pApi = nullptr;
		}
		m_ApiFactory.Shutdown();
		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn("MT5接口卸载完成.");
		return 0;
	}
	int Manager::Open()
	{
		if (m_IsOpen.load()) return 0;

		m_IsOpenT.store(true);

		Connect();

		m_IsOpen.store(true);
		return 0;
	}
	int Manager::Close()
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

	MTAPIRES Manager::Connect()
	{
		MTAPIRES result = MT_RET_OK;

		result = m_pApi->Connect(
			sk::StringConvert::MBytesToWString(Global::SetupGet()->ManagerAddress()).c_str(),
			std::stoull(Global::SetupGet()->ManagerLogin()),
			sk::StringConvert::MBytesToWString(Global::SetupGet()->ManagerPassword()).c_str(),
			NULL,
			IMTManagerAPI::PUMP_MODE_ACTIVITY |
			IMTManagerAPI::PUMP_MODE_USERS |
			IMTManagerAPI::PUMP_MODE_ORDERS |
			IMTManagerAPI::PUMP_MODE_POSITIONS |
			IMTManagerAPI::PUMP_MODE_GROUPS |
			IMTManagerAPI::PUMP_MODE_SYMBOLS |
			IMTManagerAPI::PUMP_MODE_TIME,
			std::stoul(Global::SetupGet()->ManagerConnectTimeout()));


		if (result == MT_RET_OK)
		{
			Api()->TimeServerRequest(m_ServerTimeMsc);

			if (m_ServerTimeMsc > 0)
			{
				wchar_t times[_MAX_PATH] = { 0 };
				CMTStr strTime(times, _MAX_PATH);
				SMTFormat::FormatDateTimeMsc(strTime, m_ServerTimeMsc);
				if (strTime.Len())
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn("服务器时间[{}]", sk::StringConvert::WStringToMBytes(strTime.Str()));
				}
			}

			result = m_pApi->DealerStart();
			if (result == MT_RET_OK)
			{
				Global::CoreGet()->StatusManager(sk::network::EnNetStatus::EN_NETSTATUS_SYNCHRONIZED);
			}
			else
			{
				Global::CoreGet()->StatusManager(sk::network::EnNetStatus::EN_NETSTATUS_DISCONNECTED);
			}

			sk::container::map<UINT64, shared::PositionField> positions;
			PositionsGet(positions);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn("服务器持仓总数[{}]", positions.size());

			UsersGet(Global::SharedGet()->m_MTUsersQ);
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn("服务器用户总数[{}]", Global::SharedGet()->m_MTUsersQ.size());
		}

		if (MT_RET_OK == result)
		{
			Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error("与MT5交易服务器同步完成.");
		}

		return result;
	}


	void Manager::OnConnect()
	{
		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->warn("连接到MT5交易服务器完成");
	}
	void Manager::OnDisconnect()
	{
		Global::CoreGet()->StatusManager(sk::network::EnNetStatus::EN_NETSTATUS_DISCONNECTED);
		Global::Log(sk::EnLogType::EN_LOG_TYPE_SYSTEM)->error("与MT5交易服务器断开连接...");
	}

	void Manager::OnTradeAccountSet(
		const MTAPIRES retcode,
		const INT64 request_id,
		const IMTUser* user,
		const IMTAccount* account,
		const IMTOrderArray* orders,
		const IMTPositionArray* positions)
	{

	}

	void Manager::OnDealerResult(const IMTConfirm* result)
	{

	}
	void Manager::OnDealerAnswer(const IMTRequest* request)
	{
		if (!request) return;

		Global::SharedGet()->m_RequestCacheQ.pop(
			request->IDClient(),
			[&](const auto, auto& reqField)
			{
				if (MT_RET_REQUEST_DONE != request->ResultRetcode() && MT_RET_REQUEST_PLACED!= request->ResultRetcode())
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("Dealer error({}),position({})", request->ResultRetcode(), request->Position());
				}
				else
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_SUCCESS)->info(
						L"ReqID({}),Login({}),Position({}),Symbol({}),Volume({}) Success.",
						request->IDClient(),
						request->Login(),
						request->Position(),
						request->Symbol(),
						request->Volume());
				}

			});
	}

}///namespace manager