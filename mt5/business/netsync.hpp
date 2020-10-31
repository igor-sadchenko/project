#if !defined(AFX_SK_H__BDFD27FE_9D6A_412B_9F7A_3349C03E34EE__HEAD__)
#define AFX_SK_H__BDFD27FE_9D6A_412B_9F7A_3349C03E34EE__HEAD__

namespace shared {
	class NetSyncKey final {
	public:
		NetSyncKey()
		{

		}
		NetSyncKey(const sk::network::EnNetCmd& reqCmd, const sk::network::EnNetCmd& repCmd) :
			m_ReqCmd(reqCmd),
			m_RepCmd(repCmd)
		{

		}
		~NetSyncKey()
		{

		}
	public:
		sk::network::EnNetCmd m_ReqCmd = sk::network::EnNetCmd::EN_NETCMD_000000000;
		sk::network::EnNetCmd m_RepCmd = sk::network::EnNetCmd::EN_NETCMD_000000000;
	public:
		void operator=(const NetSyncKey& obj) {
			m_ReqCmd = obj.m_ReqCmd;
			m_RepCmd = obj.m_RepCmd;
		}
		bool operator<(const NetSyncKey& obj) const {
			if (m_ReqCmd != obj.m_ReqCmd)
				return m_ReqCmd < obj.m_ReqCmd;
			return m_RepCmd < obj.m_RepCmd;
		}
	};
	typedef std::function<int()> tfSyncReqCb;
	typedef std::function<void(const bool&, const sk::network::Header&, const sk::packet&)> tfSyncRepCb;
	class NetSyncSession final {
	public:
		NetSyncSession(const NetSyncKey& Key, const tfSyncReqCb& syncReqCb, const tfSyncRepCb& syncRepCb,const time_t& execTimeout) :
			m_Key(Key),
			m_SyncReqCb(syncReqCb),
			m_SyncRepCb(syncRepCb),
			m_ExecTimeout(execTimeout)
		{

		}
		~NetSyncSession()
		{

		}
	public:
		const tfSyncReqCb& SyncReqCb() const { return m_SyncReqCb; }
		const tfSyncRepCb& SyncRepCb() const { return m_SyncRepCb; }
		const time_t& Timeout() const { return m_ExecTimeout; }
		void NetHead(const sk::network::Header& head) { m_NetHeader = head; }
		void NetPak(const sk::packet& pak) { m_NetPak = pak; }
		const sk::network::Header& NetHead() const { return m_NetHeader; }
		const sk::packet& NetPak() const { return m_NetPak; }
		const NetSyncKey& Key() const { return m_Key; }
	private:
		const NetSyncKey m_Key;
		tfSyncReqCb m_SyncReqCb = nullptr;
		tfSyncRepCb m_SyncRepCb = nullptr;
		time_t m_ExecTimeout = 5000;//!@ ms
		sk::network::Header m_NetHeader;
		sk::packet m_NetPak;
	public:
		bool operator<(const NetSyncSession& obj) { return m_Key < obj.m_Key; }
	};
	class NetSync final {
	public:
		NetSync()
		{
			m_mutex = std::make_shared<std::mutex>();
			pThreadpool = std::make_shared<sk::thread::ThreadPool>(64);
		}
		~NetSync()
		{

		}
	public:
		bool Request(const NetSyncSession& Session)
		{
			bool result_push = false;
			std::lock_guard<std::mutex> lock(*m_mutex);
			m_SyncTaskQ.push_insert_cb(
				Session.Key(),
				Session,
				[&](const NetSyncSession& session)
				{
					if (0 == session.SyncReqCb()())
					{
						m_SyncTaskResultQ.push(session.Key(), false);
						result_push = true;
					}
				});

			if (!result_push)
			{
				return false;
			}

			auto thread_param_session = Session;
			pThreadpool->enqueue(
				[this, thread_param_session]()
				{
					auto future_result = std::async(
						[&]()
						{
							bool finish = false;
							bool timeout = false;
							time_t old = sk::Helper::TickCountGet<std::chrono::milliseconds>();
							do
							{
								m_SyncTaskResultQ.search_clear(
									thread_param_session.Key(),
									[&](auto& success, bool& itclear)
									{
										itclear = success;
										finish = success;
									});
								if (finish || (sk::Helper::TickCountGet<std::chrono::milliseconds>() - old >= thread_param_session.Timeout()))
								{
									break;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(10));
							} while (1);
						});
					auto status = future_result.wait_for(std::chrono::milliseconds(thread_param_session.Timeout())) == std::future_status::ready;
					m_SyncTaskQ.pop(
						thread_param_session.Key(),
						[&](const auto&, auto& _session)
						{
							_session.SyncRepCb()(status, _session.NetHead(), _session.NetPak());
						});
				});


			return result_push;
		}
		void Response(const sk::network::EnNetCmd& repCmd, const sk::network::Header& head, const sk::packet& pak)
		{
			std::lock_guard<std::mutex> lock(*m_mutex);
			m_SyncTaskResultQ.iterate(
				[&](const auto& key, auto& status)
				{
					if (key.m_RepCmd == repCmd)
					{
						m_SyncTaskQ.search(
							key, [&](const auto&, auto& session)
							{
								session.NetHead(head);
								session.NetPak(pak);
							});
						status = true;
					}
				});
		}
	private:
		std::shared_ptr<std::mutex> m_mutex;
		std::shared_ptr<sk::thread::ThreadPool> pThreadpool;
		sk::container::map<NetSyncKey, NetSyncSession> m_SyncTaskQ;
		sk::container::map<NetSyncKey, bool> m_SyncTaskResultQ;

	};




}///namespace shared








//!@ /*新生联创®（上海）*/
//!@ /*Sat Aug 22 08:25:36 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__BDFD27FE_9D6A_412B_9F7A_3349C03E34EE__HEAD__*/