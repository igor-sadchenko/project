#if !defined(AFX_SKSTU_INTERFACE_H__6E945_93C00546_F381B6E39BFB249_376D__HEAD__)
#define AFX_SKSTU_INTERFACE_H__6E945_93C00546_F381B6E39BFB249_376D__HEAD__

#include "business_gateway.h"
#include "systemid.hpp"
#include "products.hpp"
#include "marketdata.hpp"
#include "tradeorder.hpp"

namespace shared {

	using namespace tradeorder;

#define ROUTE_MTCMD_HEAD 0xFEFF
#define ROUTE_MTCMD_TAIL 0x200B

#pragma pack(push,1)


#pragma region{abort mt business core}
	enum struct EnMTApiAppID : USHORT
	{
		EN_MTAPI_APPID_SKOBJ_SERVERAPI = 1,
		EN_MTAPI_APPID_SKOBJ_GATEWAYAPI = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 1,
		EN_MTAPI_APPID_SKOBJ_MANAGERAPI = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 2,
		EN_MTAPI_APPID_SKOBJ_ADMINAPI = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 3,
		EN_MTAPI_APPID_SKOBJ_WEBAPI = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 4,
		EN_MTAPI_APPID_SKOBJ_REPORTAPI = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 5,

		EN_MTAPI_APPID_SKOBJ_FIRST = EN_MTAPI_APPID_SKOBJ_SERVERAPI,
		EN_MTAPI_APPID_SKOBJ_LAST = EN_MTAPI_APPID_SKOBJ_REPORTAPI,
		EN_MTAPI_APPID_SKOBJ_MAX = EN_MTAPI_APPID_SKOBJ_SERVERAPI << 0xB,
	};
	static EnMTApiAppID operator|(const EnMTApiAppID& obj1, const EnMTApiAppID& obj2)
	{
		return EnMTApiAppID((USHORT)obj1 | (USHORT)obj2);
	}
	static bool operator&(const EnMTApiAppID& obj1, const EnMTApiAppID& obj2)
	{
		return (USHORT)obj1 & (USHORT)obj2;
	}
	static EnMTApiAppID operator|=(const EnMTApiAppID& obj1, const EnMTApiAppID& obj2)
	{
		USHORT temp = (USHORT)obj1;
		return EnMTApiAppID(temp |= (USHORT)obj2);
	}
	enum struct EnMTApiParamType : UCHAR
	{
		EN_MTAPI_PARAMTYPE_LOGIN = 1,
		EN_MTAPI_PARAMTYPE_DEAL = 2,
		EN_MTAPI_PARAMTYPE_ORDER = 3,
		EN_MTAPI_PARAMTYPE_POSITION = 4,
		EN_MTAPI_PARAMTYPE_VOLUME = 5,
		EN_MTAPI_PARAMTYPE_POSITION_PREV = 6,

		EN_MTAPI_PARAMTYPE_FIRST = EN_MTAPI_PARAMTYPE_LOGIN,
		EN_MTAPI_PARAMTYPE_LAST = EN_MTAPI_PARAMTYPE_POSITION_PREV,
	};
#pragma endregion{abort mt business core}



	//!签名 : tagTimePeriod
	//!说明 : 时间段结构
	//!注意 : 
	//!日期 : Sun Sep 6 19:56:44 UTC+0800 2020
	typedef struct tagTimePeriod final
	{
		UINT TimeBegin;
		UINT TimeEnd;

		tagTimePeriod() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagTimePeriod& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator==(const tagTimePeriod& obj) const { return TimeBegin == obj.TimeBegin ? TimeEnd == obj.TimeEnd : false; }
	}TimePeriod; const size_t LENTIMEPERIOD = sizeof(TimePeriod);



#pragma region{MT5结构映射}
	typedef UINT64 DealKey;
	typedef UINT64 DealID;

