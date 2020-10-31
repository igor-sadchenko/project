#if !defined(AFX_SKSTU_ROUTEOBJ_H__0E481_B2E67EF9_5B9D73F9868283B_26FD__HEAD__)
#define AFX_SKSTU_ROUTEOBJ_H__0E481_B2E67EF9_5B9D73F9868283B_26FD__HEAD__

namespace follow {

	class RouteObj final {
	public:
		typedef struct tagTaskNode
		{
			time_t TimeBegin;
			time_t TimeEnd;
			shared::serverplugin::OnTradeSignalField m_Request;
			shared::serverplugin::OnTradeSignalField m_Response;
		}TaskNode;
	public:
		RouteObj(const shared::Serial&, const shared::serverplugin::OnTradeSignalField&);
		~RouteObj();
	public:
		const auto& ProcessFinish() const { return m_ProcessFinishQ; }
		time_t TimeBegin() const { return m_TaskTimeBegin.load(); }
		time_t TimeEnd() const { return m_TaskTimeEnd.load(); }
		const shared::Serial& Serial() const { return m_Serial; }
		bool Finish() { return TaskCounter() == 0; }
		const auto& ProcessFinishGet() const { return m_ProcessFinishQ; }
		const auto& FollowPathOutGet() const { return m_FollowPathOutQ; }
		bool FollowPathOutAdd(const shared::PositionKey& at) { return m_FollowPathOutQ.push(at); }
		const shared::serverplugin::OnTradeSignalField& InitialSignal() const { return m_InitialSignal; }

		bool PerformTask(__in const shared::serverplugin::OnTradeSignalField&,__in const std::vector<shared::serverplugin::OnTradeSignalField>&);
		std::vector<sk::packet> Print() const;
	private:
		std::atomic<time_t> m_TaskTimeBegin = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		std::atomic<time_t> m_TaskTimeEnd = 0;
		const shared::Serial m_Serial;
		const shared::serverplugin::OnTradeSignalField m_InitialSignal;

