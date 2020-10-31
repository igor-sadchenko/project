#if !defined(AFX_SKSTU_TRADEORDER_H__97475_F42C8695_A066D3922798171_6C6E__HEAD__)
#define AFX_SKSTU_TRADEORDER_H__97475_F42C8695_A066D3922798171_6C6E__HEAD__

namespace lmax {

	using FixSeqNum = UINT;
	using FixOrdStatusReqID = UINT;
	using FixTradeRequestID = UINT;

	class TradeOrder final :
		public sk::object::IObject,
		public shared::ILiquidityProviderTradeOrder {
		friend class Core;
	public:
		TradeOrder();
		~TradeOrder();
	private:
		int Init() override final;
		int UnInit() override final;
	public:
		int Open() override final;
		int Close() override final;
		void ProcessT() override final;
	protected:
		void OnTcpClientConnect() override final;
		void OnTcpClientDisconnect() override final;
		void OnTcpClientRead(const sk::network::Header&, const sk::packet&) override final;
		void OnTcpClientWelcome(const sk::network::Header&, const sk::packet&) override final;
	private:
		void OnFixLogon() override final;
		void OnFixLogout() override final;
		void OnFixResponse(const libfix::EnFixMessageType&, const sk::intptr&) override final;
		void OnNetLoginResult(const sk::network::Header&, const sk::packet&) override final;
		void OnNetShutdown(const sk::network::Header&, const sk::packet&) override final;
		void OnNetShowWindow(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemSetup(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemTime(const sk::network::Header&, const sk::packet&) override final;
		void OnNetProductsRequest(const sk::network::Header&, const sk::packet&) override final;
		void OnNetSystemProducts(const sk::network::Header&, const sk::packet&) override final;
		void OnNetPositionRequest(const sk::network::Header&, const sk::packet&) override final;
		void OnNetOrderStatusRequest(const sk::network::Header&, const sk::packet&) override final;
		void OnNetTradeOrderTimeoutSignal(const sk::network::Header&, const sk::packet&) override final;
	public:
		void OnNetTradeOrderSignal(const sk::network::Header&, const sk::packet&) override final;
	private:
		SKAPIRES FixOrderStatusRequest(const shared::SystemOrder&) override final;
		SKAPIRES FixNewOrderSingle(shared::SystemOrder&, UINT&) override final;
		void FixReject(const libfix::_3__*) override final;
		void FixExecutionReport(const libfix::_8__*) override final;
		void FixTradeCaptureReport(const libfix::_AE__*);
		void FixTradeCaptureReportRequestAck(const libfix::_AQ__*);

		SKAPIRES TradeDataRecovery();
		SKAPIRES TradeDataRecoverySafe();
	public:
		SKAPIRES FixTradeCaptureReportRequest(
			__in const shared::tradeorder::TradeRecordRequest&,
			__in shared::TradeTaskRouteExchange::EnTaskType TaskType = shared::TradeTaskRouteExchange::EnTaskType::EN_TASK_TYPE_TRADE_RECORD_INTERNAL_REQUEST);
	private:
		//shared::Memmap<shared::SystemID/*ExchangeProductID*/, shared::tradeorder::ExchangePosition>* m_pTradeOrderCacheQ = nullptr;
	public:
		sk::container::queue<shared::TradeTaskRouteExchange> m_RerequestQ;
		//! 交易信号请求
		sk::container::queue<shared::TradeTaskRouteExchange> m_RequestQ;
		sk::container::queue<shared::TradeTaskRouteExchange> m_RequestResQ;
		sk::container::map<FixSeqNum, shared::SystemID> m_SeqNumCIOrdIDRouteQ;
		sk::container::map<shared::SystemID, shared::TradeTaskRouteExchange> m_OrderSignalRouteQ;
		//! 交易记录请求
		sk::container::map<FixSeqNum, FixTradeRequestID> m_SeqNumTradeRequestIDRouteQ;
		sk::container::map<FixTradeRequestID, shared::TradeTaskRouteExchange> m_TradeRecordRouteQ;
		//! 订单状态请求
		sk::container::map<FixSeqNum, FixOrdStatusReqID> m_SeqNumOrdStatusReqIDRouteQ;
		sk::container::map<FixOrdStatusReqID, shared::TradeTaskRouteExchange> m_OrdStatusRouteQ;

		//!@ 每次请求交易记录的缓冲
		sk::container::map<shared::SystemID, shared::tradeorder::TradeCaptureReport> m_TradeCaptureSafeQ;
	private:
		sk::Timer m_Timer;
		static int OnTimer(const sk::int64&, const sk::intptr&, const sk::int64&);
	public:
		int TestOrderRequest(const shared::tradeorder::ExchangeOrder&);
	};
}///namespace lmax




/// /*新生联创（上海）**/
/// /*2020年05月6日 20:41:22|020**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_TRADEORDER_H__97475_F42C8695_A066D3922798171_6C6E__TAIL__