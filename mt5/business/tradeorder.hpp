#if !defined(AFX_SKSTU_TRADEORDER_H__D8A50_5FA693FB_1852BB2AC13DECB_A330__HEAD__)
#define AFX_SKSTU_TRADEORDER_H__D8A50_5FA693FB_1852BB2AC13DECB_A330__HEAD__

#include <Windows.h>
/****
* System trading orders the bridge infrastructure
*/
namespace shared {
	namespace tradeorder {
#pragma pack(push,1)
		typedef double Price;
		typedef INT Spread;
		typedef Instrument Symbol;
		typedef UINT64 MTVolume;
		typedef UINT64 MTOrderID;
		typedef double LPVolume;
		/*typedef CHAR LPOrderID[32];*/
		/*typedef CHAR LPClOrdID[32];*/
		typedef CHAR LPPosition[32];
		typedef CHAR MTOrderExternalID[32];
		typedef long double LDOUBLE;

		const double LPVOLUMEDIV = 10000.0;

		typedef struct tagLPOrderID final
		{
			CHAR OrderID[32];

			tagLPOrderID() { memset(this, 0x00, sizeof(*this)); }
			tagLPOrderID(const CHAR* ordID) { *this = ordID; }
			void operator=(const tagLPOrderID& obj)
			{
				sk::SafeCopyA(OrderID, obj.OrderID, _countof(OrderID));
			}
			void operator=(const CHAR* obj)
			{
				if (obj)
				{
					sk::SafeCopyA(OrderID, obj, _countof(OrderID));
				}
			}
			bool operator<(const tagLPOrderID& obj) const
			{
				return sk::SafeCmpA(OrderID, obj.OrderID, _countof(OrderID)) < 0;
			}
			const CHAR* str() const
			{
				return OrderID;
			}
		}LPOrderID;

		enum struct EnOrdExecType : CHAR
		{
			EN_ORD_EXECTYPE_NEW = '0',
			EN_ORD_EXECTYPE_CANCELLED = '4',
			EN_ORD_EXECTYPE_REPLACED = '5',
			EN_ORD_EXECTYPE_PENDINGCANCEL = '6',
			EN_ORD_EXECTYPE_REJECTED = '8',
			EN_ORD_EXECTYPE_PENDINGNEW = 'A',
			EN_ORD_EXECTYPE_ORDERSTATUS = 'I',
			EN_ORD_EXECTYPE_PENDINGREPLACE = 'E',
			EN_ORD_EXECTYPE_TRADE = 'F',
		};

		enum struct EnOrdStatus : CHAR
		{
			EN_ORD_STATUS_NEW = '0',
			EN_ORD_STATUS_PARTIALLYFILLED = '1',
			EN_ORD_STATUS_FILLED = '2',
			EN_ORD_STATUS_CANCELLED = '4',
			EN_ORD_STATUS_PENDINGCANCEL = '6',
			EN_ORD_STATUS_REJECTED = '8',
			EN_ORD_STATUS_PENDINGNEW = 'A',
			EN_ORD_STATUS_PENDINGREPLACE = 'E',
		};

		enum struct EnOrdSide : CHAR
		{
			EN_ORD_SIDE_BUY = '1',
			EN_ORD_SIDE_SELL = '2',
		};
		enum struct EnOrdType : CHAR
		{
			EN_ORD_TYPE_MARKET = '1',
			EN_ORD_TYPE_LIMIT = '2',
			EN_ORD_TYPE_STOP = '3',
			EN_ORD_TYPE_STOPLIMIT = '4',
		};