	typedef struct tagDealField
	{
		UINT64    Deal;
		wchar_t   ExternalID[32];
		UINT64    Login;
		UINT64    Dealer;
		UINT64    Order;
		UINT      Action;
		UINT      Entry;
		UINT      Digits;
		UINT      DigitsCurrency;
		double    ContractSize;
		INT64     Time;
		wchar_t   Symbol[32];
		double    Price;
		UINT64    Volume;
		double    Profit;
		double    Storage;
		double    Commission;
		double    ObsoleteValue;
		double    RateProfit;
		double    RateMargin;
		UINT64    ExpertID;
		UINT64    PositionID;
		wchar_t   Comment[32];
		double    ProfitRaw;
		double    PricePosition;
		UINT64    VolumeClosed;
		double    TickValue;
		double    TickSize;
		UINT64    Flags;
		INT64     TimeMsc;
		UINT      Reason;
		/*wchar_t   Gateway;*/
		double    PriceGateway;
		UINT      ModificationFlags;
		double    PriceSL;
		double    PriceTP;
		UINT64    VolumeExt;
		UINT64    VolumeClosedExt;

		//! extern
		UINT64 ApiDataPosition;
		UINT64 ApiDataLogin;
		UINT64 ApiDataOrder;
		UINT64 ApiDataDeal;
		UINT64 ApiDataVolume;

		tagDealField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagDealField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagDealField& obj) const { return Deal < obj.Deal; }
	}DealField; const size_t LENDEALFIELD = sizeof(DealField);


	typedef UINT64 PositionKey;
	typedef UINT64 PositionID;
	typedef UINT64 PositionKeyRetailHedged;
	typedef struct tagPositionKeyRetail final
	{
		UINT64 Login;
		wchar_t Symbol[32];

		tagPositionKeyRetail() { memset(this, 0x00, sizeof(*this)); }
		tagPositionKeyRetail(const UINT64& login, const wchar_t* symbol)
		{
			memset(this, 0x00, sizeof(*this));
			Login = login;
			sk::SafeCopyW(Symbol, symbol, _countof(Symbol));
		}
		void operator=(const tagPositionKeyRetail& obj)
		{
			memset(this, 0x00, sizeof(*this));
			Login = obj.Login;
			sk::SafeCopyW(Symbol, obj.Symbol, _countof(Symbol));
		}
		bool operator<(const tagPositionKeyRetail& obj) const
		{
			if (Login != obj.Login) { return Login < obj.Login; }
			return sk::SafeCmpW(Symbol, obj.Symbol, _countof(Symbol)) < 0;
		}
		bool operator==(const tagPositionKeyRetail& obj) const
		{
			if (Login != obj.Login) return false;
			return sk::SafeCmpW(Symbol, obj.Symbol, _countof(Symbol)) == 0;
		}
		bool operator!=(const tagPositionKeyRetail& obj) const
		{
			if (Login != obj.Login) return true;
			return sk::SafeCmpW(Symbol, obj.Symbol, _countof(Symbol)) != 0;
		}
	}PositionKeyRetail;

	typedef struct tagPositionField
	{
		UINT64    Login;
		wchar_t   Symbol[32];
		UINT      Action;
		UINT      Digits;
		UINT      DigitsCurrency;
		double    ContractSize;
		INT64     TimeCreate;
		INT64     TimeUpdate;
		double    PriceOpen;
		double    PriceCurrent;
		double    PriceSL;
		double    PriceTP;
		UINT64    Volume;
		double    Profit;
		double    Storage;
		double    ObsoleteValue;
		double    RateProfit;
		double    RateMargin;
		UINT64    ExpertID;
		UINT64    ExpertPositionID;
		wchar_t   Comment[32];
		UINT      ActivationMode;
		INT64     ActivationTime;
		double    ActivationPrice;
		UINT      ActivationFlags;
		INT64     TimeCreateMsc;
		INT64     TimeUpdateMsc;
		UINT64    Dealer;
		UINT64    Position;
		wchar_t   ExternalID[32];
		UINT      ModificationFlags;
		UINT      Reason;
		UINT64    VolumeExt;

		//! extern
		UINT64 ApiDataPosition;
		UINT64 ApiDataLogin;
		UINT64 ApiDataOrder;
		UINT64 ApiDataDeal;
		UINT64 ApiDataVolume;

		tagPositionField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagPositionField& obj) { memcpy(this, &obj, sizeof(*this)); }

		//! 本结构针对锁仓(MARGIN_MODE_RETAIL_HEDGED)持仓模式的比较(专用)
		bool operator&(const tagPositionField& obj) const
		{
			return Position == obj.Position && Login == obj.Login;
		}
		bool operator&&(const tagPositionField* obj) const
		{
			return Login == obj->Login && !sk::SafeCmpW(Symbol, obj->Symbol, _countof(Symbol));
		}
	}PositionField; const size_t LENPOSITIONFIELD = sizeof(PositionField);

	typedef struct tagSimplePositionField
	{
		UINT64 Login;
		Instrument Symbol;
		tradeorder::EnOrdSide Side;
		UINT Digits;
		double ContractSize;
		double PriceOpen;
		UINT64 Volume;
		UINT64 Position;

		UINT64 ExtSignalLogin;
		UINT64 ExtSignalPosition;
		INT64 ExtOrderTime;
		shared::SystemID ExtOrderID;
		shared::SystemID ExtExchangeID;
		shared::SystemID ExtGatewayPID;
		INT64 ExtTotalQty;

		tagSimplePositionField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagSimplePositionField& obj) { memcpy(this, &obj, sizeof(*this)); }
	}SimplePositionField; const size_t LENSIMPLEPOSITIONFIELD = sizeof(SimplePositionField);


	typedef struct tagOrderField
	{
		UINT64    Order;
		wchar_t   ExternalID[32];
		UINT64    Login;
		UINT64    Dealer;
		wchar_t   Symbol[32];
		UINT      Digits;
		UINT      DigitsCurrency;
		double    ContractSize;
		UINT      State;
		UINT      Reason;
		INT64     TimeSetup;
		INT64     TimeExpiration;
		INT64     TimeDone;
		UINT      Type;
		UINT      TypeFill;
		UINT      TypeTime;
		double    PriceOrder;
		double    PriceTrigger;
		double    PriceCurrent;
		double    PriceSL;
		double    PriceTP;
		UINT64    VolumeInitial;
		UINT64    VolumeCurrent;
		UINT64    ExpertID;
		UINT64    PositionID;
		wchar_t   Comment[32];
		UINT      ActivationMode;
		INT64     ActivationTime;
		double    ActivationPrice;
		UINT      ActivationFlags;
		INT64     TimeSetupMsc;
		INT64     TimeDoneMsc;
		double    RateMargin;
		UINT64    PositionByID;
		UINT      ModificationFlags;
		UINT64    VolumeInitialExt;
		UINT64    VolumeCurrentExt;

		tagOrderField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagOrderField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagOrderField& obj) const { return Order < obj.Order; }
	}OrderField; const size_t LENORDERFIELD = sizeof(OrderField);

	typedef struct tagUserField
	{
		UINT64    Login;
		wchar_t   Group[64];
		UINT64    CertSerialNumber;
		UINT64    Rights;
		INT64     Registration;
		INT64     LastAccess;
		wchar_t   Name[128];
		wchar_t   Company[64];
		wchar_t   Account[32];
		wchar_t   Country[32];
		UINT      Language;
		wchar_t   City[32];
		wchar_t   State[32];
		wchar_t   ZIPCode[16];
		wchar_t   Address[128];
		wchar_t   Phone[32];
		wchar_t   EMail[64];
		wchar_t   ID[32];
		wchar_t   Status[16];
		wchar_t   Comment[64];
		COLORREF  Color;
		wchar_t   PhonePassword[32];
		UINT      Leverage;
		UINT64    Agent;
		double    Balance;
		double    Credit;
		double    InterestRate;
		double    CommissionDaily;
		double    CommissionMonthly;
		double    CommissionAgentDaily;
		double    CommissionAgentMonthly;
		double    BalancePrevDay;
		double    BalancePrevMonth;
		double    EquityPrevDay;
		double    EquityPrevMonth;
		UINT      ExternalAccountTotal;
		INT64     LastPassChange;
		wchar_t   LeadSource[64];

		tagUserField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagUserField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagUserField& obj) const { return Login < obj.Login; }
	}UserField; const size_t LENUSERFIELD = sizeof(UserField);

	typedef struct tagUserSimpleField final
	{
		UINT64 Login;
		CHAR Group[64];
		tagUserSimpleField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagUserSimpleField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagUserSimpleField& obj) const { return Login < obj.Login; }
	}UserSimpleField; const size_t LENUSERSIMPLEFIELD = sizeof(UserSimpleField);

	typedef struct tagConGroupField
	{
		wchar_t   Group[64];
		UINT64    Server;
		UINT64    PermissionsFlags;
		UINT      AuthMode;
		UINT      AuthPasswordMin;
		wchar_t   Company[71];
		wchar_t   CompanyPage[256];
		wchar_t   CompanyEmail[256];
		wchar_t   CompanySupportPage[256];
		wchar_t   CompanySupportEmail[256];
		wchar_t   CompanyCatalog[64];
		wchar_t   Currency[32];
		UINT      CurrencyDigits;
		UINT      ReportsMode;
		UINT64    ReportsFlags;
		wchar_t   ReportsSMTP[64];
		wchar_t   ReportsSMTPLogin[64];
		wchar_t   ReportsSMTPPass[64];
		UINT      NewsMode;
		wchar_t   NewsCategory[64];
		UINT      NewsLangTotal;
		UINT      MailMode;
		UINT64    TradeFlags;
		double    TradeInterestrate;
		double    TradeCredit;
		UINT      MarginFreeMode;
		UINT      MarginSOMode;
		double    MarginCall;
		double    MarginStopOut;
		UINT      DemoLeverage;
		double    DemoDeposit;
		UINT      LimitHistory;
		UINT      LimitOrders;
		UINT      LimitSymbols;
		UINT      CommissionTotal;
		UINT      SymbolTotal;
		UINT      MarginFreeProfitMode;
		UINT      MarginMode;
		UINT      AuthOTPMode;
		UINT      TradeTransferMode;
		UINT64    MarginFlags;
		UINT      LimitPositions;
		wchar_t   ReportsEmail[256];

		tagConGroupField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagConGroupField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagConGroupField& obj) const { return wmemcmp(Group, obj.Group, _countof(Group)) < 0; }
	}ConGroupField; const size_t LENCONGROUP = sizeof(ConGroupField);

	typedef struct tagConSymbolField
	{
		wchar_t   Symbol[32];
		wchar_t   Path[256];
		wchar_t   ISIN[16];
		wchar_t   Description[64];
		wchar_t   International[64];
		wchar_t   Basis[32];
		wchar_t   Source[32];
		wchar_t   Page[256];
		wchar_t   CurrencyBase[16];
		UINT      CurrencyBaseDigits;
		wchar_t   CurrencyProfit[16];
		UINT      CurrencyProfitDigits;
		wchar_t   CurrencyMargin[16];
		UINT      CurrencyMarginDigits;
		COLORREF  Color;
		COLORREF  ColorBackground;
		UINT      Digits;
		double    Point;
		double    Multiply;
		UINT64    TickFlags;
		UINT      TickBookDepth;
		UINT      FilterSoft;
		UINT      FilterSoftTicks;
		UINT      FilterHard;
		UINT      FilterHardTicks;
		UINT      FilterDiscard;
		UINT      FilterSpreadMax;
		UINT      FilterSpreadMin;
		UINT      TradeMode;
		UINT      CalcMode;
		UINT      ExecMode;
		UINT      GTCMode;
		UINT      FillFlags;
		UINT      ExpirFlags;
		UINT      Spread;
		INT       SpreadBalance;
		INT       SpreadDiff;
		INT       SpreadDiffBalance;
		double    TickValue;
		double    TickSize;
		double    ContractSize;
		INT       StopsLevel;
		INT       FreezeLevel;
		UINT      QuotesTimeout;
		UINT64    VolumeMin;
		UINT64    VolumeMax;
		UINT64    VolumeStep;
		UINT64    VolumeLimit;
		UINT      MarginFlags;
		double    MarginInitial;
		double    MarginMaintenance;
		double    MarginLong;
		double    MarginShort;
		double    MarginLimit;
		double    MarginStop;
		double    MarginStopLimit;
		UINT      SwapMode;
		double    SwapLong;
		double    SwapShort;
		UINT      Swap3Day;
		INT64     TimeStart;
		INT64     TimeExpiration;
		UINT      REFlags;
		UINT      RETimeout;
		UINT      IECheckMode;
		UINT      IETimeout;
		UINT      IESlipProfit;
		UINT      IESlipLosing;
		UINT64    IEVolumeMax;
		double    PriceSettle;
		double    PriceLimitMax;
		double    PriceLimitMin;
		UINT64    TradeFlags;
		UINT      OrderFlags;
		UINT      OptionsMode;
		double    PriceStrike;
		double    MarginRateLiqudity;
		double    FaceValue;
		double    AccruedInterest;
		UINT      SpliceType;
		UINT      SpliceTimeType;
		UINT      SpliceTimeDays;
		double    MarginHedged;
		double    MarginRateCurrency;
		UINT      FilterGap;
		UINT      FilterGapTicks;
		UINT      ChartMode;
		UINT      IEFlags;
		UINT64    VolumeMinExt;
		UINT64    VolumeMaxExt;
		UINT64    VolumeStepExt;
		UINT64    VolumeLimitExt;
		UINT64    IEVolumeMaxExt;

		tagConSymbolField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagConSymbolField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagConSymbolField& obj) const { return wmemcmp(Symbol, obj.Symbol, _countof(Symbol)) < 0; }
	}ConSymbolField; const size_t LENCONSYMBOLFIELD = sizeof(ConSymbolField);

	typedef struct tagConGroupSymbolField
	{
		wchar_t   Path[256];
		UINT      TradeMode;
		UINT      TradeModeDefault;
		UINT      ExecMode;
		UINT      ExecModeDefault;
		UINT      FillFlags;
		UINT      FillFlagsDefault;
		UINT      ExpirFlags;
		UINT      ExpirFlagsDefault;
		INT       SpreadDiff;
		INT       SpreadDiffDefault;
		INT       SpreadDiffBalance;
		INT       SpreadDiffBalanceDefault;
		INT       StopsLevel;
		INT       StopsLevelDefault;
		INT       FreezeLevel;
		INT       FreezeLevelDefault;
		UINT64    VolumeMin;
		UINT64    VolumeMinDefault;
		UINT64    VolumeMax;
		UINT64    VolumeMaxDefault;
		UINT64    VolumeStep;
		UINT64    VolumeStepDefault;
		UINT64    VolumeLimit;
		UINT64    VolumeLimitDefault;
		UINT      MarginFlags;
		UINT      MarginFlagsDefault;
		double    MarginInitial;
		double    MarginInitialDefault;
		double    MarginMaintenance;
		double    MarginMaintenanceDefault;
		double    MarginLong;
		double    MarginLongDefault;
		double    MarginShort;
		double    MarginShortDefault;
		double    MarginLimit;
		double    MarginLimitDefault;
		double    MarginStop;
		double    MarginStopDefault;
		double    MarginStopLimit;
		double    MarginStopLimitDefault;
		UINT      SwapMode;
		UINT      SwapModeDefault;
		double    SwapLong;
		double    SwapLongDefault;
		double    SwapShort;
		double    SwapShortDefault;
		int       Swap3Day;
		int       Swap3DayDefault;
		UINT      RETimeout;
		UINT      RETimeoutDefault;
		UINT      IECheckMode;
		UINT      IECheckModeDefault;
		UINT      IETimeout;
		UINT      IETimeoutDefault;
		UINT      IESlipProfit;
		UINT      IESlipProfitDefault;
		UINT      IESlipLosing;
		UINT      IESlipLosingDefault;
		UINT64    IEVolumeMax;
		UINT64    IEVolumeMaxDefault;
		UINT      OrderFlags;
		UINT      OrderFlagsDefault;
		double    MarginRateInitialDefault;
		double    MarginRateMaintenanceDefault;
		double    MarginRateLiquidity;
		double    MarginRateLiquidityDefault;
		UINT      REFlags;
		UINT      REFlagsDefault;
		double    MarginHedged;
		double    MarginHedgedDefault;
		UINT      PermissionsFlags;
		double    MarginRateCurrency;
		double    MarginRateCurrencyDefault;
		UINT      BookDepthLimit;
		UINT      IEFlags;
		UINT      IEFlagsDefault;
		UINT64    VolumeMinExt;
		UINT64    VolumeMinExtDefault;
		UINT64    VolumeMaxExt;
		UINT64    VolumeMaxExtDefault;
		UINT64    VolumeStepExt;
		UINT64    VolumeStepExtDefault;
		UINT64    VolumeLimitExt;
		UINT64    VolumeLimitExtDefault;
		UINT64    IEVolumeMaxExt;
		UINT64    IEVolumeMaxExtDefault;

		tagConGroupSymbolField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagConGroupSymbolField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagConGroupSymbolField& obj) const { return wmemcmp(Path, obj.Path, _countof(Path)) < 0; }
	}ConGroupSymbolField; const size_t LENCONGROUPSYMBOLFIELD = sizeof(ConGroupSymbolField);


	typedef struct tagAccountField
	{
		UINT64    Login;
		UINT      CurrencyDigits;
		double    Balance;
		double    Credit;
		double    Margin;
		double    MarginFree;
		double    MarginLevel;
		UINT      MarginLeverage;
		double    Profit;
		double    Storage;
		double    Commission;
		double    Floating;
		double    Equity;
		UINT      SOActivation;
		INT64     SOTime;
		double    SOLevel;
		double    SOEquity;
		double    SOMargin;
		double    BlockedCommission;
		double    BlockedProfit;
		double    MarginInitial;
		double    MarginMaintenance;
		double    Assets;
		double    Liabilities;

		tagAccountField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagAccountField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagAccountField& obj) const { return Login < obj.Login; }
	}AccountField; const size_t LENACCOUNTFIELD = sizeof(AccountField);

	typedef struct tagExecutionField
	{
		UINT64    ID;
		wchar_t   ExternalID[32];
		UINT      Action;
		INT64     Datetime;
		UINT64    Login;
		wchar_t   Group[64];
		UINT64    Flags;
		wchar_t   Symbol[32];
		UINT      Digits;
		wchar_t   Comment[32];
		UINT64    Order;
		wchar_t   OrderExternalID[32];
		UINT      OrderType;
		UINT64    OrderVolume;
		double    OrderPrice;
		UINT      OrderActivationFlags;
		wchar_t   DealExternalID[32];
		UINT      DealAction;
		UINT64    DealVolume;
		UINT64    DealVolumeRemaind;
		double    DealPrice;
		wchar_t   ExternalAccount[32];
		double    OrderPriceTrigger;
		UINT      OrderTypeTime;
		INT64     OrderTimeExpiration;
		UINT      OrderTypeFill;
		INT64     EOSSessionStart;
		INT64     EOSSessionEnd;
		double    EOSPriceSettlement;
		double    EOSProfitRateBuy;
		double    EOSProfitRateSell;
		double    EOSTickValue;
		double    OrderPriceSL;
		double    OrderPriceTP;
		double    PriceGateway;
		UINT      OrderActivationMode;
		double    DealCommission;
		INT64     DatetimeMsc;
		wchar_t   SymbolNew[32];
		double    DealStorage;
		double    EOSRolloverValueLong;
		double    EOSRolloverValueShort;
		UINT      DealReason;
		UINT64    GatewayID;
		UINT64    Position;
		UINT64    PositionBy;
		wchar_t   PositionExternalID[32];
		wchar_t   PositionByExternalID[32];
		int       ExternalRetcode;
		UINT64    OrderVolumeExt;
		UINT64    DealVolumeExt;
		UINT64    DealVolumeRemaindExt;

		tagExecutionField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagExecutionField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagExecutionField& obj) const { return ID < obj.ID; }
	}ExecutionField; const size_t LENEXECUTIONFIELD = sizeof(ExecutionField);


	typedef struct tagConfirmField
	{
		UINT      ID;
		UINT      Retcode;
		UINT64    Volume;
		double    Price;
		double    TickBid;
		double    TickAsk;
		double    TickLast;
		wchar_t   Comment[32];
		UINT      Flags;
		wchar_t   DealID[32];
		wchar_t   OrderID[32];
		double    PriceGateway;
		wchar_t   PositionExternalID[32];
		int       ExternalRetcode;
		UINT64    VolumeExt;

		tagConfirmField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagConfirmField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagConfirmField& obj) const { return ID < obj.ID; }
	}ConfirmField; const size_t LENCONFIRMFIELD = sizeof(ConfirmField);

	typedef struct tagConfirmSimpleField
	{
		UINT      Retcode;
		UINT64    Volume;
		UINT64 OrderID;
		double    Price;
		double    PriceGateway;
		SystemID PositionExternalID;
		tagConfirmSimpleField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagConfirmSimpleField& obj) { memcpy(this, &obj, sizeof(*this)); }
	}ConfirmSimpleField; const size_t LENCONFIRMSIMPLEFIELD = sizeof(ConfirmSimpleField);

	typedef struct tagRequestField
	{
		UINT      ID;
		UINT64    Login;
		wchar_t   Group[64];
		wchar_t   Symbol[32];
		UINT      Digits;
		UINT      Action;
		INT64     TimeExpiration;
		UINT      Type;
		UINT      TypeFill;
		UINT      TypeTime;
		UINT64    Flags;
		UINT64    Volume;
		UINT64    Order;
		wchar_t   OrderExternalID[32];
		double    PriceOrder;
		double    PriceTrigger;
		double    PriceSL;
		double    PriceTP;
		UINT64    PriceDeviation;
		double    PriceDeviationTop;
		double    PriceDeviationBottom;
		wchar_t   Comment[32];
		UINT		ResultRetcode;
		UINT64    ResultDealer;
		UINT64    ResultDeal;
		UINT64    ResultOrder;
		UINT64    ResultVolume;
		double    ResultPrice;
		double    ResultDealerBid;
		double    ResultDealerAsk;
		double    ResultDealerLast;
		double    ResultMarketBid;
		double    ResultMarketAsk;
		double    ResultMarketLast;
		wchar_t   ResultComment[32];
		wchar_t   ExternalAccount[32];
		UINT      IDClient;
		UINT64    SourceLogin;
		UINT64    Position;
		UINT64    PositionBy;
		wchar_t   PositionExternalID[32];
		wchar_t   PositionByExternalID[32];
		UINT64    VolumeExt;
		UINT64    ResultVolumeExt;

		tagRequestField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagRequestField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagRequestField& obj) const { return ID < obj.ID; }
	}RequestField; const size_t LENREQUESTFIELD = sizeof(RequestField);

	typedef struct tagRequestSimpleField
	{
		UINT      ID;
		UINT64    Login;
		Instrument   Symbol;
		UINT64    Volume;
		UINT64    Order;
		double    PriceOrder;
		UINT      Action;
		UINT      Type;
		UINT64    Flags;
		SystemID   PositionExternalID;
		SystemID SystemProductID;
		tagRequestSimpleField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagRequestSimpleField& obj) { memcpy(this, &obj, sizeof(*this)); }
	}RequestSimpleField; const size_t LENREQUESTSIMPLEFIELD = sizeof(RequestSimpleField);
