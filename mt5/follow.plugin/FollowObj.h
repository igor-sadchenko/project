#if !defined(AFX_SKSTU_TRADE_FOLLOW_H__8A4AB_4AA88205_B42603D1F9F69D0_F8B0__HEAD__)
#define AFX_SKSTU_TRADE_FOLLOW_H__8A4AB_4AA88205_B42603D1F9F69D0_F8B0__HEAD__

namespace follow {
	class FollowObj final : public sk::object::IObject {
	public:
		FollowObj();
		~FollowObj();
	public:
		int Open() override final;
		int Close() override final;
	protected:
		int Init() override final;
		int UnInit() override final;
	public:
		shared::DiskLogEx* DiskLogGet() const { return m_pDiskLog; }
		shared::Memmap<shared::FollowKey, shared::FollowField>* FollowconObj() const { return m_pFollowconObj; }

		static std::shared_ptr<shared::ConGroupSymbolField> GroupConfigGet(__in const sk::uint64&, __in const sk::stringw&);
		static std::shared_ptr<shared::ConGroupField> GroupConfigGet(__in const sk::uint64&);
		static SKAPIRES FollowMadeIn(__inout shared::SignalField&, __in const shared::FollowField&, __inout shared::RequestField&);
		static SKAPIRES FollowMadeOut(__inout shared::SignalField&, __in const shared::FollowField&, __in const shared::PositionField&, __inout shared::RequestField&);
	private:
		SKAPIRES FollowProcessFollow(__in RouteObj*, __inout shared::serverplugin::OnTradeSignalField&, __out std::vector<shared::serverplugin::OnTradeSignalField>&);
		SKAPIRES FollowProcessModify(__in const shared::PositionField&);
		SKAPIRES FollowProcessPatchIn(__in const shared::FollowField&);
		SKAPIRES FollowProcessRefollowIn(__in const shared::FollowField&);
		SKAPIRES FollowProcessPatchOut();
		SKAPIRES FollowProcessPathSLTP();
		
		//!签名 : FollowPatchGo
		//!说明 : 补仓/强跟 算法
		//!注意 : 
		//!日期 : Sun Oct 18 08:02:36 UTC+0800 2020
		SKAPIRES FollowPatchGo(const shared::FollowField& signal, const UINT64& position,std::vector<shared::FollowField>& signals,std::vector<shared::serverplugin::OnTradeSignalField>& patch);
	public:
		SKAPIRES FollowProcessPatchIn();//!@ 平台强跟
	private:
		void FollowLogT();
		void FollowModifyT();
		void FollowExecuteT();
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
	private:
		sk::Timer m_Timer;
		shared::DiskLogEx* m_pDiskLog= nullptr;
		shared::Memmap<shared::FollowKey, shared::FollowField>* m_pFollowconObj = nullptr;
	public:
		/*交易跟单信号*/
		sk::container::queue<shared::serverplugin::OnTradeSignalField> m_TradeFollowSignalQ;
		/*持仓跟单信号*/
		sk::container::queue<shared::PositionField> m_PositionFollowSignalQ;
		/*追加跟(被动) : 强跟/补仓/追加*/
		sk::container::queue<shared::FollowField> m_FollowPatchInQ;
		/*跟漏平(主动) : 修正跟单关系表 / 跟漏平任务队列*/
		sk::container::map<shared::PositionKey/*follow*/, shared::PositionField/*signal*/> m_FollowPatchOutQ;
		/*更新持仓路由 -- 用于过滤信号*/
		sk::container::map<shared::PositionKey/*target*/, shared::serverplugin::OnTradeSignalField/*targetField*/> m_PositionUpdateRouteQ;
	public://! 跟单回路关系
		/*IMTRequest::ID@shared::FollowRouteField*/
		sk::container::map<sk::uint32, shared::FollowRouteField> m_FollowProcessReqIDRouteQ;
		/*IMTOrder::OrderID@shared::FollowRouteField*/
		sk::container::map<sk::uint64, shared::FollowRouteField> m_FollowProcessOrdIDRouteQ;
		/*IMTPosition::Position@shared::PositionField*/
		sk::container::map<sk::uint64, shared::FollowRouteField> m_FollowProcessPosIDRouteQ;
		/*IMTDeal::Deal@shared::FollowRouteField*/
		sk::container::map<sk::uint64, shared::FollowRouteField> m_FollowProcessDealIDRouteQ;
		//!@ 路由任务队列
		sk::container::map<shared::Serial, RouteObj*> m_FollowProcessRouteTaskQ;
		sk::container::map<shared::serverplugin::OnTradeSignalKey, RouteObj*> m_FollowProcessRouteTaskReqIDQ;
		//!@ 2020年5月21日
		sk::container::map<UINT64/*OrderID*/, shared::RequestField> m_OrdIDReqFieldQ;
	};
}///follow




/// /*新生联创（上海）**/
/// /*2019年10月28日 20:34:59|901**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_TRADE_FOLLOW_H__8A4AB_4AA88205_B42603D1F9F69D0_F8B0__TAIL__