		enum struct EnOrdTimeInForce : CHAR
		{
			//!@ LMAX : 0 1 3 4
			//!@ CFH : 0 1 3 4 6
			EN_ORD_TIMEINFORCE_DAY = '0',//! Day(trading session)
			EN_ORD_TIMEINFORCE_GTC = '1',//! GoodTillCancel
			EN_ORD_TIMEINFORCE_IOC = '3',//! Immediate or Cancel
			EN_ORD_TIMEINFORCE_FOK = '4',//! FillorKill
			EN_ORD_TIMEINFORCE_GTD = '6',//! GoodTillDate 
		};
		enum struct EnPositionEffect : CHAR
		{
			EN_POSITION_EFFECT_OPEN = 'O',
			EN_POSITION_EFFECT_CLOSE = 'C',
		};
		enum struct EnSessionRejectReason : UINT {
			EN_SESSION_REJECT_REASON_0 = 0,//！Invalid tag number
			EN_SESSION_REJECT_REASON_1 = 1,//！Required tag missing
			EN_SESSION_REJECT_REASON_2 = 2,//！Tag not defined for this message type
			EN_SESSION_REJECT_REASON_3 = 3,//！Undefined Tag
			EN_SESSION_REJECT_REASON_4 = 4,//！Tag specified without a value
			EN_SESSION_REJECT_REASON_5 = 5,//！Value is incorrect (out of range) for this tag
			EN_SESSION_REJECT_REASON_6 = 6,//！Incorrect data format for value
			EN_SESSION_REJECT_REASON_7 = 7,//！Decryption problem
			EN_SESSION_REJECT_REASON_8 = 8,//！Signature problem
			EN_SESSION_REJECT_REASON_9 = 9,//！CompID problem
			EN_SESSION_REJECT_REASON_10 = 10,//！SendingTime accuracy problem
			EN_SESSION_REJECT_REASON_11 = 11,//！Invalid MsgType
			EN_SESSION_REJECT_REASON_12 = 12,//！XML Validation error
			EN_SESSION_REJECT_REASON_13 = 13,//！Tag appears more than once
			EN_SESSION_REJECT_REASON_14 = 14,//！Tag specified out of required order 
			EN_SESSION_REJECT_REASON_15 = 15,//！Repeating group fields out of order
			EN_SESSION_REJECT_REASON_16 = 16,//！Incorrect NumInGroup count for repeating group
			EN_SESSION_REJECT_REASON_17 = 17,//！Non“data”value includes field delimiter (SOH character)
			EN_SESSION_REJECT_REASON_99 = 99,//！Other
		};

		enum struct EnOrdRejReason : UINT
		{
			EN_ORDREJ_REASON_0 = 0,//! Broker / Exchange option
			EN_ORDREJ_REASON_1 = 1,//! Unknown symbol
			EN_ORDREJ_REASON_2 = 2,//! Exchange closed
			EN_ORDREJ_REASON_3 = 3,//! Order exceeds limit
			EN_ORDREJ_REASON_4 = 4,//! Too late to enter
			EN_ORDREJ_REASON_5 = 5,//! Unknown Order
			EN_ORDREJ_REASON_6 = 6,//! Duplicate Order (e.g. duplicate CIOrdID ())
			EN_ORDREJ_REASON_7 = 7,//! Duplicate of a verbally communicated order
			EN_ORDREJ_REASON_8 = 8,//! Stale Order
			EN_ORDREJ_REASON_9 = 9,//! Trade Along required
			EN_ORDREJ_REASON_10 = 10,//! Invalid Investor ID
			EN_ORDREJ_REASON_11 = 11,//! Unsupported order characteristic
			EN_ORDREJ_REASON_12 = 12,//! Surveillence Option
			EN_ORDREJ_REASON_13 = 13,//! Incorrect quantity
			EN_ORDREJ_REASON_14 = 14,//! Incorrect allocated quantity
			EN_ORDREJ_REASON_15 = 15,//! Unknown account(s)
			EN_ORDREJ_REASON_99 = 99,//! Other
		};

		enum struct EnOrderAction : UINT
		{
			EN_ORDER_ACTION_NEW = 0,//! 新建定单
			EN_ORDER_ACTION_CANCEL = 1,//! 撤销定单
			EN_ORDER_ACTION_MODIFY = 2,//! 编辑定单
		};

		enum struct EnPositionAction :UINT
		{
			EN_POSITION_ACTION_NULL = 0,//! 无状态
			EN_POSITION_ACTION_OPEN = 1,//! 开仓
			EN_POSITION_ACTION_IN = 2,//! 加仓
			EN_POSITION_ACTION_OUT = 3,//! 减仓
			EN_POSITION_ACTION_CLOSE = 4,//! 平仓
		};