		sk::container::map<UINT64, TaskNode> m_ProcessFinishQ;
		sk::container::set<shared::PositionKey> m_FollowPathOutQ;
	private:
		volatile long long m_TaskCounter = 0;
		long long TaskCounter() { return InterlockedExchangeAdd64(&m_TaskCounter, 0); }
		void TaskCounterIncrement() { InterlockedExchangeAdd64(&m_TaskCounter, 1); }
		void TaskCounterDecrement() { InterlockedExchangeAdd64(&m_TaskCounter, -1); }
#pragma region{Type}
	public:
		enum struct EnProcessTaskType : UINT//!@ 任务处理类型
		{
			EN_PROCESS_TASK_TYPE_FOLLOW = 0,//! 跟单
			EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN = 1,//!@ 补跟开
			EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE = 2,//!@ 补跟平
			EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY = 3,//!@ 跟随持仓编辑
			EN_PROCESS_TASK_TYPE_DEFAULT = EN_PROCESS_TASK_TYPE_FOLLOW,
		};
		enum struct EnProcessTaskFollowType : UINT
		{
			EN_PROCESS_TASK_FOLLOW_TYPE_IN = IMTDeal::EnDealEntry::ENTRY_IN,
			EN_PROCESS_TASK_FOLLOW_TYPE_OUT = IMTDeal::EnDealEntry::ENTRY_OUT,
			EN_PROCESS_TASK_FOLLOW_TYPE_INOUT = IMTDeal::EnDealEntry::ENTRY_INOUT,
			EN_PROCESS_TASK_FOLLOW_TYPE_OUT_BY = IMTDeal::EnDealEntry::ENTRY_OUT_BY,
			EN_PROCESS_TASK_FOLLOW_TYPE_DEFAULT = EN_PROCESS_TASK_FOLLOW_TYPE_IN,
		};
		enum struct EnProcessTaskModifyType : UINT
		{
			EN_PROCESS_TASK_MODIFY_TYPE_PRICESL = 1,
			EN_PROCESS_TASK_MODIFY_TYPE_PRICETP = EN_PROCESS_TASK_MODIFY_TYPE_PRICESL << 1,
			EN_PROCESS_TASK_MODIFY_TYPE_PRICETRIGGER = EN_PROCESS_TASK_MODIFY_TYPE_PRICESL << 2,
			EN_PROCESS_TASK_MODIFY_TYPE_DEFAULT = EN_PROCESS_TASK_MODIFY_TYPE_PRICETRIGGER,
		};
		enum struct EnProcessTaskPatchType : UINT
		{
			EN_PROCESS_TASK_PATCH_TYPE_IN = IMTDeal::EnDealEntry::ENTRY_IN,
			EN_PROCESS_TASK_PATHC_TYPE_OUT = IMTDeal::EnDealEntry::ENTRY_OUT,
			EN_PROCESS_TASK_PATCH_TYPE_DEFAULT = EN_PROCESS_TASK_PATCH_TYPE_IN,
		};
		enum struct EnSignalSourceType : UINT //!@ 信号来源
		{
			EN_ROUTE_SIGNAL_SOURCE_TYPE_EXTERNAL = 0,
			EN_ROUTE_SIGNAL_SOURCE_TYPE_INTERNAL = 1,
			EN_ROUTE_SIGNAL_SOURCE_TYPE_DEFAULT = EN_ROUTE_SIGNAL_SOURCE_TYPE_EXTERNAL,
		};
		enum struct EnProcessTaskSwicth : UINT
		{
			EN_PROCESS_TASK_SWITCH_ENABLE = 0,
			EN_PROCESS_TASK_SWITCH_DISABLE = 1,
			EN_PROCESS_TASK_SWITCH_DEFAULT = EN_PROCESS_TASK_SWITCH_ENABLE,
		};
	private:
		const EnProcessTaskType m_ProcessTaskType;
		std::atomic<EnProcessTaskFollowType> m_ProcessTaskFollowType = EnProcessTaskFollowType::EN_PROCESS_TASK_FOLLOW_TYPE_DEFAULT;
		std::atomic<EnProcessTaskModifyType> m_ProcessTaskModifyType = EnProcessTaskModifyType::EN_PROCESS_TASK_MODIFY_TYPE_DEFAULT;
		std::atomic<EnProcessTaskPatchType> m_ProcessTaskPatchType = EnProcessTaskPatchType::EN_PROCESS_TASK_PATCH_TYPE_DEFAULT;
		std::atomic<EnSignalSourceType> m_SignalSourceType = EnSignalSourceType::EN_ROUTE_SIGNAL_SOURCE_TYPE_DEFAULT;
		std::atomic<EnProcessTaskSwicth> m_ProcessTaskSwicth = EnProcessTaskSwicth::EN_PROCESS_TASK_SWITCH_DEFAULT;
	public:
		const EnProcessTaskType& ProcessTaskType() const { return m_ProcessTaskType; }
		EnProcessTaskFollowType ProcessTaskFollowType() const { return m_ProcessTaskFollowType.load(); }
		void ProcessTaskFollowType(const EnProcessTaskFollowType& type) { m_ProcessTaskFollowType.store(type); }
		EnProcessTaskModifyType ProcessTaskModifyType() const { return m_ProcessTaskModifyType.load(); }
		void ProcessTaskModifyType(const EnProcessTaskModifyType& type) { m_ProcessTaskModifyType.store(type); }
		EnProcessTaskPatchType ProcessTaskPatchType() const { return m_ProcessTaskPatchType.load(); }
		void ProcessTaskPatchType(const EnProcessTaskPatchType& type) { m_ProcessTaskPatchType.store(type); }
		EnSignalSourceType SignalSourceType() const { return m_SignalSourceType.load(); }
		void SignalSourceType(const EnSignalSourceType& flag) { m_SignalSourceType.store(flag); }
		EnProcessTaskSwicth ProcessTaskSwicth() const { return m_ProcessTaskSwicth.load(); }
		void ProcessTaskSwicth(const EnProcessTaskSwicth& flag) { m_ProcessTaskSwicth.store(flag); }

		const char* ProcessTaskTypeStr() const
		{
			switch (m_ProcessTaskType)
			{
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW:
			{
				return "交易跟单";
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_POSITION_MODIFY:
			{
				return "编辑跟单";
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATHC_OPEN:
			{
				return "补充跟开";
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_CLOSE:
			{
				return "补充跟平";
			}break;
			}
			return "未定义";
		}
#pragma endregion{Type}
	};




}///namespace follow




/// /*新生联创（上海）**/
/// /*2020年02月10日 19:32:58|165**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_ROUTEOBJ_H__0E481_B2E67EF9_5B9D73F9868283B_26FD__TAIL__