#pragma endregion{MT5结构映射}

	//!@ 跟单系统接口
		/* This is a signal source transaction interface
	* To express the necessary information about a signal source
	* This is the only way the system can express the source
	*/
	typedef struct tagSignalField {

		sk::uint64 Login;/*login id*/
		sk::uint64 Deal;/*deal id*/
		sk::uint64 Order;/*order id*/
		sk::uint64 Position;/*position id*/
		sk::uint32 PositionAction;
		sk::uint64 Volume;/*turnover 这一笔下单量*/
		sk::uint64 VolumeClosed;/*close volume 平仓量*/
		sk::uint32 Entry;/*entry type*/
		sk::uint32 Action;/*direction type or action type*/
		sk::real64 Price;/*order price*/
		sk::uint32 Digits;/*价格点位*/
		sk::real64 PriceSL;/**/
		sk::real64 PriceTP;/**/
		sk::uint32 MarginMode;/*持仓模式*/
		sk::int64 Time;/*deal time*/
		wchar_t Symbol[32];/*symbol*/
		wchar_t ExternalID[32];/*外部交易所持仓或订单ID*/
		sk::uint64 VolumeTotal;/*surplus amount 当前持仓总量*/
		sk::int64 TimeLocal;/*to mtserver timediff -- need calc.*/

		//! follow append status
		sk::uint64 FollowPositionTotal;/*前一比仓位总量 用与计算跟单总量的限制/或者跟单次数的限制*/

		//! runtime use dynamic / extend
		sk::int64 RunTimeDataInt64[8];
		sk::int32 RunTimeDataInt32[8];
		sk::uint8 RunTimeDataUint8[64];

		tagSignalField() { memset(this, 0x00, sizeof(*this)); }
		bool operator==(const tagSignalField& obj) const { return Deal == obj.Deal; }
		bool operator<(const tagSignalField& obj) const { return Deal < obj.Deal; }
		void operator=(const tagSignalField& obj) { memcpy(this, &obj, sizeof(*this)); }
	}SignalField; const size_t LENSIGNALFIELD = sizeof(SignalField);


	typedef struct tagFollowKey
	{
		char Key[64];

		tagFollowKey() { memset(this, 0x00, sizeof(*this)); }
		tagFollowKey(const char* key) { *this = key; }
		void operator=(const tagFollowKey& obj) { *this = obj.Key; }
		void operator=(const char* obj)
		{
			memset(this, 0x00, sizeof(*this));
			memcpy(Key, obj, strlen(obj) > _countof(Key) ? _countof(Key) : strlen(obj));
		}
		bool operator<(const tagFollowKey& obj) const
		{
			return memcmp(Key, obj.Key, Key[_countof(Key) - 1] ? strlen(Key) : _countof(Key)) < 0;
		}
		bool operator==(const tagFollowKey& obj) const
		{
			return memcmp(Key, obj.Key, Key[_countof(Key) - 1] ? strlen(Key) : _countof(Key)) == 0;
		}
		bool operator!=(const tagFollowKey& obj) const
		{
			return memcmp(Key, obj.Key, Key[_countof(Key) - 1] ? strlen(Key) : _countof(Key)) != 0;
		}
		const char* Get() const { return Key; }
	}FollowKey;

	/* This is a merchandiser deal interface
	 * Used to convey necessary information about a merchandiser
	 * This is the only way for the system to express merchandisers size 988
	 */
	typedef struct tagFollowField
	{
		enum EnFollowFlag
		{
			EN_TRADE_FLAG_DISABLE = 0,/*总开关: 禁用*/
			EN_TRADE_FLAG_ENABLE = 1,/*总开关: 启用*/
		};
		enum EnSLTPLimitFlag
		{
			EN_SLTP_LIMIT_FLAG_DISABLE = 0,
			EN_SLTP_LIMIT_FLAG_ENABLE = 1,
		};
		enum EnFollowMode
		{
			EN_TRADE_MODE_VOLUME_FIXED = 0,/*固定数量*/
			EN_TRADE_MODE_VOLUME_RATIO = 1,/*信号源交易量比例*/
			EN_TRADE_MODE_VOLUME_RANGE = 2,/*手数,比例,范围,组合*/
		};
		enum EnTypeType
		{
			EN_TYPE_TYPE_BUY = 0,/*只跟多单*/
			EN_TYPE_TYPE_SELL = 1,/*只跟空单*/
			EN_TYPE_TYPE_ALL = 2,/*多空全跟*/
		};
		enum EnTypeDriection
		{
			EN_TYPE_DRIECTION_PROS = 0,/*正向*/
			EN_TYPE_DRIECTION_CONS = 1,/*反向*/
		};
		enum EnFollowAddToFlag
		{
			EN_FOLLOWADDTO_FLAG_DISABLE = 0,
			EN_FOLLOWADDTO_FLAG_ENABLE = 1,
		};
		enum EnTimeFrameFollowFlag
		{
			EN_TIMEFRAME_FOLLOW_FLAG_DISABLE = 0,
			EN_TIMEFRAME_FOLLOW_FLAG_ENABLE = 1,
		};

		sk::uint64 IdentifyHead;
		sk::uint64 LoginSignal;/*信号源*/
		sk::uint64 LoginFollow;/*跟单者*/

		sk::uint32 FollowFlag;/*跟单开关*/
		sk::uint32 FollowMode;/*跟单模式*/
		sk::uint32 TypeType;/*买卖类型*/
		sk::uint32 TypeDirection;/*方向类型*/
		sk::uint32 PointSLTPFlag;/*损盈点差开关*/
		sk::uint32 FollowAddToFlag;/*补仓|强跟 / 追加交易开关*/
		sk::uint32 TimeFrameFollowFlag;/*时段设置开关*/

		sk::real64 VolumeFixed;/*固定数量*/
		sk::real64 VolumeRatio;/*数量比例*/
		sk::real64 VolumeMin;/*最小数量*/
		sk::real64 VolumeMax;/*最大数量*/
		sk::real64 VolumeOverMin;/*数量最小阀值 */
		sk::real64 VolumeOverMax;/*数量最大阀值 */
		sk::uint64 PositionOverMax;/*跟随持仓量阀值*/

		sk::int64 TimeBrith;/*诞生时间*/
		sk::int64 TimeUpdate;/*更新时间*/
		sk::int64 TimeFrameFollowEnd[5];/*跟随交易时段设置*/
		sk::int64 TimeFrameFollowBegin[5];/*跟随交易时段设置*/

		sk::int32 PointSLTP;/*止损止盈校正点差*/
		sk::real64 PriceSL;/*止损点差,要转换成整型*/
		sk::real64 PriceTP;/*止盈点差,要转换成整型*/

		wchar_t SymbolSuffix[32];/*后缀校正*/
		wchar_t SymbolAllow[128];/*允许品种列表*/
		wchar_t SymbolReject[128];/*排斥品种列表*/
		wchar_t IMTComment[32];/*平台注释*/

		//! runtime use dynamic / extend
		sk::int64 RunTimeDataInt64[8];
		sk::int32 RunTimeDataInt32[8];
		sk::uint8 RunTimeDataUint8[64];
		sk::uint64 IdentifyTail;

		tagFollowField() { memset(this, 0x00, sizeof(*this)); IdentifyHead = 0xFAC9C2D0; IdentifyTail = 0xB4B4AAC1; }
		FollowKey GetKey() const
		{
			return sk::stringa(tostringa(LoginSignal)).append("@").append(tostringa(LoginFollow)).c_str();
		}
		static tagFollowKey Key(const UINT64& signal, const UINT64& follow) {
			return sk::stringa(tostringa(signal).append("@").append(tostringa(follow))).c_str();
		}
		void operator=(const tagFollowField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool Equal(const tagFollowField& obj) const
		{
			return LoginSignal == obj.LoginSignal && LoginFollow == obj.LoginFollow;
		}
		bool operator==(const tagFollowField& obj) const
		{
			if (LoginSignal != obj.LoginSignal ||
				LoginFollow != obj.LoginFollow ||
				FollowFlag != obj.FollowFlag ||
				FollowMode != obj.FollowMode ||
				TypeType != obj.TypeType ||
				TypeDirection != obj.TypeDirection ||
				PointSLTPFlag != obj.PointSLTPFlag ||
				FollowAddToFlag != obj.FollowAddToFlag ||
				TimeFrameFollowFlag != obj.TimeFrameFollowFlag ||
				VolumeFixed != obj.VolumeFixed ||
				VolumeRatio != obj.VolumeRatio ||
				VolumeMin != obj.VolumeMin ||
				VolumeMax != obj.VolumeMax ||
				VolumeOverMin != obj.VolumeOverMin ||
				VolumeOverMax != obj.VolumeOverMax ||
				PositionOverMax != obj.PositionOverMax ||
				PriceSL != obj.PriceSL ||
				PriceTP != obj.PriceTP ||
				PointSLTP != obj.PointSLTP ||
				/*TimeUpdate != obj.TimeUpdate ||*/
				TimeBrith != obj.TimeBrith) {
				return false;
			}
			for (size_t i = 0; i < sizeof(TimeFrameFollowBegin) / sizeof(TimeFrameFollowBegin[0]); ++i)
			{
				if (TimeFrameFollowBegin[i] != obj.TimeFrameFollowBegin[i] || \
					TimeFrameFollowEnd[i] != obj.TimeFrameFollowEnd[i])
				{
					return false;
				}
			}
			if (wcscmp(SymbolSuffix, obj.SymbolSuffix) || \
				wcscmp(SymbolAllow, obj.SymbolAllow) || \
				wcscmp(SymbolReject, obj.SymbolReject) || \
				wcscmp(IMTComment, obj.IMTComment))
			{
				return false;
			}
			return true;
		}

		bool Verify() const { return IdentifyHead == 0xFAC9C2D0 && IdentifyTail == 0xB4B4AAC1; }

		SKAPIRES Check() const
		{
			if (!LoginFollow)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_LOGIN_NULL;
			}
			if (!LoginSignal)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_LOGIN_NULL;
			}
			if (LoginFollow == LoginSignal)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
			}
			for (size_t i = 0; i < sizeof(TimeFrameFollowBegin) / sizeof(TimeFrameFollowBegin[0]); ++i)
			{
				if (TimeFrameFollowBegin[0] >= TimeFrameFollowEnd[0])
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TIMEFRAME_BROKEN;
				}
			}

			switch (FollowMode)
			{
			case EnFollowMode::EN_TRADE_MODE_VOLUME_FIXED:
			{
				if (VolumeFixed <= 0)
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
				}
			}break;
			case EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO:
			{
				if (VolumeRatio <= 0)
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
				}
			}break;
			case EnFollowMode::EN_TRADE_MODE_VOLUME_RANGE:
			{
				if (VolumeRatio <= 0)
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
				}
				if (!(VolumeMin == 0 && VolumeMax == 0))
				{
					if (!(VolumeMin < VolumeMax))
					{
						return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
					}
				}
			}break;
			}

			if (!(VolumeOverMax == 0 && VolumeOverMin == 0))
			{
				if (!(VolumeOverMin < VolumeOverMax))
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
				}
			}
			if (SymbolReject[0] == '*' && SymbolAllow[0] == '*')
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_FOLLOW_CONFIGURE;
			}
			return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
		}

		void Default()
		{
			memset(this, 0x00, sizeof(*this));
			IdentifyHead = 0xFAC9C2D0;
			IdentifyTail = 0xB4B4AAC1;

			VolumeFixed = 1.0;/*固定数量*/
			VolumeRatio = 1.0;/*数量比例*/
			VolumeMin = 0.1;/*最小数量*/
			VolumeMax = 10.0;/*最大数量*/
			VolumeOverMin = 0.001;/*数量最小阀值 */
			VolumeOverMax = 50.0;/*数量最大阀值 */
			PositionOverMax = 0;/*跟随持仓量阀值*/
			FollowFlag = EnFollowFlag::EN_TRADE_FLAG_ENABLE;
			FollowMode = EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO;
			TypeType = EnTypeType::EN_TYPE_TYPE_ALL;
		}


		const wchar_t* GetFollowFlagStr() const
		{
			if (EnFollowFlag::EN_TRADE_FLAG_DISABLE == FollowFlag)
			{
				return L"关";
			}
			else if (EnFollowFlag::EN_TRADE_FLAG_ENABLE == FollowFlag)
			{
				return L"开";
			}
			return L"未知";
		}
		const wchar_t* GetTypeDirectionStr() const
		{
			if (EnTypeDriection::EN_TYPE_DRIECTION_PROS == TypeDirection)
			{
				return L"正向";
			}
			else if (EnTypeDriection::EN_TYPE_DRIECTION_CONS == TypeDirection)
			{
				return L"反向";
			}
			return L"未知";
		}
		const wchar_t* GetTypeTypeStr() const
		{
			if (EnTypeType::EN_TYPE_TYPE_ALL == TypeType)
			{
				return L"全跟";
			}
			else if (EnTypeType::EN_TYPE_TYPE_BUY == TypeType)
			{
				return L"跟多";
			}
			else if (EnTypeType::EN_TYPE_TYPE_SELL == TypeType)
			{
				return L"跟空";
			}
			return L"未知";
		}
		const wchar_t* GetSLTPLimitFlagStr() const
		{
			if (EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE == PointSLTPFlag)
			{
				return L"开";
			}
			else if (EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_DISABLE == PointSLTPFlag)
			{
				return L"关";
			}
			return L"未知";
		}
		const wchar_t* GetFollowAddToFlagStr() const
		{
			if (EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE == FollowAddToFlag)
			{
				return L"开";
			}
			else if (EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_DISABLE == FollowAddToFlag)
			{
				return L"关";
			}
			return L"未知";
		}
		const wchar_t* GetFollowModeStr() const
		{
			if (EnFollowMode::EN_TRADE_MODE_VOLUME_FIXED == FollowMode)
			{
				return L"固定手数";
			}
			else if (EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO == FollowMode)
			{
				return L"成交比例";
			}
			else if (EnFollowMode::EN_TRADE_MODE_VOLUME_RANGE == FollowMode)
			{
				return L"范围结合";
			}
			return L"未知";
		}
		const wchar_t* GetTimeFrameFollowFlagStr() const
		{
			if (EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_ENABLE == TimeFrameFollowFlag)
			{
				return L"开";
			}
			else if (EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_DISABLE == TimeFrameFollowFlag)
			{
				return L"关";
			}
			return L"未知";
		}
		//! 23:59:59 = 86399	00:00:00 = 90060 || 00:00:00 = 0
		//! 计算开始时间帧 --  [ 00:00:00 <= BeginTime <= 23:59:59 ]
		static sk::timestamp TimeFrameStrToTimestampBegin(const sk::string& srcTime)
		{
			if (!srcTime.empty())
			{
				std::vector<sk::string> vrParse;
				sk::Helper::OneUnknownParse(srcTime, ':', vrParse);
				if (vrParse.size() == 3)
				{
					auto result = _tcstoll(vrParse.at(0).c_str(), nullptr, 10) * 3600 + _tcstoll(vrParse.at(1).c_str(), nullptr, 10) * 60 + _tcstoll(vrParse.at(2).c_str(), nullptr, 10);
					if (result > 86399)
					{
						return 86399;
					}
					if (result > 0)
					{
						return result;
					}
				}
			}
			return 0;
		}
		//! 计算结束时间帧 -- 最小为 1 最大为 90060
		static sk::timestamp TimeFrameStrToTimestampEnd(const sk::string& srcTime)
		{
			if (!srcTime.empty())
			{
				std::vector<sk::string> vrParse;
				sk::Helper::OneUnknownParse(srcTime, ':', vrParse);
				if (vrParse.size() == 3)
				{
					auto result = _tcstoll(vrParse.at(0).c_str(), nullptr, 10) * 3600 + _tcstoll(vrParse.at(1).c_str(), nullptr, 10) * 60 + _tcstoll(vrParse.at(2).c_str(), nullptr, 10);
					if (result > 0 && result < 90060)
					{
						return result;
					}
				}
			}
			return 90060;
		}

		static sk::string TimeFrameTimestampToStrBegin(const sk::timestamp& srcTime)
		{
			if (srcTime >= 90060)
			{
				return _T("23:59:59");
			}
			if (srcTime > 0)
			{
				sk::int64 sec = srcTime % 60;
				sk::int64 min = (srcTime - sec) % 3600 / 60;
				sk::int64 hour = (srcTime - sec - min * 60) / 3600;

				TCHAR result[100] = { 0 };
				_stprintf_s(result, _T("%02I64d:%02I64d:%02I64d"), hour, min, sec);
				return result;
			}
			return _T("00:00:00");
		}
		static sk::string TimeFrameTimestampToStrEnd(const sk::timestamp& srcTime)
		{
			if (srcTime > 0 && srcTime < 90060)
			{
				sk::int64 sec = srcTime % 60;
				sk::int64 min = (srcTime - sec) % 3600 / 60;
				sk::int64 hour = (srcTime - sec - min * 60) / 3600;

				TCHAR result[100] = { 0 };
				_stprintf_s(result, _T("%02I64d:%02I64d:%02I64d"), hour, min, sec);
				return result;
			}
			return _T("00:00:00");
		}
	}FollowField, UserconField; const size_t LENFOLLOWFIELD = sizeof(FollowField);

	typedef sk::container::map<sk::stringw, shared::FollowField> tfFollowFields;





	typedef struct tagFollowLogField {
		//! 完成状态类型
		enum struct EnCompletionFlag : UINT
		{
			EN_COMPLETION_STATUS_RUNNING = 0,
			EN_COMPLETION_STATUS_FINISHED = 1,
		};
		//! 处理结果类型
		enum struct EnProcessResult : UINT
		{
			EN_PROCESS_RESULT_FAILED = 0,
			EN_PROCESS_RESULT_SUCCESS = 1,
		};
		//! 处理任务类型
		enum struct EnProcessTaskType : UINT
		{
			EN_PROCESS_TASK_TYPE_FOLLOW_EXECUTE = 0,/*交易跟单*/
			EN_PROCESS_TASK_TYPE_FOLLOW_MODIFY = 1,/*编辑订单*/
			EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_IN = 2,/*补跟*/
			EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_OUT = 3,/*补平*/
		};

		struct LogNode
		{
			UINT64 Login;
			wchar_t Symbol[32];
			UINT Action;
			UINT Entry;
			double Price;
			double PriceSL;
			double PriceTP;
			UINT64 Volume;
			UINT64 Deal;
			UINT64 Position;
		};

		int Head;
		LogNode FollowObj;
		LogNode SignalObj;
		UINT64 InitialLogin;//! 始发信号Login
		UINT64 InitialPosition;//! 始发信号Position
		EnProcessResult ProcessResult;
		EnCompletionFlag CompletionFlag;
		EnProcessTaskType ProcessTaskType;
		INT64 TimeUpdate;
		INT64 TimeCreate;
		CHAR Reason[64];/*失败原因*/
		UINT MTRetcode;/*MT返回代码*/
		int Tail;

		tagFollowLogField()
		{
			memset(this, 0x00, sizeof(*this));
			Head = ROUTE_MTCMD_HEAD;
			Tail = ROUTE_MTCMD_TAIL;
			ProcessResult = EnProcessResult::EN_PROCESS_RESULT_SUCCESS;
			CompletionFlag = EnCompletionFlag::EN_COMPLETION_STATUS_RUNNING;
			ProcessTaskType = EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_EXECUTE;
		}
		void operator=(const tagFollowLogField& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool Verify() const { return Head == ROUTE_MTCMD_HEAD && Tail == ROUTE_MTCMD_TAIL; }

		const TCHAR* ProcessResultStr() const
		{
			switch (ProcessResult)
			{
			case EnProcessResult::EN_PROCESS_RESULT_FAILED:
			{
				return _T("失败");
			}break;
			case EnProcessResult::EN_PROCESS_RESULT_SUCCESS:
			{
				return _T("完成");
			}break;
			}
			return _T("未知");
		}
		const TCHAR* ProcessTaskTypeStr() const
		{
			switch (ProcessTaskType)
			{
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_EXECUTE:
			{
				return _T("交易跟单");
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_MODIFY:
			{
				return _T("持仓跟单");
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_IN:
			{
				return _T("补仓跟单");
			}break;
			case EnProcessTaskType::EN_PROCESS_TASK_TYPE_FOLLOW_PATCH_OUT:
			{
				return _T("补仓跟平");
			}break;
			}
			return _T("未知");
		}

	}FollowLogField; const size_t LENFOLLOWLOGFIELD = sizeof(FollowLogField);


	typedef struct tagFollowRouteField
	{
		enum struct EnRouteSignalTypeFlag : UINT
		{
			EN_ROUTE_SIGNAL_TYPE_INITER = 0,//! 内联信号
			EN_ROUTE_SIGNAL_TYPE_EXTER = 1,//! 初始信号
			EN_ROUTE_SIGNAL_TYPE_FOLLOW = 2,//! 跟随者

			EN_ROUTE_SIGNAL_TYPE_FIRST = EN_ROUTE_SIGNAL_TYPE_INITER,
			EN_ROUTE_SIGNAL_TYPE_LAST = EN_ROUTE_SIGNAL_TYPE_FOLLOW,
		};

		enum struct EnRouteFollowStatusFlag : UINT
		{
			EN_ROUTE_STATUS_TYPE_FAILED = 0,
			EN_ROUTE_STATUS_TYPE_RUNING = 1,
			EN_ROUTE_STATUS_TYPE_SUCCESS = 2,

			EN_ROUTE_STATUS_TYPE_FIRST = EN_ROUTE_STATUS_TYPE_FAILED,
			EN_ROUTE_STATUS_TYPE_LAST = EN_ROUTE_STATUS_TYPE_SUCCESS,
		};

		enum struct EnRouteSignalStatusFlag : UINT
		{
			EN_ROUTE_SIGNAL_STATUS_FLAG_ENABLE = 0,
			EN_ROUTE_SIGNAL_STATUS_FLAG_DISABLE = 1,

			EN_ROUTE_SIGNAL_STATUS_FLAG_FIRST = EN_ROUTE_SIGNAL_STATUS_FLAG_ENABLE,
			EN_ROUTE_SIGNAL_STATUS_FLAG_LAST = EN_ROUTE_SIGNAL_STATUS_FLAG_DISABLE,
		};

		enum struct EnPositionUpdateFlag : UINT
		{
			EN_POSITION_MODIFY_FLAG_NULL = 0,
			EN_POSITION_MODIFY_FALG_PRICE_SL = 1,
			EN_POSITION_MODIFY_FALG_PRICE_TP = EN_POSITION_MODIFY_FALG_PRICE_SL << 1,

			EN_POSITION_MODIFY_FALG_FIRST = EN_POSITION_MODIFY_FLAG_NULL,
			EN_POSITION_MODIFY_FALG_LAST = EN_POSITION_MODIFY_FALG_PRICE_TP,
			EN_POSITION_MODIFY_FALG_ALL = EN_POSITION_MODIFY_FALG_PRICE_SL | EN_POSITION_MODIFY_FALG_PRICE_TP,
		};

		shared::DealID Key;
		FollowField Follow;
		SignalField Signal;
		RequestField Request;
		RequestField Response;
		PositionField PositionFollow;
		PositionField PositionSignal;
		FollowLogField FollowLog;

		EnRouteFollowStatusFlag RouteFollowStatusFlag;
		EnRouteSignalStatusFlag RouteSignalStatusFlag;
		EnRouteSignalTypeFlag RouteSignalTypeFlag;
		EnPositionUpdateFlag PositionUpdateFlag;
		//! append extern
		sk::uint64 InitialSignalIn;//! 初始进场信号(login)
		sk::uint64 InitialSignalOut;//! 初始出场信号(position)
		sk::uint64 InitialSignalInOut;//! 初始进出场信号(position)
		sk::uint64 InitialSignalModify;//! 持仓编辑初始信号(position)

		tagFollowRouteField()
		{
			Key = 0;
			InitialSignalIn = 0;
			InitialSignalOut = 0;
			InitialSignalInOut = 0;
			InitialSignalModify = 0;
			RouteSignalStatusFlag = EnRouteSignalStatusFlag::EN_ROUTE_SIGNAL_STATUS_FLAG_ENABLE;
			RouteFollowStatusFlag = EnRouteFollowStatusFlag::EN_ROUTE_STATUS_TYPE_RUNING;
			RouteSignalTypeFlag = EnRouteSignalTypeFlag::EN_ROUTE_SIGNAL_TYPE_EXTER;
			PositionUpdateFlag = EnPositionUpdateFlag::EN_POSITION_MODIFY_FLAG_NULL;
		}
		void operator=(const tagFollowRouteField& obj)
		{
			Key = obj.Key;
			Follow = obj.Follow;
			Signal = obj.Signal;
			Request = obj.Request;
			PositionFollow = obj.PositionFollow;
			PositionSignal = obj.PositionSignal;
			FollowLog = obj.FollowLog;

			InitialSignalIn = obj.InitialSignalIn;
			InitialSignalOut = obj.InitialSignalOut;
			InitialSignalInOut = obj.InitialSignalInOut;
			InitialSignalModify = obj.InitialSignalModify;
			RouteSignalStatusFlag = obj.RouteSignalStatusFlag;
			RouteFollowStatusFlag = obj.RouteFollowStatusFlag;
			RouteSignalTypeFlag = obj.RouteSignalTypeFlag;
			PositionUpdateFlag = obj.PositionUpdateFlag;
		}
	}FollowRouteField; const size_t LENFOLLOWROUTEFIELD = sizeof(FollowRouteField);


	//!@ 跟单系统/桥系统 登录
	typedef struct tagFollowObjLoginField final
	{
		shared::SystemID ID;
		CHAR Username[32];
		CHAR Password[64];
		CHAR Identify[64];
		tagFollowObjLoginField() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagFollowObjLoginField& obj)
		{
			memcpy(this, &obj, sizeof(*this));
		}
		bool operator==(const tagFollowObjLoginField& obj)
		{
			return ID == obj.ID;
		}
	}FollowObjLoginField, BridgeLoginField; const size_t LENLOGINFIELD = sizeof(FollowObjLoginField);

	typedef struct tagQuoteTaskRoute
	{

	}QuoteTaskRoute;

	//!@ 网关定单任务路由
	typedef struct tagQuoteTaskRouteFeeder final : public QuoteTaskRoute
	{

		tagQuoteTaskRouteFeeder() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagQuoteTaskRouteFeeder& obj) { memcpy(this, &obj, sizeof(*this)); }
	}QuoteTaskRouteFeeder;


	//!签名 : 
	//!说明 : 网关确认结果上传到桥
	//!注意 : 
	//!日期 : Sun Jun 7 19:09:52 UTC+0800 2020
	typedef struct tagGatewayConfirmResult final
	{
		shared::SystemID SystemOrderID;

		RequestSimpleField MTRequest;
		ConfirmSimpleField MTConfirm;

		//!说明 : 网关处理订单结果
		//!注意 : 
		//!日期 : Sun Jun 7 19:26:17 UTC+0800 2020
		SKAPIRES GatewayOrderProcessResult;

		//!说明 : 网关定单处理耗时
		//!注意 : 毫秒
		//!日期 : Sun Jun 7 18:57:03 UTC+0800 2020
		__int64 GatewayOrderProcessElapsedTime;

		tagGatewayConfirmResult() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagGatewayConfirmResult& obj) { memcpy(this, &obj, sizeof(*this)); }
	}GatewayConfirmResult; const size_t LENGATEWAYCONFIRMRESULT = sizeof(GatewayConfirmResult);


	//!签名 : tagDealerHedgeKey
	//!说明 : 交易插件/网关 组合定单Key
	//!注意 : 
	//!日期 : Wed Sep 9 16:52:59 UTC+0800 2020
	typedef struct tagHedgeKey
	{
		UINT64 MTLogin;//! MT帐户
		Serial GatewaySerial;//! 网关定单序列号
		SystemID SystemPID; //! 桥系统产品ID
		SystemID ExchangeID;//! 交易所ID
		tradeorder::EnOrdSide OrdSide;//! 定单多空类型

		tagHedgeKey() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagHedgeKey& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool operator<(const tagHedgeKey& obj) const
		{
			if (MTLogin != obj.MTLogin)
				return MTLogin < obj.MTLogin;
			else if (GatewaySerial != obj.GatewaySerial)
				return GatewaySerial < obj.GatewaySerial;
			else if (SystemPID != obj.SystemPID)
				return SystemPID < obj.SystemPID;
			else if (ExchangeID != obj.ExchangeID)
				return ExchangeID < obj.ExchangeID;
			return OrdSide < obj.OrdSide;
		}
		bool operator==(const tagHedgeKey& obj) const
		{
			return \
				MTLogin == obj.MTLogin &&
				GatewaySerial == obj.GatewaySerial &&
				SystemPID == obj.SystemPID &&
				ExchangeID == obj.ExchangeID &&
				OrdSide == obj.OrdSide;
		}
	}HedgeKey;

	typedef struct tagSystemOrder final
	{
		enum struct EnMTRetType : UINT
		{
			EN_MTRET_TYPE_INWAY = 10001,//!正要进行请求
			EN_MTRET_TYPE_ACCEPTED = 10002,//!已接受的请求
			EN_MTRET_TYPE_PROCESS = 10003,//!已处理的请求
			EN_MTRET_TYPE_REJECT = 10006,//!已拒绝的请求
			EN_MTRET_TYPE_CANCEL = 10007,//!已取消的请求
			EN_MTRET_TYPE_DONE = 10009,//!已实现的请求
			EN_MTRET_TYPE_DONE_PARTIAL = 10010,//!已部分实现的请求
			EN_MTRET_TYPE_ERROR = 10011,//!请求的常规错误
			EN_MTRET_TYPE_TIMEOUT = 10012,//!请求已超时
			EN_MTRET_TYPE_INVALID = 10013,//!无效请求
			EN_MTRET_TYPE_INVALID_VOLUME = 10014,//!无效量
			EN_MTRET_TYPE_INVALID_PRICE = 10015,//!无效价格
			EN_MTRET_TYPE_TRADE_DISABLED = 10017,//!禁用交易
			EN_MTRET_TYPE_MARKET_CLOSED = 10018,//!关闭市场
			EN_MTRET_TYPE_NO_MONEY = 10019,//!没有足够的钱款
			EN_MTRET_TYPE_TOO_MANY = 10024,//!太多交易请求
			EN_MTRET_TYPE_LIMIT_ORDERS = 10033,//!已达到订单数的限制
			EN_MTRET_TYPE_LIMIT_VOLUME = 10034,//!已达到订单数的限制
			EN_MTRET_TYPE_INVALID_ORDER = 10035,//!无效或被禁止的订单类型
			EN_MTRET_TYPE_POSITION_CLOSED = 10036,//!已经平仓
			EN_MTRET_TYPE_INVALID_CLOSE_VOLUME = 10038,//!要平仓的量超过持仓的当前量
			EN_MTRET_TYPE_CONNECTION = 10031,//!无连接
			EN_MTRET_TYPE_INVALID_FILL = 10030,//!成交模式不受支持
			EN_MTRET_TYPE_PRICE_OFF = 10021,//! 无价格
		};

		enum struct EnOrdRequestType : UINT64
		{
			//!说明 : 默认订单类型
			//!注意 : 
			//!日期 : Sun Sep 13 14:27:11 UTC+0800 2020
			EN_ORD_REQUEST_TYPE_DEFAULT = 0x0,
			//!@ 加仓
			EN_ORD_REQUEST_TYPE_POSITION_IN = 0x1000,
			//!@ 减仓
			EN_ORD_REQUEST_TYPE_POSITION_OUT = 0x2000,
			//!@ 开仓
			EN_ORD_REQUEST_TYPE_POSITION_OPEN =0x3000,
			//!@ 平仓
			EN_ORD_REQUEST_TYPE_POSITION_CLOSE = 0x4000,
			//!说明 : 调仓订单类型
			//!注意 : 
			//!日期 : Sun Sep 13 14:26:34 UTC+0800 2020
			EN_ORD_REQUEST_TYPE_POSITION_ADJUST = 0x10000000,
		};

		LONG Head;
		//!说明 : 业务对冲KEY
		//!注意 : 
		//!日期 : Sat Jun 13 20:43:10 UTC+0800 2020
		tagHedgeKey Key;
		//!说明 : 系统序列号
		//!注意 : 
		//!日期 : Sun Sep 13 17:54:00 UTC+0800 2020
		Serial SystemSerial;
		//!说明 : 网关产品ID
		//!注意 : 
		//!日期 : Sat Jun 13 20:25:44 UTC+0800 2020
		SystemID GatewayPID;
		//!说明 : 交易所产品ID
		//!注意 : 桥系统唯一
		//!日期 : Fri Jun 12 14:51:34 UTC+0800 2020
		SystemID ExchangePID;
		//!说明 : 定单数量
		//!注意 : 
		//!日期 : Thu Jun 11 19:56:27 UTC+0800 2020
		double LPOrderQty;
		//!说明 : 定单数量
		//!注意 : 
		//!日期 : Sat Aug 29 15:32:55 UTC+0800 2020
		UINT64 MTOrderQty;
		//!说明 : 定单价格
		//!注意 : 
		//!日期 : Sat Jun 13 17:42:50 UTC+0800 2020
		double MTOrderPx;
		double MTStopPx;
		//!说明 : 成交量
		//!注意 : 
		//!日期 : Thu Jun 11 20:10:23 UTC+0800 2020
		double LPTradeQty;
		//!说明 : 成交量
		//!注意 : 
		//!日期 : Sat Aug 29 15:33:34 UTC+0800 2020
		UINT64 MTTradeQty;
		//!说明 : 系统订单ID
		//!注意 : 系统生成
		//!日期 : Thu Jun 11 20:12:40 UTC+0800 2020
		SystemID OrderID;
		//!说明 : 成交均价
		//!注意 : LP赋值
		//!日期 : Thu Jun 11 20:13:22 UTC+0800 2020
		double TradeAvgPx;
		//!说明 : 成交最新价
		//!注意 : LP赋值 一批次成交最后一笔的价格
		//!日期 : Thu Jun 11 20:13:48 UTC+0800 2020
		double TradeLastPx;
		//!说明 : 定单请求终端ID
		//!注意 : 请求上下文标记
		//!日期 : Fri Jun 12 14:42:20 UTC+0800 2020
		SystemID ClientID;
		//!说明 : 定单结果
		//!注意 : 
		//!日期 : Fri Jun 12 19:15:53 UTC+0800 2020
		EnMTRetType MTRetcode;
		//!说明 : 系统结果
		//!注意 : 
		//!日期 : Fri Aug 28 03:33:28 UTC+0800 2020
		SKAPIRES SKRetcode;
		//!说明 : 订单请求类型
		//!注意 : 
		//!日期 : Tue Sep 8 20:05:32 UTC+0800 2020
		UINT64 OrdRequestType;

		tradeorder::EnOrdType OrdType;
		tradeorder::EnOrdTimeInForce OrdTimeInForce;
		tradeorder::EnOrdStatus OrdStatus;
		tradeorder::EnOrdExecType OrdExecType;
		tradeorder::EnPositionEffect PositionEffect;
		LONG LPProcessCount;//!@ LP继续处理次数
		CHAR Reserve[18];
		LONG Tail;

		tagSystemOrder() 
		{
			memset(this, 0x00, sizeof(*this)); 
			Head = 0xFEFF;
			Tail = 0x200B;
			SKRetcode = SKAPIRES::SYSTEM_ERROR;
			MTRetcode = EnMTRetType::EN_MTRET_TYPE_ERROR;
		}
		void operator=(const tagSystemOrder& obj) { memcpy(this, &obj, sizeof(*this)); }
		bool Verify() const { return Head == 0xFEFF && Tail == 0x200B; }
	}SystemOrder; const size_t LENSYSTEMORDER = sizeof(SystemOrder);

	typedef struct tagSystemPosition final
	{
		UINT64 Login;
		UINT64 Position;
		Instrument Symbol;//! MTSymbol
		UINT Digits;
		double ContractSize;
		double OpenPx;
		double SLPx;
		double TPPx;
		INT64 TimeCreateMsc;
		INT64 TimeUpdateMsc;
		UINT Action;//IMTPosition::EnPositionAction

		INT64 TimeOrder;
		SystemID OrderID;
		SystemID SystemPID;
		SystemID GatewayPID;
		SystemID ExchangeID;
		INT64 TotalQty;//! '+'(BUY)  '-'(SELL)

		tagSystemPosition() { memset(this, 0x00, sizeof(*this)); }
	}SystemPosition; const size_t LENSYSTEMPOSITION = sizeof(SystemPosition);













	//!签名 : tagTradeTaskRouteGateway
	//!说明 : 网关订单任务路由
	//!注意 : 
	//!日期 : Sun Jun 7 12:44:17 UTC+0800 2020
	typedef struct tagTradeTaskRouteGateway final
	{
		SystemOrder LPRequest;
		RequestField MTRequest;
		ConfirmField MTConfirm;

		shared::ConSymbolField MTSymbol;
		shared::MTGatewayProduct GatewayProdcon;

		time_t TimeCreate;

		//!说明 : 定单总数
		//!注意 : 针对组合定单的包装个数
		//!日期 : Fri Sep 11 10:07:05 UTC+0800 2020
		UINT CombinationOfOrderTotal;
		//!说明 : 包装定单数量
		//!注意 : 
		//!日期 : Sat Sep 12 00:25:27 UTC+0800 2020
		UINT64 CombinationOfOrderTotalQty;

		tagTradeTaskRouteGateway() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagTradeTaskRouteGateway& obj) { memcpy(this, &obj, sizeof(*this)); }
	}TradeTaskRouteGateway; const size_t LENTRADETASKROUTEGATEWAY = sizeof(TradeTaskRouteGateway);

	//!@ 桥定单任务主键
	typedef struct tagTradeTaskRouteBridgeKey final
	{
		UINT MTRequestID;
		SystemID ExchangeID;

		tagTradeTaskRouteBridgeKey()
		{
			MTRequestID = 0;
			ExchangeID = 0;
		}
		tagTradeTaskRouteBridgeKey(const UINT& reqID, const SystemID& exchangeID)
		{
			MTRequestID = reqID;
			ExchangeID = exchangeID;
		}
		void operator=(const tagTradeTaskRouteBridgeKey& obj)
		{
			MTRequestID = obj.MTRequestID;
			ExchangeID = obj.ExchangeID;
		}
		bool operator<(const tagTradeTaskRouteBridgeKey& obj) const
		{
			if (MTRequestID != obj.MTRequestID)
				return MTRequestID < obj.MTRequestID;
			return ExchangeID < obj.ExchangeID;
		}
	}TradeTaskRouteBridgeKey;

	typedef struct tagConsoleCommand final
	{
		SystemID ID;
		CHAR Input[100];
		CHAR Explain[260];

		tagConsoleCommand() { memset(this, 0x00, sizeof(*this)); }
	}ConsoleCommand; const size_t LENCONSOLECOMMAND = sizeof(ConsoleCommand);




	//!签名 : tagBridgeTradeRecord
	//!说明 : 桥系统磁盘日志，显示到桥系桥管理器
	//!注意 : 
	//!日期 : Sun Jun 7 09:00:21 UTC+0800 2020
	typedef struct tagBridgeTradeRecord final
	{
		//!说明 : 网关产品符号
		//!注意 : 
		//!日期 : Tue Jul 14 10:06:14 UTC+0800 2020
		Instrument Symbol;

		//!说明 : 网关产品精度
		//!注意 : 
		//!日期 : Wed Jul 15 07:45:00 UTC+0800 2020
		UINT Digits;

		//!说明 : 更新时间
		//!注意 : 日志更新时间(s)
		//!日期 : Sun Jun 7 09:10:06 UTC+0800 2020
		__int64 TimeUpdate;

		//!说明 : 网关处理时间
		//!注意 : 订单锁定->订单确认(ms)
		//!日期 : Sun Jun 7 12:05:30 UTC+0800 2020
		__int64 TimeProcessGateway;

		//!说明 : 过桥处理时间
		//!注意 : 订单接受->订单回执(ms)
		//!日期 : Sun Jun 7 12:06:03 UTC+0800 2020
		__int64 TimeProcessBridge;

		//!说明 : 订单数据
		//!注意 : 
		//!日期 : Tue Jul 14 07:37:31 UTC+0800 2020
		SystemOrder Order;

		//!说明 : 原因
		//!注意 : 
		//!日期 : Tue Jul 14 07:38:35 UTC+0800 2020
		CHAR Reason[64];

		tagBridgeTradeRecord() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagBridgeTradeRecord& obj) { memcpy(this, &obj, sizeof(*this)); }
	}BridgeTradeRecord; const size_t LENBRIDGETRADERECORD = sizeof(BridgeTradeRecord);



	//!@ 桥定单任务路由
	typedef struct tagTradeTaskRouteBridge final
	{
		SystemOrder OrderRequest;
		SystemProduct SystemProductCon;
		ExchangeProduct ExchangeProductCon;
		MTGatewayProduct GatewayProductCon;

		time_t TimeCreate;//!@ 
		time_t TimeTotal;//!@ 任务总耗时
		time_t TimeAvg;//!@ 任务平均耗时
		UINT ProcessCount;//!@ 任务处理次数

		tagTradeTaskRouteBridge() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagTradeTaskRouteBridge& obj) { memcpy(this, &obj, sizeof(*this)); }
	}TradeTaskRouteBridge; const size_t LENTRADETASKROUTEBRIDGE = sizeof(TradeTaskRouteBridge);


	//!@ 自动调仓
	typedef struct tagTradeTaskAutoAdjust final
	{
		double TradeQtyOpen;//! 进场成交量
		double TradeQtyClose;//! 出场成交量

		tagTradeTaskAutoAdjust() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagTradeTaskAutoAdjust& obj) { memcpy(this, &obj, sizeof(*this)); }
	}TradeTaskAutoAdjust;


	//!@ 交易所定单任务路由
	typedef struct tagTradeTaskRouteExchange final
	{
		enum struct EnTaskType : int
		{
			EN_TASK_TYPE_TRADE_ORDER = 0,//! 交易定单任务
			EN_TASK_TYPE_TRADE_RECORD_EXTERNAL_REQUEST = 1,//! 交易记录任务--外部请求
			EN_TASK_TYPE_TRADE_RECORD_INTERNAL_REQUEST = 2,//! 交易记录任务--内部请求
		};

		EnTaskType TaskType;

		//!说明 : 交易记录请求
		//!注意 : 
		//!日期 : Sat Jun 20 08:50:57 UTC+0800 2020
		tradeorder::TradeRecordRequest TradeRequest;
		//!说明 : 交易订单请求
		//!注意 : 
		//!日期 : Sat Jun 20 08:51:04 UTC+0800 2020
		SystemOrder OrderRequest;
		//!说明 : 订单状态请求
		//!注意 : 
		//!日期 : Sat Jun 20 17:53:22 UTC+0800 2020
		SystemOrder OrderStatus;

		//!@ FIX交易报表
		tradeorder::ExecutionReport TradeReport;
		//!@ 交易所持仓
		tradeorder::ExchangePosition TradePosition;

		time_t UpdateTime;

		TradeTaskAutoAdjust AutoAdjust;

		tagTradeTaskRouteExchange() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagTradeTaskRouteExchange& obj) { memcpy(this, &obj, sizeof(*this)); }
	}TradeTaskRouteExchange;


	//!@ 跟单系统配置
	typedef struct tagFollowSystemSetup final
	{
		enum struct EnSystemSwitch : UINT
		{
			EN_SYSTEM_SWITCH_ENABLE = 0,
			EN_SYSTEM_SWITCH_DISABLE = 1,
		};

		enum struct EnAutoPositionCloseSwitch : UINT
		{
			EN_AUTO_POSITION_CLOSE_SWITCH_ENABLE = 0,
			EN_AUTO_POSITION_CLOSE_SWITCH_CLOSE = 1,
		};

		//!@ 强跟/补跟 选项
		enum struct EnPatchInAction : UINT
		{
			EN_PATCH_IN_ACTION_FOLLOWRE = 0,//!@ 重新跟单
			EN_PATCH_IN_ACTION_FOLLOWGO = 1,//!@ 继续跟单
		};

		//!@ SLTP校正开关
		enum struct EnCorrectionSLTPSwitch : UINT
		{
			EN_CORRECTION_SLTP_SWITCH_ENABLE = 0,
			EN_CORRECTION_SLTP_SWITCH_DISABLE = 1,
		};

		//!@ 跟单信号交易源开关
		enum struct EnFollowToSignalExchangeSwitch : UINT
		{
			EN_FOLLOWTO_SIGNAL_EXCHANGE_SWITCH_ENABLE = 0,
			EN_FOLLOWTO_SIGNAL_EXCHANGE_SWITCH_DISABLE = 1,
		};

		EnSystemSwitch SystemSwitch;//!@ 跟单系统总开关
		EnPatchInAction PatchInAction;//!@ 强跟选项
		EnCorrectionSLTPSwitch CorrectionSLTPSwitch;//!@ SLTP校正开关
		EnAutoPositionCloseSwitch AutoPositionCloseSwitch;//!@ 自动跟漏平开关
		time_t TimeAutoPositionCloseCheck;//!@ 遗漏平仓检测时间间隔(ms)
		time_t TimeSLTPCorrectionInterval;//!@ SLTP校正检测时间间隔(ms)
		EnFollowToSignalExchangeSwitch FollowToSignalExchangeSwitch;//!@ 跟单信号交易源开关


		tagFollowSystemSetup() { memset(this, 0x00, sizeof(*this)); Default(); }
		void operator=(const tagFollowSystemSetup& obj) { memcpy(this,&obj,sizeof(*this)); }

		void Default()
		{
			TimeSLTPCorrectionInterval = 10 * 1000;
			TimeAutoPositionCloseCheck = 10 * 1000;
			CorrectionSLTPSwitch = EnCorrectionSLTPSwitch::EN_CORRECTION_SLTP_SWITCH_ENABLE;
			PatchInAction = EnPatchInAction::EN_PATCH_IN_ACTION_FOLLOWGO;
			SystemSwitch = EnSystemSwitch::EN_SYSTEM_SWITCH_ENABLE;
			AutoPositionCloseSwitch = EnAutoPositionCloseSwitch::EN_AUTO_POSITION_CLOSE_SWITCH_ENABLE;
			FollowToSignalExchangeSwitch = EnFollowToSignalExchangeSwitch::EN_FOLLOWTO_SIGNAL_EXCHANGE_SWITCH_DISABLE;
		}


	}FollowSystemSetup; const size_t LENFOLLOWSYSTEMSETUP = sizeof(FollowSystemSetup);



	//!@ 桥系统配置
	typedef struct tagBridgeSystemSetup final
	{
		enum struct EnSystemPlatformMode : UINT
		{
			EN_SYSTEM_PLATFORM_MODE_UNDEFINED = 0,
			EN_SYSTEM_PLATFORM_MODE_STP = 1,//!@ Straight Through Processing -- 直接交易员模式/中介模式/无交易员模式
			EN_SYSTEM_PLATFORM_MODE_ECN = EN_SYSTEM_PLATFORM_MODE_STP << 1,//!@ Electronic Communications Network -- 自动撮合成交模式/无交易员模式
			EN_SYSTEM_PLATFORM_MODE_MTF = EN_SYSTEM_PLATFORM_MODE_STP << 2,//!@ Multilateral Trading Facility -- 多边交易模式/无交易员模式
			EN_SYSTEM_PLATFORM_MODE_MM = EN_SYSTEM_PLATFORM_MODE_STP << 3,//!@ Market Maker -- 做市商模式/对赌模式/交易员模式
		};

		enum struct EnDevelopDebugSwitch : UINT
		{
			EN_DEVELOP_DEBUG_MODE_DISABLE = 0,
			EN_DEVELOP_DEBUG_MODE_ENABLE = 1,
		};

		enum struct EnAggregateQuoteSwitch : UINT
		{
			EN_AGGREGATE_QUOTE_FALG_DISABLE = 0,
			EN_AGGREGATE_QUOTE_FALG_ENABLE = 1,
		};

		enum struct EnDealerTradePxType : UINT
		{
			EN_DEALER_TRADEPX_TYPE_AVGPX = 0,//! 成交均价
			EN_DEALER_TRADEPX_TYPE_LASTPX = 1,//! 成交最后一笔价格
			EN_DEALER_TRADEPX_TYPE_ORDPX = 2,//! 下单价
		};

		enum struct EnSystemOrderMode : UINT
		{
			EN_SYSTEM_ORDER_MODE_SINGLE = 0,
			EN_SYSTEM_ORDER_MODE_MULTIPLE = 1,
		};

		enum struct EnDealerContinueToRequestSwitch : UINT
		{
			EN_DEALER_CONTINUE_TO_REQUEST_DISABLE = 0,
			EN_DEALER_CONTINUE_TO_REQUEST_ENABLE = 1,
		};

		enum struct EnAutoAdjustPositionSwitch : UINT
		{
			EN_AUTO_ADJUST_POSITION_SWITCH_DISABLE = 0,
			EN_AUTO_ADJUST_POSITION_SWITCH_ENABLE = 1,
		};

		enum struct EnFeederSpreadMode : UINT
		{
			EN_FEEDER_SPREAD_MODE_NOPROCESSING = 0,//! 不处理
			EN_FEEDER_SPREAD_MODE_SYSTEM = 1,//! 在桥系统报价上做点差处理
			EN_FEEDER_SPREAD_MODE_PLATFORM = 2,//! 在MT5平台最终报价上做点差处理
		};

		EnAggregateQuoteSwitch AggregateQuoteSwitch;//!@ 聚合报价开关
		EnSystemPlatformMode SystemPlatformMode;//! 系统平台模式
		TimePeriod SystemOperatingTimePeriod[7];//! 系统工作时间段
		tradeorder::EnOrdType DealerOrdType;//!@ 网关订单执行价格类型
		tradeorder::EnOrdTimeInForce DealerOrdTimeInForce;//!@ 经纪商订单价格有效期类型	
		EnDealerTradePxType DealerTradePxType;//!@ 网关定单成交价类型
		time_t TimeoutDealerOrder;//!@ 网关订单任务超时时间间隔(毫秒)
		time_t TimeoutQuoteActive;//!@ 报价未激活超时时间间隔(毫秒) 提醒
		EnDevelopDebugSwitch DevelopDebugSwitch;//!@ 开发调试开关
		EnSystemOrderMode SystemOrderMode;//! 系统抛单模式
		UINT64 OneByOneOrderAssignLogin[200];//!@ 特殊业务过滤帐户/逐个抛单单帐户
		EnDealerContinueToRequestSwitch DealerContinueToRequestSwitch;//!@ 部分成交订单，继续请求开关
		time_t DealerContinueToRequestDelay;//! 继续请求延迟时间
		EnAutoAdjustPositionSwitch AutoAdjustPositionSwitch;
		EnFeederSpreadMode FeederSpreadMode;

		tagBridgeSystemSetup() { memset(this, 0x00, sizeof(*this)); Default(); }
		void operator=(const tagBridgeSystemSetup& obj) { memcpy(this, &obj, sizeof(*this)); }



		void Default()
		{		
			SystemPlatformMode = EnSystemPlatformMode::EN_SYSTEM_PLATFORM_MODE_STP;
			AutoAdjustPositionSwitch = EnAutoAdjustPositionSwitch::EN_AUTO_ADJUST_POSITION_SWITCH_DISABLE;
			SystemOrderMode = EnSystemOrderMode::EN_SYSTEM_ORDER_MODE_MULTIPLE;
			TimeoutDealerOrder = 30 * 1000;
			TimeoutQuoteActive = 300 * 1000;
			DealerOrdType = tradeorder::EnOrdType::EN_ORD_TYPE_MARKET;
			DealerOrdTimeInForce = tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC;
			DevelopDebugSwitch = EnDevelopDebugSwitch::EN_DEVELOP_DEBUG_MODE_DISABLE;
			AggregateQuoteSwitch = EnAggregateQuoteSwitch::EN_AGGREGATE_QUOTE_FALG_ENABLE;
			DealerTradePxType = EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_ORDPX;
			DealerContinueToRequestDelay = 3000;
			DealerContinueToRequestSwitch = EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_ENABLE;
			FeederSpreadMode = EnFeederSpreadMode::EN_FEEDER_SPREAD_MODE_SYSTEM;
			memset(&OneByOneOrderAssignLogin[0], 0x00, sizeof(UINT64) * 200);
			//! 23:59:59 = 86399	00:00:00 = 90060 || 00:00:00 = 0
			//! 注意,起始时间不能小于结束时间
			//! 注意,当时间节点的起始时间=结束时间 系统视为时间段不生效/不限制
			for (size_t wday = 0; wday < _countof(SystemOperatingTimePeriod); ++wday)
			{
				SystemOperatingTimePeriod[wday].TimeBegin = 0;//! 00:00:00
				SystemOperatingTimePeriod[wday].TimeEnd = 90060;//! 00:00:00
			}
			SystemOperatingTimePeriod[6].TimeBegin = 0;//! 00:00:00
			SystemOperatingTimePeriod[6].TimeEnd = 0;//! 00:00:00
			SystemOperatingTimePeriod[0].TimeBegin = 0;//! 00:00:00
			SystemOperatingTimePeriod[0].TimeEnd = 0;//! 00:00:00
		}


		bool OneByOneOrderAssignLoginAppend(const UINT64& login)
		{
			for (size_t i = 0; i < 200; ++i)
			{
				if (OneByOneOrderAssignLogin[i] == login || OneByOneOrderAssignLogin[i] <= 0)
				{ 
					OneByOneOrderAssignLogin[i] = login;
					return true;
				}
			}
			return false;
		}
		bool OneByOneOrderAssignLoginCheck(const UINT64& login) const
		{
			if (OneByOneOrderAssignLogin[0] <= 0) return false;
			for (size_t i = 0; i < 200; ++i)
			{
				if (OneByOneOrderAssignLogin[i] == login)
				{
					return true;
				}
				else if (OneByOneOrderAssignLogin[i] <= 0)
					break;
			}
			return false;
		}

		size_t OneByOneOrderAssignLoginCount() const
		{
			size_t result = 0;
			for (size_t i = 0; i < 200; ++i)
			{
				if (OneByOneOrderAssignLogin[i] > 0)
				{
					++result;
					continue;
				}
				break;
			}
			return result;
		}

		const char* DealerOrdTypeStr() const
		{
			switch (DealerOrdType)
			{
			case tradeorder::EnOrdType::EN_ORD_TYPE_LIMIT:
			{
				return "限价";
			}break;
			case tradeorder::EnOrdType::EN_ORD_TYPE_MARKET:
			{
				return "市价";
			}break;
			}
			return "未知";
		}
		const char* DealerOrdTimeInForceStr() const
		{
			switch (DealerOrdTimeInForce)
			{
			case tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_DAY:
			{
				return "EN_ORD_TIMEINFORCE_DAY";
			}break;
			case tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_FOK:
			{
				return "EN_ORD_TIMEINFORCE_FOK";
			}break;
			case tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_GTC:
			{
				return "EN_ORD_TIMEINFORCE_GTC";
			}break;
			case tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_GTD:
			{
				return "EN_ORD_TIMEINFORCE_GTD";
			}break;
			case tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC:
			{
				return "EN_ORD_TIMEINFORCE_IOC";
			}break;
			}
			return "未知";
		}
		const char* SystemPlatformModeStr() const
		{
			switch (SystemPlatformMode)
			{
			case EnSystemPlatformMode::EN_SYSTEM_PLATFORM_MODE_ECN:
			{
				return "自动撮合成交模式";
			}break;
			case EnSystemPlatformMode::EN_SYSTEM_PLATFORM_MODE_MM:
			{
				return "做市商模式";
			}break;
			case EnSystemPlatformMode::EN_SYSTEM_PLATFORM_MODE_MTF:
			{
				return "多边交易模式";
			}break;
			case EnSystemPlatformMode::EN_SYSTEM_PLATFORM_MODE_STP:
			{
				return "直接交易员模式";
			}break;
			}
			return "未定义";
		}

		const char* DealerTradePxTypeStr() const
		{
			switch (DealerTradePxType)
			{
			case EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_AVGPX:
			{
				return "交易所成交均价";
			}break;
			case EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_LASTPX:
			{
				return "交易所成交最新价";
			}break;
			case EnDealerTradePxType::EN_DEALER_TRADEPX_TYPE_ORDPX:
			{
				return "网关下单价";
			}break;
			}
			return "未定义";
		}

		const char* SystemOrderModeStr() const
		{
			switch (SystemOrderMode)
			{
			case EnSystemOrderMode::EN_SYSTEM_ORDER_MODE_MULTIPLE:
			{
				return "组合抛单";
			}break;
			case EnSystemOrderMode::EN_SYSTEM_ORDER_MODE_SINGLE:
			{
				return "逐个抛单";
			}break;
			}
			return "未定义";
		}

		const char* FeederSpreadModeStr() const
		{
			switch (FeederSpreadMode)
			{
			case EnFeederSpreadMode::EN_FEEDER_SPREAD_MODE_PLATFORM:
			{
				return "平台";
			}break;
			case EnFeederSpreadMode::EN_FEEDER_SPREAD_MODE_SYSTEM:
			{
				return "系统";
			}break;
			}
			return "未定义";
		}

		void Print(sk::stringa& out)
		{
			std::ostringstream os;
			os.clear();
			os << std::endl
				<< "---系统参数---" << std::endl
				<< "系统平台模式: " << SystemPlatformModeStr() << std::endl
				<< "聚合报价开关: " << (AggregateQuoteSwitch == EnAggregateQuoteSwitch::EN_AGGREGATE_QUOTE_FALG_DISABLE ? "启用" : "禁用") << std::endl
				<< "成交价类型: " << DealerTradePxTypeStr() << std::endl
				<< "系统抛单模式: " << SystemOrderModeStr() << std::endl
				<< "自动调仓开关: " << (AutoAdjustPositionSwitch == EnAutoAdjustPositionSwitch::EN_AUTO_ADJUST_POSITION_SWITCH_DISABLE ? "禁用" : "启用") << std::endl
				<< "开发调试模式: " << (DevelopDebugSwitch == EnDevelopDebugSwitch::EN_DEVELOP_DEBUG_MODE_DISABLE ? "禁用" : "启用") << std::endl
				<< "点差处理模式: " << FeederSpreadModeStr() << std::endl
				<< "订单类型: " << DealerOrdTypeStr() << std::endl
				<< "订单有效期类型: " << DealerOrdTimeInForceStr() << std::endl;
			out = os.str();
		}
	}BridgeSystemSetup; const size_t LENBRIDGESYSTEMSETUP = sizeof(BridgeSystemSetup);




	//! 通用请求ACK
	typedef struct tagCommRequestAck final
	{
		SKAPIRES Retcode;

		tagCommRequestAck() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagCommRequestAck& obj) { memcpy(this, &obj, sizeof(*this)); }
	}CommRequestAck; const size_t LENCOMMREQUESTACK = sizeof(CommRequestAck);
	//! 通用请求RES
	typedef struct tagCommRequestResult final
	{
		SKAPIRES Retcode;

		tagCommRequestResult() { memset(this, 0x00, sizeof(*this)); }
		void operator=(const tagCommRequestResult& obj) { memcpy(this, &obj, sizeof(*this)); }
	}CommRequestResult;







#pragma pack(pop)

}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年07月30日 11:34:58|814**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_INTERFACE_H__6E945_93C00546_F381B6E39BFB249_376D__TAIL__