		enum struct MTTRADERES : UINT
		{
			MT_RET_REQUEST_INWAY = 10001,//!正要进行请求
			MT_RET_REQUEST_ACCEPTED = 10002,//!已接受的请求
			MT_RET_REQUEST_PROCESS = 10003,//!已处理的请求
			MT_RET_REQUEST_REJECT = 10006,//!已拒绝的请求
			MT_RET_REQUEST_CANCEL = 10007,//!已取消的请求
			MT_RET_REQUEST_DONE = 10009,//!已实现的请求
			MT_RET_REQUEST_DONE_PARTIAL = 10010,//!已部分实现的请求
			MT_RET_REQUEST_ERROR = 10011,//!请求的常规错误
			MT_RET_REQUEST_TIMEOUT = 10012,//!请求已超时
			MT_RET_REQUEST_INVALID = 10013,//!无效请求
			MT_RET_REQUEST_INVALID_VOLUME = 10014,//!无效量
			MT_RET_REQUEST_INVALID_PRICE = 10015,//!无效价格
			MT_RET_REQUEST_TRADE_DISABLED = 10017,//!禁用交易
			MT_RET_REQUEST_MARKET_CLOSED = 10018,//!关闭市场
			MT_RET_REQUEST_NO_MONEY = 10019,//!没有足够的钱款
			MT_RET_REQUEST_TOO_MANY = 10024,//!太多交易请求
			MT_RET_REQUEST_LIMIT_ORDERS = 10033,//!已达到订单数的限制
			MT_RET_REQUEST_LIMIT_VOLUME = 10034,//!已达到订单数的限制
			MT_RET_REQUEST_INVALID_ORDER = 10035,//!无效或被禁止的订单类型
			MT_RET_REQUEST_POSITION_CLOSED = 10036,//!已经平仓
			MT_RET_REQUEST_INVALID_CLOSE_VOLUME = 10038,//!要平仓的量超过持仓的当前量
			MT_RET_REQUEST_CONNECTION = 10031,//!无连接
			MT_RET_REQUEST_INVALID_FILL = 10030,//!成交模式不受支持
			MT_RET_REQUEST_PRICE_OFF = 10021,//! 无价格
		};


		enum struct EnTradeRequestStatus : UINT
		{
			EN_TRADE_REQUEST_STATUS_ACCEPTED = 0,
			EN_TRADE_REQUEST_STATUS_REJECTED = 2,
		};
		enum struct EnTradeRequestResult : UINT
		{
			EN_TRADE_REQUEST_RESULT_SUCCESSFUL = 0,
			EN_TRADE_REQUEST_RESULT_OTHER = 99,
		};


		//!@ 交易报表
		typedef struct tagExecutionReport final
		{
			CHAR SecurityID[32];//! 48
			CHAR SecurityIDSource[32];//! 22
			CHAR SecondaryExecID[32];//! 527
			CHAR OrdStatusReqID[32];//! 790
			CHAR TransactTime[32];//! 60
			CHAR OrigClOrdID[64];//! 41
			CHAR ExecID[32];//! 17
			CHAR OrderID[64];//! 37
			CHAR CIOrdID[64];//! 11
			CHAR MassStatusReqId[64];//! 584
			CHAR ListId[64];//! 66
			CHAR Account[32];//! 1
			CHAR Symbol[32];//! 55
			CHAR SettlDate[32];//! 64
			CHAR Currency[32];//! 15
			CHAR ExpireDate[32];//! 432
			CHAR CommCurrency[32]; //! 479
			CHAR Text[256];//! 58

			CHAR ExecType; //! 150
			CHAR OrdStatus;//! 39
			CHAR Side;//! 54
			double OrderQty;//! 38
			CHAR OrdType;//! 40
			double Price;//! 44
			double StopPx;//! 99
			CHAR TimeInForce;//! 59
			double LastQty;//! 32
			double LastPx;//! 31
			double LeavesQty;//! 151
			double CumQty;//! 14
			double AvgPx;//! 6
			double Commission;//! 12
			CHAR CommType;//! 13
			CHAR HandInst;//! 21
			INT OrdRejReason;//! 103
			double MarkUp;//! 5001
			INT Track;//! 5003

			tagExecutionReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagExecutionReport& obj) { memcpy(this, &obj, sizeof(*this)); }

			template<typename Tag8>
			void Assign(const Tag8& in)
			{
				if (!in._48.empty()) { sk::SafeCopyA(SecurityID, in._48.c_str(), _countof(SecurityID)); }
				if (!in._22.empty()) { sk::SafeCopyA(SecurityIDSource, in._22.c_str(), _countof(SecurityIDSource)); }
				if (!in._527.empty()) { sk::SafeCopyA(SecondaryExecID, in._527.c_str(), _countof(SecondaryExecID)); }
				if (!in._790.empty()) { sk::SafeCopyA(OrdStatusReqID, in._790.c_str(), _countof(OrdStatusReqID)); }
				if (!in._60.empty()) { sk::SafeCopyA(TransactTime, in._60.c_str(), _countof(TransactTime)); }
				if (!in._17.empty()) { sk::SafeCopyA(ExecID, in._17.c_str(), _countof(ExecID)); }
				if (!in._37.empty()) { sk::SafeCopyA(OrderID, in._37.c_str(), _countof(OrderID)); }
				if (!in._11.empty()) { sk::SafeCopyA(CIOrdID, in._11.c_str(), _countof(CIOrdID)); }
				if (!in._584.empty()) { sk::SafeCopyA(MassStatusReqId, in._584.c_str(), _countof(MassStatusReqId)); }
				if (!in._66.empty()) { sk::SafeCopyA(ListId, in._66.c_str(), _countof(ListId)); }
				if (!in._1.empty()) { sk::SafeCopyA(Account, in._1.c_str(), _countof(Account)); }
				if (!in._55.empty()) { sk::SafeCopyA(Symbol, in._55.c_str(), _countof(Symbol)); }
				if (!in._64.empty()) { sk::SafeCopyA(SettlDate, in._64.c_str(), _countof(SettlDate)); }
				if (!in._15.empty()) { sk::SafeCopyA(Currency, in._15.c_str(), _countof(Currency)); }
				if (!in._432.empty()) { sk::SafeCopyA(ExpireDate, in._432.c_str(), _countof(ExpireDate)); }
				if (!in._479.empty()) { sk::SafeCopyA(CommCurrency, in._479.c_str(), _countof(CommCurrency)); }
				if (!in._58.empty()) { sk::SafeCopyA(Text, in._58.c_str(), _countof(Text)); }
				if (!in._150.empty()) { ExecType = in._150[0]; }
				if (!in._39.empty()) { OrdStatus = in._39[0]; }
				if (!in._54.empty()) { Side = in._54[0]; }
				if (!in._13.empty()) { CommType = in._13[0]; }
				if (!in._21.empty()) { HandInst = in._21[0]; }
				if (!in._40.empty()) { OrdType = in._40[0]; }
				if (!in._59.empty()) { TimeInForce = in._59[0]; }
				if (!in._103.empty()) { OrdRejReason = strtol(in._103.c_str(), nullptr, 10); }
				if (!in._5003.empty()) { Track = strtol(in._5003.c_str(), nullptr, 10); }
				if (!in._38.empty()) { OrderQty = strtod(in._38.c_str(), nullptr); }
				if (!in._44.empty()) { Price = strtod(in._44.c_str(), nullptr); }
				if (!in._99.empty()) { StopPx = strtod(in._99.c_str(), nullptr); }
				if (!in._32.empty()) { LastQty = strtod(in._32.c_str(), nullptr); }
				if (!in._31.empty()) { LastPx = strtod(in._31.c_str(), nullptr); }
				if (!in._151.empty()) { LeavesQty = strtod(in._151.c_str(), nullptr); }
				if (!in._14.empty()) { CumQty = strtod(in._14.c_str(), nullptr); }
				if (!in._6.empty()) { AvgPx = strtod(in._6.c_str(), nullptr); }
				if (!in._12.empty()) { Commission = strtod(in._12.c_str(), nullptr); }
				if (!in._5001.empty()) { MarkUp = strtod(in._5001.c_str(), nullptr); }
			}
		}ExecutionReport; const size_t LENEXECUTIONREPORT = sizeof(ExecutionReport);
		//!@ 交易所持仓
		typedef struct tagExchangePosition
		{
			unsigned short head;

			SystemID ID;//! 交易所代码	

			CHAR SecurityID[32];//! 48
			CHAR SecurityIDSource[32];//! 22
			CHAR SecondaryExecID[32];//! 527
			CHAR OrdStatusReqID[32];//! 790
			CHAR TransactTime[32];//! 60
			CHAR OrigClOrdID[64];//! 41
			CHAR ExecID[32];//! 17
			CHAR OrderID[64];//! 37
			CHAR CIOrdID[64];//! 11
			CHAR MassStatusReqId[64];//! 584
			CHAR ListId[64];//! 66
			CHAR Account[32];//! 1
			CHAR Symbol[32];//! 55
			CHAR SettlDate[32];//! 64
			CHAR Currency[32];//! 15
			CHAR ExpireDate[32];//! 432
			CHAR CommCurrency[32]; //! 479

			CHAR ExecType; //! 150
			CHAR OrdStatus;//! 39
			CHAR Side;//! 54
			CHAR CommType;//! 13
			CHAR HandInst;//! 21
			CHAR OrdType;//! 40
			CHAR TimeInForce;//! 59

			INT OrdRejReason;//! 103
			double OrderQty;//! 38
			double Price;//! 44
			double StopPx;//! 99
			double LastQty;//! 32
			double LastPx;//! 31
			double LeavesQty;//! 151
			double CumQty;//! 14
			double AvgPx;//! 6
			double Commission;//! 12		
			double MarkUp;//! 5001
			INT Track;//! 5003

			INT64 TotalQty;//!@ 总成交量 == CumQty+->

			CHAR Reserved[69];

			unsigned short tail;

			void operator=(const tagExchangePosition& obj) { memcpy(this, &obj, sizeof(*this)); head = 0xfeff; tail = 0x200b; }
			bool operator<(const tagExchangePosition& obj) const { return ID < obj.ID; }
			bool Verify() const { return head == 0xfeff && tail == 0x200b; }
			tagExchangePosition() { memset(this, 0x00, sizeof(*this)); head = 0xfeff; tail = 0x200b; }
		}ExchangePosition, ExchangePositionDisk, ExchangeHedgePosition; const size_t LENEXCHANGEPOSITION = sizeof(ExchangePosition);














		typedef struct tagTradeOrderRouteData
		{
			SKAPIRES SKRetcode;
			UINT MTRetcode;
			UINT64 MTOrderID;//!@ MTOrderID
			UINT MTRequestID;
			SystemID SystemProductID;//!@ (网关方向初始化,桥方向修正) 

			UINT64 NetKey;//!@ 
			SystemID SystemOrderID;//!@ 所有组件唯一的定单ID 上至LP下至网关 由桥系统生成

			tagTradeOrderRouteData() { memset(this, 0x00, sizeof(*this)); }
			void Assign(const tagTradeOrderRouteData* obj)
			{
				NetKey = obj->NetKey;
				SKRetcode = obj->SKRetcode;
				MTRetcode = obj->MTRetcode;
				MTOrderID = obj->MTOrderID;
				MTRequestID = obj->MTRequestID;
				SystemOrderID = obj->SystemOrderID;		
				SystemProductID = obj->SystemProductID;
			}
		}TradeOrderRouteData;

		typedef struct tagExchangeOrder final : public TradeOrderRouteData
		{
			//!说明 : 处理耗时
			//!注意 : 
			//!日期 : Mon Jun 8 15:13:43 UTC+0800 2020
			INT64 ProcessTime;
			EnOrderAction OrderAction;
			EnPositionAction PositionAction;
			EnOrdSide Side;
			EnOrdType OrdType;
			EnOrdTimeInForce TimeInForce;
			EnOrdStatus OrdStatus;
			EnOrdExecType OrdExecType;
			LPOrderID OrderID;
			//!说明 : MT的请求量与MT合约大小(ContractSize)换算后的数量
			//!注意 : 
			//!日期 : Sun Jun 7 12:59:56 UTC+0800 2020
			LDOUBLE VolumeOrder;
			//!说明 : LP的成交数量经过桥换算后的MT规格数量
			//!注意 : 
			//!日期 : Sun Jun 7 13:01:18 UTC+0800 2020
			LDOUBLE VolumeTrade;

			LDOUBLE PriceOrder;//! MT请求订单价
			LDOUBLE PriceTrade;//! LP成交价
			INT64 TransactTime;//!@ 定单报告事务的时间 LMAX(UTCTimestamp)时间
			EnOrdRejReason OrdRejReason;
			EnSessionRejectReason RejectReason;
			tagExchangeOrder() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagExchangeOrder& obj) { memcpy(this, &obj, sizeof(*this)); }
		}ExchangeOrder, ExchangeOrderAck, ExchangeOrderResult; const size_t LENEXCHANGEORDER = sizeof(ExchangeOrder);

		typedef struct tagTradeOrderExecutionReport final : public TradeOrderRouteData {
			CHAR SecurityID[32];//! 48
			CHAR SecurityIDSource[32];//! 22
			CHAR SecondaryExecID[32];//! 527
			CHAR OrdStatusReqID[32];//! 790
			CHAR TransactTime[32];//! 60
			CHAR OrigClOrdID[64];//! 41
			CHAR ExecID[32];//! 17
			CHAR OrderID[64];//! 37
			CHAR CIOrdID[64];//! 11
			CHAR MassStatusReqId[64];//! 584
			CHAR ListId[64];//! 66
			CHAR Account[32];//! 1
			CHAR Symbol[32];//! 55
			CHAR SettlDate[32];//! 64
			CHAR Currency[32];//! 15
			CHAR ExpireDate[32];//! 432
			CHAR CommCurrency[32]; //! 479
			CHAR Text[256];//! 58

			CHAR ExecType; //! 150
			CHAR OrdStatus;//! 39
			CHAR Side;//! 54
			double OrderQty;//! 38
			CHAR OrdType;//! 40
			double Price;//! 44
			double StopPx;//! 99
			CHAR TimeInForce;//! 59
			double LastQty;//! 32
			double LastPx;//! 31
			double LeavesQty;//! 151
			double CumQty;//! 14
			double AvgPx;//! 6
			double Commission;//! 12
			CHAR CommType;//! 13
			CHAR HandInst;//! 21
			INT OrdRejReason;//! 103
			double MarkUp;//! 5001
			INT Track;//! 5003

			tagTradeOrderExecutionReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagTradeOrderExecutionReport& obj) { memcpy(this, &obj, sizeof(*this)); }
		}TradeOrderExecutionReport; const size_t LENTRADEORDEREXECUTIONREPORT = sizeof(TradeOrderExecutionReport);

		typedef struct tagLPPositionReportRequest final : public TradeOrderRouteData {
			Instrument Symbol;
			SystemID ID;

			tagLPPositionReportRequest() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagLPPositionReportRequest& obj) { memcpy(this, &obj, sizeof(*this)); }
		}LPPositionReportRequest; const size_t LENLPPOSITIONREPORTREQUEST = sizeof(LPPositionReportRequest);

		typedef struct tagLPPositionReportRequestAck final : public TradeOrderRouteData {
			enum struct EnPosReqResult : int
			{
				EN_POS_REQ_RESULT_ValidRequest = 0,
				EN_POS_REQ_RESULT_InvalidRequest = 1,
				EN_POS_REQ_RESULT_NoPositionsFoundThatMatchCriteria = 2,
			};

			enum struct EnPosReqStatus : int
			{
				EN_POS_REQ_STATUS_Completed = 0,
				EN_POS_REQ_STATUS_Rejected = 2,
			};

			SKAPIRES Retcode;
			char Authcode[64];
			EnPosReqStatus PosReqStatus;//! 729
			EnPosReqResult PosReqResult;//! 728
			int TotalNumPosReports;//! total positions
			char Reason[128];

			tagLPPositionReportRequestAck() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagLPPositionReportRequestAck& obj) { memcpy(this, &obj, sizeof(*this)); }
		}LPPositionReportRequestAck; const size_t LENLPPOSITIONREPORTREQUESTACK = sizeof(LPPositionReportRequestAck);

		typedef struct tagLPPositionReport final : public TradeOrderRouteData {

			enum struct EnPosReqResult : int
			{
				EN_POS_REQ_RESULT_ValidRequest = 0,
				EN_POS_REQ_RESULT_InvalidRequest = 1,
				EN_POS_REQ_RESULT_NoPositionsFoundThatMatchCriteria = 2,
			};

			Instrument Symbol;
			SystemID ID;
			char Account[32];
			int TotalNumPosReports;//! total positions
			//!-CFH PositionReport--------------------------------
			int ContractMultiplier;
			Price SettlPrice;//!结算价
			int SettlPriceType;//!结算价类型 Always 1=Final 
			Price PriorSetlPrice;//!前期定价
			LPVolume LongPos;//! 多仓
			LPVolume ShortPos;//! 空仓
			char PosMaintRptID[64];
			char PosReqId[32];
			char ClearingBusinessDate[32];
			EnPosReqResult PosReqResult;
			//!--------------------------------------

			//!-LMAX TradeCaptureReport-------------------------------
			char TradeRequestID[17];
			bool LastRptRequested;
			char ExecID[32];
			char SecondaryExecID[32];
			LPVolume LastQty;
			Price LastPx;
			char TradeDate[32];
			char TransactTime[32];
			int NoSides;
			EnOrdSide Side;
			LPOrderID OrderID;
			SystemID CIOrdID;
			//!---------------------------------------

			tagLPPositionReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagLPPositionReport& obj) { memcpy(this, &obj, sizeof(*this)); }
		}LPPositionReport; const size_t LENLPPOSITIONREPORT = sizeof(LPPositionReport);

		typedef struct tagMTPositionReport
		{
			SystemID ID;
			Instrument Symbol;
			LPVolume Volume;
			UINT PositionAction;
			CHAR Comment[32];
			int ContractMultiplier;
			int Digits;
			Price AvgPrice;

			tagMTPositionReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagMTPositionReport& obj) { memcpy(this, &obj, sizeof(*this)); }
		}MTPositionReport; const size_t LENMTPOSITIONREPORT = sizeof(MTPositionReport);

		typedef struct tagTradeCaptureReport final
		{
			CHAR TradeRequestID[16];//tag568
			CHAR ExecID[32];//!tag17
			CHAR SecondaryExecID[32];//!527
			CHAR SecurityID[32];//! 48
			CHAR SecurityIDSource[32];//! 22
			LPVolume LastQty;//!tag32
			Price LastPx;//!tag31
			CHAR TradeDate[32];//!tag75
			CHAR TransactTime[32];//!tag60
			EnOrdSide Side;//!tag54
			LPOrderID OrderID;//!tag37
			SystemID CIOrdID;//!tag11
			CHAR Account[32];//!tag1
			tagTradeCaptureReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagTradeCaptureReport& obj) { memcpy(this, &obj, sizeof(*this)); }
		}TradeCaptureReport; const size_t LENTRADECAPTUREREPORT = sizeof(TradeCaptureReport);


		typedef struct tagPositionReport final
		{
			SystemID SystemProductID;//! 系统产品ID
			__int64 SystemOrderID;//! 系统定单ID
			__int64 Volume;//! 净额持仓量/对冲持仓量
			Instrument LPSymbol;//! LP符号
			Instrument MTSymbol;//! MT符号
			double AvgPx;//! 均价
			int Digits;//! 精度
			int ContractMultiplier;//! 合约乘数
			CHAR Comment[32];//! 注释传递

			tagPositionReport() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagPositionReport& obj) { memcpy(this, &obj, sizeof(*this)); }
		}PositionReport; const size_t LENPOSITIONREPORT = sizeof(PositionReport);

		//! 自动调仓Key
		typedef struct tagAdjustPositionKey
		{
			SystemID ExchangeID;
			LPOrderID OrderID;

			tagAdjustPositionKey() { memset(this, 0x00, sizeof(*this)); }
			tagAdjustPositionKey(const SystemID& exchangeID, const LPOrderID& orderID)
			{
				ExchangeID = exchangeID;
				OrderID = orderID;
			}
			bool operator<(const tagAdjustPositionKey& obj) const
			{
				if (ExchangeID != obj.ExchangeID)
				{
					return ExchangeID < obj.ExchangeID;
				}
				return OrderID < obj.OrderID;
			}
		}AdjustPositionKey;

		typedef struct tagTradeRecordRoute
		{
			SystemID NetKey;
			INT64 Reserve;
		}TradeRecordRoute;

		typedef struct tagTradeRecordRequest : public tagTradeRecordRoute
		{
			enum struct EnRequestFlag : UINT
			{
				EN_REQUEST_FLAG_AUTO = 0,//!自动
				EN_REQUEST_FLAG_HANDLE = 1,//!手动
			};
			SystemID ExchangeID;
			INT64 From;
			INT64 To;
			EnRequestFlag RequestFlag;

			tagTradeRecordRequest() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagTradeRecordRequest& obj) { memcpy(this, &obj, sizeof(*this)); }
		}TradeRecordRequest; const size_t LENTRADERECORDREQUEST = sizeof(TradeRecordRequest);

		typedef struct tagTradeRecordRequestAck : public tagTradeRecordRoute
		{
			SKAPIRES Retcode;
			TradeRecordRequest Request;

			tagTradeRecordRequestAck() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagTradeRecordRequestAck& obj) { memcpy(this, &obj, sizeof(*this)); }
		}TradeRecordRequestAck; const size_t LENTRADERECORDREQUESTACK = sizeof(TradeRecordRequestAck);

		typedef struct tagTradeRecordRequestResult : public tagTradeRecordRoute
		{
			SKAPIRES Retcode;
			LPVolume LastQty;//!tag32
			Price LastPx;//!tag31
			EnOrdSide Side;//!tag54
			SystemID CIOrdID;//!tag11

			SystemID ExchangeProductID;
			Instrument Symbol;//! 交易所Symbol对应到系统Symbol
			double ContractSize;//! 合约乘数/合约大小

			tagTradeRecordRequestResult() { memset(this, 0x00, sizeof(*this)); }
			void operator=(const tagTradeRecordRequestResult& obj) { memcpy(this, &obj, sizeof(*this)); }
		}TradeRecordRequestResult; const size_t LENTRADERECORDREQUESTRESULT = sizeof(TradeRecordRequestResult);
		typedef std::string TradeRecordRequestResults;


#pragma pack(pop)

		class Misc final
		{
		public:
			static void OrdSideNot(EnOrdSide& side)
			{
				if (side == EnOrdSide::EN_ORD_SIDE_BUY)
					side = EnOrdSide::EN_ORD_SIDE_SELL;
				else
					side = EnOrdSide::EN_ORD_SIDE_BUY;
			}
			static const TCHAR* StrOrdSide(const EnOrdSide& side)
			{
				switch (side)
				{
				case EnOrdSide::EN_ORD_SIDE_BUY:
				{
					return _T("多单");
				}break;
				case EnOrdSide::EN_ORD_SIDE_SELL:
				{
					return _T("空单");
				}break;
				}
				return _T("未知");
			}
			static const TCHAR* StrPositionEffect(const EnPositionEffect& effect)
			{
				switch (effect)
				{
				case EnPositionEffect::EN_POSITION_EFFECT_OPEN:
				{
					return _T("开仓");
				}break;
				case EnPositionEffect::EN_POSITION_EFFECT_CLOSE:
				{
					return _T("平仓");
				}break;
				}
				return _T("未知");
			}
			static const CHAR* StrOrdSideA(const EnOrdSide& side)
			{
				switch (side)
				{
				case EnOrdSide::EN_ORD_SIDE_BUY:
				{
					return ("多单");
				}break;
				case EnOrdSide::EN_ORD_SIDE_SELL:
				{
					return ("空单");
				}break;
				}
				return ("未知");
			}
			static const CHAR* StrPositionEffectA(const EnPositionEffect& effect)
			{
				switch (effect)
				{
				case EnPositionEffect::EN_POSITION_EFFECT_OPEN:
				{
					return ("开仓");
				}break;
				case EnPositionEffect::EN_POSITION_EFFECT_CLOSE:
				{
					return ("平仓");
				}break;
				}
				return ("未知");
			}
		};


	}///namespace tradeorder

}///namespace shared




/// /*新生联创（上海）**/
/// /*2020年01月11日 7:7:00|451**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_TRADEORDER_H__D8A50_5FA693FB_1852BB2AC13DECB_A330__TAIL__