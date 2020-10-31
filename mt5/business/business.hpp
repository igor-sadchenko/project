#if !defined(AFX_SKSTU_BUSINESS_H__1AB6C_34697472_CEA454390E7D5F5_768E__HEAD__)
#define AFX_SKSTU_BUSINESS_H__1AB6C_34697472_CEA454390E7D5F5_768E__HEAD__

namespace shared {
	class Business {
	public:
#if 0
		static void TradeOrderReportToMTConfirm(const TradeOrderExecutionReport& report, IMTConfirm* confirm) {		
			if (!confirm) return;
			confirm->Clear();
			confirm->ID(report.OrderReq.MtOrder.RequestID);
			confirm->Volume((UINT64)report.OrderRep.LastQty);
			confirm->Price(report.OrderRep.LastPx);
			confirm->OrderID(sk::StringConvert::MBytesToWString(report.OrderRep.OrderID).c_str());	
			confirm->Retcode(MT_RET_REQUEST_ERROR);
		}
		static void TradeOrderToConfirm(const TradeOrderField& order, ConfirmField& confirm) {
			confirm.ID = (order.MtOrder.RequestID);
			confirm.Volume = (order.MtOrder.Volume);
			confirm.Price=(order.LpOrder.Price);
			wcsncpy_s(confirm.OrderID, sk::StringConvert::MBytesToWString(order.LpOrder.OrderID).c_str(), _countof(confirm.OrderID));
			confirm.Retcode = MT_RET_REQUEST_DONE;
		}
		static void TradeOrderToMTConfirm(const TradeOrderField& order,IMTConfirm* confirm) {
			if (!confirm) return;
			confirm->Clear();
			confirm->ID(order.MtOrder.RequestID);
			confirm->Volume(order.MtOrder.Volume);
			confirm->Price(order.LpOrder.Price);
			confirm->OrderID(sk::StringConvert::MBytesToWString(order.LpOrder.OrderID).c_str());
			confirm->Retcode(MT_RET_REQUEST_DONE);
		}
#endif
		static void ExecutionMake(IMTExecution* outExecution, const ExecutionField& inExecution) {
			if (!outExecution) return;
			outExecution->ID(inExecution.ID);
			outExecution->Action(inExecution.Action);
			outExecution->Datetime(inExecution.Datetime);
			outExecution->Login(inExecution.Login);
			outExecution->Flags(inExecution.Flags);
			outExecution->Digits(inExecution.Digits);
			outExecution->Order(inExecution.Order);
			outExecution->OrderType(inExecution.OrderType);
			outExecution->OrderVolume(inExecution.OrderVolume);
			outExecution->OrderPrice(inExecution.OrderPrice);
			outExecution->OrderActivationFlags(inExecution.OrderActivationFlags);
			outExecution->DealAction(inExecution.DealAction);
			outExecution->DealVolume(inExecution.DealVolume);
			outExecution->DealVolumeRemaind(inExecution.DealVolumeRemaind);
			outExecution->DealPrice(inExecution.DealPrice);
			outExecution->OrderPriceTrigger(inExecution.OrderPriceTrigger);
			outExecution->OrderTypeTime(inExecution.OrderTypeTime);
			outExecution->OrderTimeExpiration(inExecution.OrderTimeExpiration);
			outExecution->OrderTypeFill(inExecution.OrderTypeFill);
			outExecution->EOSSessionStart(inExecution.EOSSessionStart);
			outExecution->EOSSessionEnd(inExecution.EOSSessionEnd);
			outExecution->EOSPriceSettlement(inExecution.EOSPriceSettlement);
			outExecution->EOSTickValue(inExecution.EOSTickValue);
			outExecution->OrderPriceSL(inExecution.OrderPriceSL);
			outExecution->OrderPriceTP(inExecution.OrderPriceTP);
			outExecution->PriceGateway(inExecution.PriceGateway);
			outExecution->OrderActivationMode(inExecution.OrderActivationMode);
			outExecution->DealCommission(inExecution.DealCommission);
			outExecution->DatetimeMsc(inExecution.DatetimeMsc);
			outExecution->DealStorage(inExecution.DealStorage);
			outExecution->DealReason(inExecution.DealReason);
			outExecution->GatewayID(inExecution.GatewayID);
			outExecution->Position(inExecution.Position);
			outExecution->PositionBy(inExecution.PositionBy);
			outExecution->ExternalRetcode(inExecution.ExternalRetcode);
			outExecution->OrderVolumeExt(inExecution.OrderVolumeExt);
			outExecution->DealVolumeExt(inExecution.DealVolumeExt);
			outExecution->DealVolumeRemaindExt(inExecution.DealVolumeRemaindExt);
			outExecution->PositionExternalID(inExecution.PositionExternalID);
			outExecution->PositionByExternalID(inExecution.PositionByExternalID);
			outExecution->SymbolNew(inExecution.SymbolNew);
			outExecution->ExternalAccount(inExecution.ExternalAccount);
			outExecution->DealExternalID(inExecution.DealExternalID);
			outExecution->OrderExternalID(inExecution.OrderExternalID);
			outExecution->Comment(inExecution.Comment);
			outExecution->Symbol(inExecution.Symbol);
			outExecution->Group(inExecution.Group);
			outExecution->ExternalID(inExecution.ExternalID);
		}
		static void MakeExecutionField(const IMTExecution* inExecution, ExecutionField& outExecution) {
			if (!inExecution) return;
			outExecution.ID = inExecution->ID();
			outExecution.Action = inExecution->Action();
			outExecution.Datetime = inExecution->Datetime();
			outExecution.Login = inExecution->Login();
			outExecution.Flags = inExecution->Flags();
			outExecution.Digits = inExecution->Digits();
			outExecution.Order = inExecution->Order();
			outExecution.OrderType = inExecution->OrderType();
			outExecution.OrderVolume = inExecution->OrderVolume();
			outExecution.OrderPrice = inExecution->OrderPrice();
			outExecution.OrderActivationFlags = inExecution->OrderActivationFlags();
			outExecution.DealAction = inExecution->DealAction();
			outExecution.DealVolume = inExecution->DealVolume();
			outExecution.DealVolumeRemaind = inExecution->DealVolumeRemaind();
			outExecution.DealPrice = inExecution->DealPrice();
			outExecution.OrderPriceTrigger = inExecution->OrderPriceTrigger();
			outExecution.OrderTypeTime = inExecution->OrderTypeTime();
			outExecution.OrderTimeExpiration = inExecution->OrderTimeExpiration();
			outExecution.OrderTypeFill = inExecution->OrderTypeFill();
			outExecution.EOSSessionStart = inExecution->EOSSessionStart();
			outExecution.EOSSessionEnd = inExecution->EOSSessionEnd();
			outExecution.EOSPriceSettlement = inExecution->EOSPriceSettlement();
			outExecution.EOSProfitRateBuy = inExecution->EOSProfitRateBuy();
			outExecution.EOSProfitRateSell = inExecution->EOSProfitRateSell();
			outExecution.EOSTickValue = inExecution->EOSTickValue();
			outExecution.OrderPriceSL = inExecution->OrderPriceSL();
			outExecution.OrderPriceTP = inExecution->OrderPriceTP();
			outExecution.PriceGateway = inExecution->PriceGateway();
			outExecution.OrderActivationMode = inExecution->OrderActivationMode();
			outExecution.DealCommission = inExecution->DealCommission();
			outExecution.DatetimeMsc = inExecution->DatetimeMsc();
			outExecution.DealStorage = inExecution->DealStorage();
			outExecution.EOSRolloverValueLong = inExecution->EOSRolloverValueLong();
			outExecution.EOSRolloverValueShort = inExecution->EOSRolloverValueShort();
			outExecution.DealReason = inExecution->DealReason();
			outExecution.GatewayID = inExecution->GatewayID();
			outExecution.Position = inExecution->Position();
			outExecution.PositionBy = inExecution->PositionBy();
			outExecution.ExternalRetcode = inExecution->ExternalRetcode();
			outExecution.OrderVolumeExt = inExecution->OrderVolumeExt();
			outExecution.DealVolumeExt = inExecution->DealVolumeExt();
			outExecution.DealVolumeRemaindExt = inExecution->DealVolumeRemaindExt();
			wcscpy_s(outExecution.PositionExternalID, inExecution->PositionExternalID());
			wcscpy_s(outExecution.PositionByExternalID, inExecution->PositionByExternalID());
			wcscpy_s(outExecution.SymbolNew, inExecution->SymbolNew());
			wcscpy_s(outExecution.ExternalAccount, inExecution->ExternalAccount());
			wcscpy_s(outExecution.DealExternalID, inExecution->DealExternalID());
			wcscpy_s(outExecution.OrderExternalID, inExecution->OrderExternalID());
			wcscpy_s(outExecution.Comment, inExecution->Comment());
			wcscpy_s(outExecution.Symbol, inExecution->Symbol());
			wcscpy_s(outExecution.Group, inExecution->Group());
			wcscpy_s(outExecution.ExternalID, inExecution->ExternalID());
		}
		static void MakeDealField(const IMTDeal* inDeal, DealField& outDeal) {
			if (!inDeal) return;
			outDeal.Deal = inDeal->Deal();
			wcscpy_s(outDeal.ExternalID, inDeal->ExternalID());
			outDeal.Login = inDeal->Login();
			outDeal.Dealer = inDeal->Dealer();
			outDeal.Order = inDeal->Order();
			outDeal.Action = inDeal->Action();
			outDeal.Entry = inDeal->Entry();
			outDeal.Digits = inDeal->Digits();
			outDeal.DigitsCurrency = inDeal->DigitsCurrency();
			outDeal.ContractSize = inDeal->ContractSize();
			outDeal.Time = inDeal->Time();
			wcscpy_s(outDeal.Symbol, inDeal->Symbol());
			outDeal.Price = inDeal->Price();
			outDeal.Volume = inDeal->Volume();
			outDeal.Profit = inDeal->Profit();
			outDeal.Storage = inDeal->Storage();
			outDeal.Commission = inDeal->Commission();
			outDeal.ObsoleteValue = inDeal->ObsoleteValue();
			outDeal.RateProfit = inDeal->RateProfit();
			outDeal.RateMargin = inDeal->RateMargin();
			outDeal.ExpertID = inDeal->ExpertID();
			outDeal.PositionID = inDeal->PositionID();
			wcscpy_s(outDeal.Comment, inDeal->Comment());
			outDeal.ProfitRaw = inDeal->ProfitRaw();
			outDeal.PricePosition = inDeal->PricePosition();
			outDeal.VolumeClosed = inDeal->VolumeClosed();
			outDeal.TickValue = inDeal->TickValue();
			outDeal.TickSize = inDeal->TickSize();
			outDeal.Flags = inDeal->Flags();
			outDeal.TimeMsc = inDeal->TimeMsc();
			outDeal.Reason = inDeal->Reason();
			outDeal.PriceGateway = inDeal->PriceGateway();
			outDeal.ModificationFlags = inDeal->ModificationFlags();
			outDeal.PriceSL = inDeal->PriceSL();
			outDeal.PriceTP = inDeal->PriceTP();
			outDeal.VolumeExt = inDeal->VolumeExt();
			outDeal.VolumeClosedExt = inDeal->VolumeClosedExt();

			inDeal->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, outDeal.ApiDataLogin);
			inDeal->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, outDeal.ApiDataDeal);
			inDeal->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, outDeal.ApiDataOrder);
			inDeal->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, outDeal.ApiDataPosition);
		}
		static void MakePositionField(const IMTPosition* inPosition, PositionField& outPosition) {
			if (!inPosition) return;
			outPosition.Login = inPosition->Login();
			wcscpy_s(outPosition.Symbol, inPosition->Symbol());
			outPosition.Action = inPosition->Action();
			outPosition.Digits = inPosition->Digits();
			outPosition.DigitsCurrency = inPosition->DigitsCurrency();
			outPosition.ContractSize = inPosition->ContractSize();
			outPosition.TimeCreate = inPosition->TimeCreate();
			outPosition.TimeUpdate = inPosition->TimeUpdate();
			outPosition.PriceOpen = inPosition->PriceOpen();
			outPosition.PriceCurrent = inPosition->PriceCurrent();
			outPosition.PriceSL = inPosition->PriceSL();
			outPosition.PriceTP = inPosition->PriceTP();
			outPosition.Volume = inPosition->Volume();
			outPosition.Profit = inPosition->Profit();
			outPosition.Storage = inPosition->Storage();
			outPosition.ObsoleteValue = inPosition->ObsoleteValue();
			outPosition.RateProfit = inPosition->RateProfit();
			outPosition.RateMargin = inPosition->RateMargin();
			outPosition.ExpertID = inPosition->ExpertID();
			outPosition.ExpertPositionID = inPosition->ExpertPositionID();
			wcscpy_s(outPosition.Comment, inPosition->Comment());
			outPosition.ActivationMode = inPosition->ActivationMode();
			outPosition.ActivationTime = inPosition->ActivationTime();
			outPosition.ActivationPrice = inPosition->ActivationPrice();
			outPosition.ActivationFlags = inPosition->ActivationFlags();
			outPosition.TimeCreateMsc = inPosition->TimeCreateMsc();
			outPosition.TimeUpdateMsc = inPosition->TimeUpdateMsc();
			outPosition.Dealer = inPosition->Dealer();
			outPosition.Position = inPosition->Position();
			wcscpy_s(outPosition.ExternalID, inPosition->ExternalID());
			outPosition.ModificationFlags = inPosition->ModificationFlags();
			outPosition.Reason = inPosition->Reason();
			outPosition.VolumeExt = inPosition->VolumeExt();

			inPosition->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_LOGIN, outPosition.ApiDataLogin);
			inPosition->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_DEAL, outPosition.ApiDataDeal);
			inPosition->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_ORDER, outPosition.ApiDataOrder);
			inPosition->ApiDataGet(
				(USHORT)shared::EnMTApiAppID::EN_MTAPI_APPID_SKOBJ_SERVERAPI,
				(UCHAR)shared::EnMTApiParamType::EN_MTAPI_PARAMTYPE_POSITION, outPosition.ApiDataPosition);
		}
		static void ConfirmFromRequestField(const RequestField& inRequest, IMTConfirm* outConfirm) {
			if (!outConfirm) return;
			outConfirm->Clear();
			outConfirm->ID(inRequest.ID);
			outConfirm->Retcode(inRequest.ResultRetcode);
			outConfirm->Volume(inRequest.Volume);
			outConfirm->Price(inRequest.ResultPrice);
			outConfirm->TickBid(0);
			outConfirm->TickAsk(0);
			outConfirm->TickLast(0);
			outConfirm->Comment(inRequest.Comment);
			outConfirm->Flags(static_cast<UINT>(inRequest.Flags));
			outConfirm->DealID(tostringw(inRequest.ResultDeal).c_str());
			outConfirm->OrderID(tostringw(inRequest.Order).c_str());
			outConfirm->PriceGateway(inRequest.PriceOrder);
			outConfirm->PositionExternalID(inRequest.PositionExternalID);
			outConfirm->ExternalRetcode(inRequest.ResultRetcode);
			outConfirm->VolumeExt(inRequest.VolumeExt);
		}
		static void RequestMake(const RequestField& inRequest, IMTRequest* outRequest) {
			if (!outRequest) return;
			outRequest->Clear();
			outRequest->Login(inRequest.Login);
			outRequest->Symbol(inRequest.Symbol);
			outRequest->PriceOrder(inRequest.PriceOrder);
			outRequest->PriceSL(inRequest.PriceSL);
			outRequest->PriceTP(inRequest.PriceTP);
			outRequest->TimeExpiration(inRequest.TimeExpiration);
			outRequest->Volume(inRequest.Volume);
			outRequest->Type(inRequest.Type);
			outRequest->TypeTime(inRequest.TypeTime);
			outRequest->Action(inRequest.Action);
			outRequest->Comment(inRequest.Comment);
			outRequest->Position(inRequest.Position);
			outRequest->PositionExternalID(inRequest.PositionExternalID);
		}
		static void MakeConGroupSymbol(const IMTConGroupSymbol* in, ConGroupSymbolField& out) {
			if (!in) return;
			wcsncpy_s(out.Path, in->Path(),_countof(out.Path));
			out.TradeMode = in->TradeMode();
			out.TradeModeDefault = in->TradeModeDefault();
			out.ExecMode = in->ExecMode();
			out.ExecModeDefault = in->ExecModeDefault();
			out.FillFlags = in->FillFlags();
			out.FillFlagsDefault = in->FillFlagsDefault();
			out.ExpirFlags = in->ExpirFlags();
			out.ExpirFlagsDefault = in->ExpirFlagsDefault();
			out.SpreadDiff = in->SpreadDiff();
			out.SpreadDiffDefault = in->SpreadDiffDefault();
			out.SpreadDiffBalance = in->SpreadDiffBalance();
			out.SpreadDiffBalanceDefault = in->SpreadDiffBalanceDefault();
			out.StopsLevel = in->StopsLevel();
			out.StopsLevelDefault = in->StopsLevelDefault();
			out.FreezeLevel = in->FreezeLevel();
			out.FreezeLevelDefault = in->FreezeLevelDefault();
			out.VolumeMin = in->VolumeMin();
			out.VolumeMinDefault = in->VolumeMinDefault();
			out.VolumeMax = in->VolumeMax();
			out.VolumeMaxDefault = in->VolumeMaxDefault();
			out.VolumeStep = in->VolumeStep();
			out.VolumeStepDefault = in->VolumeStepDefault();
			out.VolumeLimit = in->VolumeLimit();
			out.VolumeLimitDefault = in->VolumeLimitDefault();
			out.MarginFlags = in->MarginFlags();
			out.MarginFlagsDefault = in->MarginFlagsDefault();
			out.MarginInitial = in->MarginInitial();
			out.MarginInitialDefault = in->MarginInitialDefault();
			out.MarginMaintenance = in->MarginMaintenance();
			out.MarginMaintenanceDefault = in->MarginMaintenanceDefault();
			out.MarginLong = in->MarginLong();
			out.MarginLongDefault = in->MarginLongDefault();
			out.MarginShort = in->MarginShort();
			out.MarginShortDefault = in->MarginShortDefault();
			out.MarginLimit = in->MarginLimit();
			out.MarginLimitDefault = in->MarginLimitDefault();
			out.MarginStop = in->MarginStop();
			out.MarginStopDefault = in->MarginStopDefault();
			out.MarginStopLimit = in->MarginStopLimit();
			out.MarginStopLimitDefault = in->MarginStopLimitDefault();
			out.SwapMode = in->SwapMode();
			out.SwapModeDefault = in->SwapModeDefault();
			out.SwapLong = in->SwapLong();
			out.SwapLongDefault = in->SwapLongDefault();
			out.SwapShort = in->SwapShort();
			out.SwapShortDefault = in->SwapShortDefault();
			out.Swap3Day = in->Swap3Day();
			out.Swap3DayDefault = in->Swap3DayDefault();
			out.RETimeout = in->RETimeout();
			out.RETimeoutDefault = in->RETimeoutDefault();
			out.IECheckMode = in->IECheckMode();
			out.IECheckModeDefault = in->IECheckModeDefault();
			out.IETimeout = in->IETimeout();
			out.IETimeoutDefault = in->IETimeoutDefault();
			out.IESlipProfit = in->IESlipProfit();
			out.IESlipProfitDefault = in->IESlipProfitDefault();
			out.IESlipLosing = in->IESlipLosing();
			out.IESlipLosingDefault = in->IESlipLosingDefault();
			out.IEVolumeMax = in->IEVolumeMax();
			out.IEVolumeMaxDefault = in->IEVolumeMaxDefault();
			out.OrderFlags = in->OrderFlags();
			out.OrderFlagsDefault = in->OrderFlagsDefault();
			out.MarginRateInitialDefault = in->MarginRateInitialDefault();
			out.MarginRateMaintenanceDefault = in->MarginRateMaintenanceDefault();
			out.MarginRateLiquidity = in->MarginRateLiquidity();
			out.MarginRateLiquidityDefault = in->MarginRateLiquidityDefault();
			out.REFlags = in->REFlags();
			out.REFlagsDefault = in->REFlagsDefault();
			out.MarginHedged = in->MarginHedged();
			out.MarginHedgedDefault = in->MarginHedgedDefault();
			out.PermissionsFlags = in->PermissionsFlags();
			out.MarginRateCurrency = in->MarginRateCurrency();
			out.MarginRateCurrencyDefault = in->MarginRateCurrencyDefault();
			out.BookDepthLimit = in->BookDepthLimit();
			out.IEFlags = in->IEFlags();
			out.IEFlagsDefault = in->IEFlagsDefault();
			out.VolumeMinExt = in->VolumeMinExt();
			out.VolumeMinExtDefault = in->VolumeMinExtDefault();
			out.VolumeMaxExt = in->VolumeMaxExt();
			out.VolumeMaxExtDefault = in->VolumeMaxExtDefault();
			out.VolumeStepExt = in->VolumeStepExt();
			out.VolumeStepExtDefault = in->VolumeStepExtDefault();
			out.VolumeLimitExt = in->VolumeLimitExt();
			out.VolumeLimitExtDefault = in->VolumeLimitExtDefault();
			out.IEVolumeMaxExt = in->IEVolumeMaxExt();
			out.IEVolumeMaxExtDefault = in->IEVolumeMaxExtDefault();
		}
		static void MakeRequestField(const IMTRequest* inRequest, RequestField& outRequest) {
			if (!inRequest) return;
			outRequest.ID = inRequest->ID();
			outRequest.Login = inRequest->Login();
			wcscpy_s(outRequest.Group, inRequest->Group());
			wcscpy_s(outRequest.Symbol, inRequest->Symbol());
			outRequest.Digits = inRequest->Digits();
			outRequest.Action = inRequest->Action();
			outRequest.TimeExpiration = inRequest->TimeExpiration();
			outRequest.Type = inRequest->Type();
			outRequest.TypeFill = inRequest->TypeFill();
			outRequest.TypeTime = inRequest->TypeTime();
			outRequest.Flags = inRequest->Flags();
			outRequest.Volume = inRequest->Volume();
			outRequest.Order = inRequest->Order();
			wcscpy_s(outRequest.OrderExternalID, inRequest->OrderExternalID());
			outRequest.PriceOrder = inRequest->PriceOrder();
			outRequest.PriceTrigger = inRequest->PriceTrigger();
			outRequest.PriceSL = inRequest->PriceSL();
			outRequest.PriceTP = inRequest->PriceTP();
			outRequest.PriceDeviation = inRequest->PriceDeviation();
			outRequest.PriceDeviationTop = inRequest->PriceDeviationTop();
			outRequest.PriceDeviationBottom = inRequest->PriceDeviationBottom();
			wcscpy_s(outRequest.Comment, inRequest->Comment());
			outRequest.ResultRetcode = inRequest->ResultRetcode();
			outRequest.ResultDealer = inRequest->ResultDealer();
			outRequest.ResultDeal = inRequest->ResultDeal();
			outRequest.ResultOrder = inRequest->ResultOrder();
			outRequest.ResultVolume = inRequest->ResultVolume();
			outRequest.ResultPrice = inRequest->ResultPrice();
			outRequest.ResultDealerBid = inRequest->ResultDealerBid();
			outRequest.ResultDealerAsk = inRequest->ResultDealerAsk();
			outRequest.ResultDealerLast = inRequest->ResultDealerLast();
			outRequest.ResultMarketBid = inRequest->ResultMarketBid();
			outRequest.ResultMarketAsk = inRequest->ResultMarketAsk();
			outRequest.ResultMarketLast = inRequest->ResultMarketLast();
			wcscpy_s(outRequest.ResultComment, inRequest->ResultComment());
			wcscpy_s(outRequest.ExternalAccount, inRequest->ExternalAccount());
			outRequest.IDClient = inRequest->IDClient();
			outRequest.SourceLogin = inRequest->SourceLogin();
			outRequest.Position = inRequest->Position();
			outRequest.PositionBy = inRequest->PositionBy();
			wcscpy_s(outRequest.PositionExternalID, inRequest->PositionExternalID());
			wcscpy_s(outRequest.PositionByExternalID, inRequest->PositionByExternalID());
			outRequest.VolumeExt = inRequest->VolumeExt();
			outRequest.ResultVolumeExt = inRequest->ResultVolumeExt();
		}
		static void MakeOrderField(const IMTOrder* inOrder, OrderField& outOrder) {
			if (!inOrder) return;
			outOrder.Order = inOrder->Order();
			wcscpy_s(outOrder.ExternalID, inOrder->ExternalID());
			outOrder.Login = inOrder->Login();
			outOrder.Dealer = inOrder->Dealer();
			wcscpy_s(outOrder.Symbol, inOrder->Symbol());
			outOrder.Digits = inOrder->Digits();
			outOrder.DigitsCurrency = inOrder->DigitsCurrency();
			outOrder.ContractSize = inOrder->ContractSize();
			outOrder.State = inOrder->State();
			outOrder.Reason = inOrder->Reason();
			outOrder.TimeSetup = inOrder->TimeSetup();
			outOrder.TimeExpiration = inOrder->TimeExpiration();
			outOrder.TimeDone = inOrder->TimeDone();
			outOrder.Type = inOrder->Type();
			outOrder.TypeFill = inOrder->TypeFill();
			outOrder.TypeTime = inOrder->TypeTime();
			outOrder.PriceOrder = inOrder->PriceOrder();
			outOrder.PriceTrigger = inOrder->PriceTrigger();
			outOrder.PriceCurrent = inOrder->PriceCurrent();
			outOrder.PriceSL = inOrder->PriceSL();
			outOrder.PriceTP = inOrder->PriceTP();
			outOrder.VolumeInitial = inOrder->VolumeInitial();
			outOrder.VolumeCurrent = inOrder->VolumeCurrent();
			outOrder.ExpertID = inOrder->ExpertID();
			outOrder.PositionID = inOrder->PositionID();
			wcscpy_s(outOrder.Comment, inOrder->Comment());
			outOrder.ActivationMode = inOrder->ActivationMode();
			outOrder.ActivationTime = inOrder->ActivationTime();
			outOrder.ActivationPrice = inOrder->ActivationPrice();
			outOrder.ActivationFlags = inOrder->ActivationFlags();
			outOrder.TimeSetupMsc = inOrder->TimeSetupMsc();
			outOrder.TimeDoneMsc = inOrder->TimeDoneMsc();
			outOrder.RateMargin = inOrder->RateMargin();
			outOrder.PositionByID = inOrder->PositionByID();
			outOrder.ModificationFlags = inOrder->ModificationFlags();
			outOrder.VolumeInitialExt = inOrder->VolumeInitialExt();
			outOrder.VolumeCurrentExt = inOrder->VolumeCurrentExt();
		}
		static void MakeConSymbolField(const IMTConSymbol* inSymbol, ConSymbolField& outSymbol) {
			if (!inSymbol) return;
			wcscpy_s(outSymbol.Symbol, inSymbol->Symbol());
			wcscpy_s(outSymbol.Path, inSymbol->Path());
			wcscpy_s(outSymbol.ISIN, inSymbol->ISIN());
			wcscpy_s(outSymbol.Description, inSymbol->Description());
			wcscpy_s(outSymbol.International, inSymbol->International());
			wcscpy_s(outSymbol.Basis, inSymbol->Basis());
			wcscpy_s(outSymbol.Source, inSymbol->Source());
			wcscpy_s(outSymbol.Page, inSymbol->Page());
			wcscpy_s(outSymbol.CurrencyBase, inSymbol->CurrencyBase());
			wcscpy_s(outSymbol.Symbol, inSymbol->Symbol());
			wcscpy_s(outSymbol.Symbol, inSymbol->Symbol());
			outSymbol.CurrencyBaseDigits = inSymbol->CurrencyBaseDigits();
			wcscpy_s(outSymbol.CurrencyProfit, inSymbol->CurrencyProfit());
			outSymbol.CurrencyProfitDigits = inSymbol->CurrencyProfitDigits();
			wcscpy_s(outSymbol.CurrencyMargin, inSymbol->CurrencyMargin());
			outSymbol.CurrencyMarginDigits = inSymbol->CurrencyMarginDigits();
			COLORREF  Color = inSymbol->Color();
			COLORREF  ColorBackground = inSymbol->ColorBackground();
			outSymbol.Digits = inSymbol->Digits();
			outSymbol.Point = inSymbol->Point();
			outSymbol.Multiply = inSymbol->Multiply();
			outSymbol.TickFlags = inSymbol->TickFlags();
			outSymbol.TickBookDepth = inSymbol->TickBookDepth();
			outSymbol.FilterSoft = inSymbol->FilterSoft();
			outSymbol.FilterSoftTicks = inSymbol->FilterSoftTicks();
			outSymbol.FilterHard = inSymbol->FilterHard();
			outSymbol.FilterHardTicks = inSymbol->FilterHardTicks();
			outSymbol.FilterDiscard = inSymbol->FilterDiscard();
			outSymbol.FilterSpreadMax = inSymbol->FilterSpreadMax();
			outSymbol.FilterSpreadMin = inSymbol->FilterSpreadMin();
			outSymbol.TradeMode = inSymbol->TradeMode();
			outSymbol.CalcMode = inSymbol->CalcMode();
			outSymbol.ExecMode = inSymbol->ExecMode();
			outSymbol.GTCMode = inSymbol->GTCMode();
			outSymbol.FillFlags = inSymbol->FillFlags();
			outSymbol.ExpirFlags = inSymbol->ExpirFlags();
			outSymbol.Spread = inSymbol->Spread();
			outSymbol.SpreadBalance = inSymbol->SpreadBalance();
			outSymbol.SpreadDiff = inSymbol->SpreadDiff();
			outSymbol.SpreadDiffBalance = inSymbol->SpreadDiffBalance();
			outSymbol.TickValue = inSymbol->TickValue();
			outSymbol.TickSize = inSymbol->TickSize();
			outSymbol.ContractSize = inSymbol->ContractSize();
			outSymbol.StopsLevel = inSymbol->StopsLevel();
			outSymbol.FreezeLevel = inSymbol->FreezeLevel();
			outSymbol.QuotesTimeout = inSymbol->QuotesTimeout();
			outSymbol.VolumeMin = inSymbol->VolumeMin();
			outSymbol.VolumeMax = inSymbol->VolumeMax();
			outSymbol.VolumeStep = inSymbol->VolumeStep();
			outSymbol.VolumeLimit = inSymbol->VolumeLimit();
			outSymbol.MarginFlags = inSymbol->MarginFlags();
			outSymbol.MarginInitial = inSymbol->MarginInitial();
			outSymbol.MarginMaintenance = inSymbol->MarginMaintenance();
			outSymbol.MarginLong = inSymbol->MarginLong();
			outSymbol.MarginShort = inSymbol->MarginShort();
			outSymbol.MarginLimit = inSymbol->MarginLimit();
			outSymbol.MarginStop = inSymbol->MarginStop();
			outSymbol.MarginStopLimit = inSymbol->MarginStopLimit();
			outSymbol.SwapMode = inSymbol->SwapMode();
			outSymbol.SwapLong = inSymbol->SwapLong();
			outSymbol.SwapShort = inSymbol->SwapShort();
			outSymbol.Swap3Day = inSymbol->Swap3Day();
			outSymbol.TimeStart = inSymbol->TimeStart();
			outSymbol.TimeExpiration = inSymbol->TimeExpiration();
			outSymbol.REFlags = inSymbol->REFlags();
			outSymbol.RETimeout = inSymbol->RETimeout();
			outSymbol.IECheckMode = inSymbol->IECheckMode();
			outSymbol.IETimeout = inSymbol->IETimeout();
			outSymbol.IESlipProfit = inSymbol->IESlipProfit();
			outSymbol.IESlipLosing = inSymbol->IESlipLosing();
			outSymbol.IEVolumeMax = inSymbol->IEVolumeMax();
			outSymbol.PriceSettle = inSymbol->PriceSettle();
			outSymbol.PriceLimitMax = inSymbol->PriceLimitMax();
			outSymbol.PriceLimitMin = inSymbol->PriceLimitMin();
			outSymbol.TradeFlags = inSymbol->TradeFlags();
			outSymbol.OrderFlags = inSymbol->OrderFlags();
			outSymbol.OptionsMode = inSymbol->OptionsMode();
			outSymbol.PriceStrike = inSymbol->PriceStrike();
			outSymbol.MarginRateLiqudity = inSymbol->MarginRateLiqudity();
			outSymbol.FaceValue = inSymbol->FaceValue();
			outSymbol.AccruedInterest = inSymbol->AccruedInterest();
			outSymbol.SpliceType = inSymbol->SpliceType();
			outSymbol.SpliceTimeType = inSymbol->SpliceTimeType();
			outSymbol.SpliceTimeDays = inSymbol->SpliceTimeDays();
			outSymbol.MarginHedged = inSymbol->MarginHedged();
			outSymbol.MarginRateCurrency = inSymbol->MarginRateCurrency();
			outSymbol.FilterGap = inSymbol->FilterGap();
			outSymbol.FilterGapTicks = inSymbol->FilterGapTicks();
			outSymbol.ChartMode = inSymbol->ChartMode();
			outSymbol.IEFlags = inSymbol->IEFlags();
			outSymbol.VolumeMinExt = inSymbol->VolumeMinExt();
			outSymbol.VolumeMaxExt = inSymbol->VolumeMaxExt();
			outSymbol.VolumeStepExt = inSymbol->VolumeStepExt();
			outSymbol.VolumeLimitExt = inSymbol->VolumeLimitExt();
			outSymbol.IEVolumeMaxExt = inSymbol->IEVolumeMaxExt();
		}
		static void MakeConGroupField(const IMTConGroup* inGroup, ConGroupField& outGroup) {
			if (!inGroup) return;
			wcscpy_s(outGroup.Group, inGroup->Group());
			outGroup.Server = inGroup->Server();
			outGroup.PermissionsFlags = inGroup->PermissionsFlags();
			outGroup.AuthMode = inGroup->AuthMode();
			outGroup.AuthPasswordMin = inGroup->AuthPasswordMin();
			wcscpy_s(outGroup.Company, inGroup->Company());
			wcscpy_s(outGroup.CompanyPage, inGroup->CompanyPage());
			wcscpy_s(outGroup.CompanyEmail, inGroup->CompanyEmail());
			wcscpy_s(outGroup.CompanySupportPage, inGroup->CompanySupportPage());
			wcscpy_s(outGroup.CompanySupportEmail, inGroup->CompanySupportEmail());
			wcscpy_s(outGroup.CompanyCatalog, inGroup->CompanyCatalog());
			wcscpy_s(outGroup.Currency, inGroup->Currency());
			outGroup.CurrencyDigits = inGroup->CurrencyDigits();
			outGroup.ReportsMode = inGroup->ReportsMode();
			outGroup.ReportsFlags = inGroup->ReportsFlags();
			wcscpy_s(outGroup.ReportsSMTP, inGroup->ReportsSMTP());
			wcscpy_s(outGroup.ReportsSMTPLogin, inGroup->ReportsSMTPLogin());
			wcscpy_s(outGroup.ReportsSMTPPass, inGroup->ReportsSMTPPass());
			outGroup.NewsMode = inGroup->NewsMode();
			wcscpy_s(outGroup.NewsCategory, inGroup->NewsCategory());
			outGroup.NewsLangTotal = inGroup->NewsLangTotal();
			outGroup.MailMode = inGroup->MailMode();
			outGroup.TradeFlags = inGroup->TradeFlags();
			outGroup.TradeInterestrate = inGroup->TradeInterestrate();
			outGroup.MarginFreeMode = inGroup->MarginFreeMode();
			outGroup.MarginSOMode = inGroup->MarginSOMode();
			outGroup.MarginCall = inGroup->MarginCall();
			outGroup.MarginStopOut = inGroup->MarginStopOut();
			outGroup.DemoLeverage = inGroup->DemoLeverage();
			outGroup.DemoDeposit = inGroup->DemoDeposit();
			outGroup.LimitHistory = inGroup->LimitHistory();
			outGroup.LimitOrders = inGroup->LimitOrders();
			outGroup.LimitSymbols = inGroup->LimitSymbols();
			outGroup.CommissionTotal = inGroup->CommissionTotal();
			outGroup.SymbolTotal = inGroup->SymbolTotal();
			outGroup.MarginFreeProfitMode = inGroup->MarginFreeProfitMode();
			outGroup.MarginMode = inGroup->MarginMode();
			outGroup.AuthOTPMode = inGroup->AuthOTPMode();
			outGroup.TradeTransferMode = inGroup->TradeTransferMode();
			outGroup.MarginFlags = inGroup->MarginFlags();
			outGroup.LimitPositions = inGroup->LimitPositions();
			wcscpy_s(outGroup.ReportsEmail, inGroup->ReportsEmail());
		}
		static void MakeUserField(const IMTUser* inUser, UserField& outUser) {
			if (!inUser) return;
			outUser.Login = inUser->Login();
			wcscpy_s(outUser.Group, inUser->Group());
			outUser.CertSerialNumber = inUser->CertSerialNumber();
			outUser.Rights = inUser->Rights();
			outUser.Registration = inUser->Registration();
			outUser.LastAccess = inUser->LastAccess();
			wcscpy_s(outUser.Name, inUser->Name());
			wcscpy_s(outUser.Company, inUser->Company());
			wcscpy_s(outUser.Account, inUser->Account());
			wcscpy_s(outUser.Country, inUser->Country());
			outUser.Language = inUser->Language();
			wcscpy_s(outUser.City, inUser->City());
			wcscpy_s(outUser.State, inUser->State());
			wcscpy_s(outUser.ZIPCode, inUser->ZIPCode());
			wcscpy_s(outUser.Address, inUser->Address());
			wcscpy_s(outUser.Phone, inUser->Phone());
			wcscpy_s(outUser.EMail, inUser->EMail());
			wcscpy_s(outUser.ID, inUser->ID());
			wcscpy_s(outUser.Status, inUser->Status());
			wcscpy_s(outUser.Comment, inUser->Comment());
			COLORREF  Color = inUser->Color();
			wcscpy_s(outUser.PhonePassword, inUser->PhonePassword());
			outUser.Leverage = inUser->Leverage();
			outUser.Agent = inUser->Agent();
			outUser.Balance = inUser->Balance();
			outUser.Credit = inUser->Credit();
			outUser.InterestRate = inUser->InterestRate();
			outUser.CommissionDaily = inUser->CommissionDaily();
			outUser.CommissionMonthly = inUser->CommissionMonthly();
			outUser.CommissionAgentDaily = inUser->CommissionAgentDaily();
			outUser.CommissionAgentMonthly = inUser->CommissionAgentMonthly();
			outUser.BalancePrevDay = inUser->BalancePrevDay();
			outUser.BalancePrevMonth = inUser->BalancePrevMonth();
			outUser.EquityPrevDay = inUser->EquityPrevDay();
			outUser.EquityPrevMonth = inUser->EquityPrevMonth();
			outUser.ExternalAccountTotal = inUser->ExternalAccountTotal();
			outUser.LastPassChange = inUser->LastPassChange();
			wcscpy_s(outUser.LeadSource, inUser->LeadSource());
		}
		static void MakeAccountField(const IMTAccount* inAccount, AccountField& outAccount) {
			if (!inAccount) return;
			outAccount.Login = inAccount->Login();
			outAccount.CurrencyDigits = inAccount->CurrencyDigits();
			outAccount.Balance = inAccount->Balance();
			outAccount.Credit = inAccount->Credit();
			outAccount.Margin = inAccount->Margin();
			outAccount.MarginFree = inAccount->MarginFree();
			outAccount.MarginLevel = inAccount->MarginLevel();
			outAccount.MarginLeverage = inAccount->MarginLeverage();
			outAccount.Profit = inAccount->Profit();
			outAccount.Storage = inAccount->Storage();
			outAccount.Commission = inAccount->Commission();
			outAccount.Floating = inAccount->Floating();
			outAccount.Equity = inAccount->Equity();
			outAccount.SOActivation = inAccount->SOActivation();
			outAccount.SOTime = inAccount->SOTime();
			outAccount.SOLevel = inAccount->SOLevel();
			outAccount.SOEquity = inAccount->SOEquity();
			outAccount.SOMargin = inAccount->SOMargin();
			outAccount.BlockedCommission = inAccount->BlockedCommission();
			outAccount.BlockedProfit = inAccount->BlockedProfit();
			outAccount.MarginInitial = inAccount->MarginInitial();
			outAccount.MarginMaintenance = inAccount->MarginMaintenance();
			outAccount.Assets = inAccount->Assets();
			outAccount.Liabilities = inAccount->Liabilities();
		}
		static void MakeConfirmField(const IMTConfirm* inConfirm, tagConfirmField& outConfirm) {
			if (!inConfirm) return;
			outConfirm.ID;
			outConfirm.Volume;
			outConfirm.Price;
			outConfirm.TickBid;
			outConfirm.TickAsk;
			outConfirm.TickLast;
			wcscpy_s(outConfirm.Comment, inConfirm->Comment());
			outConfirm.Flags;
			wcscpy_s(outConfirm.DealID, inConfirm->DealID());
			wcscpy_s(outConfirm.OrderID, inConfirm->OrderID());
			outConfirm.PriceGateway;
			wcscpy_s(outConfirm.PositionExternalID, inConfirm->PositionExternalID());
			outConfirm.ExternalRetcode;
			outConfirm.VolumeExt;
		}
		static void ConfirmMake(const tagConfirmField& inConfirm,IMTConfirm* outConfirm) {
			if (!outConfirm) return;
			outConfirm->Clear();
			outConfirm->ID(inConfirm.ID);
			outConfirm->Retcode(inConfirm.Retcode);
			outConfirm->Volume(inConfirm.Volume);
			outConfirm->Price(inConfirm.Price);
			outConfirm->TickBid(inConfirm.TickBid);
			outConfirm->TickAsk(inConfirm.TickAsk);
			outConfirm->TickLast(inConfirm.TickLast);
			outConfirm->Comment(inConfirm.Comment);
			outConfirm->Flags(inConfirm.Flags);
			outConfirm->DealID(inConfirm.DealID);
			outConfirm->OrderID(inConfirm.OrderID);
			outConfirm->PriceGateway(inConfirm.PriceGateway);
			outConfirm->PositionExternalID(inConfirm.PositionExternalID);
			outConfirm->ExternalRetcode(inConfirm.ExternalRetcode);
			outConfirm->VolumeExt(inConfirm.VolumeExt);
		}


		static int MakeSignalField(__in const tagDealField& mtdeal, __inout SignalField& signal, const std::function<void(SignalField&)>& replenish_cb) {
			memset(&signal, 0x00, sizeof(signal));
			signal.Action = mtdeal.Action;
			signal.Deal = mtdeal.Deal;
			signal.Entry = mtdeal.Entry;
			signal.Login = mtdeal.Login;
			signal.Order = mtdeal.Order;
			signal.Position = mtdeal.PositionID;
			signal.Price = mtdeal.Price;
			signal.Digits = mtdeal.Digits;
			signal.PriceSL = mtdeal.PriceSL;
			signal.PriceTP = mtdeal.PriceTP;
			wcscpy_s(signal.Symbol, mtdeal.Symbol);
			signal.Time = mtdeal.Time;
			signal.Volume = mtdeal.Volume;
			signal.VolumeClosed = mtdeal.VolumeClosed;
			if (replenish_cb) { replenish_cb(signal); }
			return 0;
		}


		public:
			template<typename ApiType, typename ObjType>
			static std::wstring MTParamenterGet(ApiType* apiObj, const ObjType* configObj, const std::wstring& name)
			{
				std::wstring result = L"";
				if (!configObj || !apiObj) return result;
				IMTConParam* param = apiObj->GatewayParamCreate();
				if (param)
				{
					for (UINT i = 0; i < configObj->ParameterTotal(); ++i)
					{
						param->Clear();
						if (MT_RET_OK != configObj->ParameterNext(i, param))
						{
							break;
						}
						if (!name.compare(param->Name()))
						{
							result = param->Value();
							break;
						}
					}
					param->Release();
				}
				return result;
			}
			static const char* MTServerTypeStrA(const UINT& type)
			{
				switch (IMTConServer::EnServerTypes(type))
				{
				case IMTConServer::EnServerTypes::NET_MAIN_TRADE_SERVER:
				{
					return "NET_MAIN_TRADE_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_TRADE_SERVER:
				{
					return "NET_TRADE_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_HISTORY_SERVER:
				{
					return "NET_HISTORY_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_ACCESS_SERVER:
				{
					return "NET_ACCESS_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_BACKUP_SERVER:
				{
					return "NET_BACKUP_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_OBSOLETE_SERVER_1:
				{
					return "NET_OBSOLETE_SERVER_1";
				}break;
				case IMTConServer::EnServerTypes::NET_OBSOLETE_SERVER_2:
				{
					return "NET_OBSOLETE_SERVER_2";
				}break;
				case IMTConServer::EnServerTypes::NET_ANTIDDOS_SERVER:
				{
					return "NET_ANTIDDOS_SERVER";
				}break;
				}
				return "未知";
			}
			static const wchar_t* MTServerTypeStrW(const UINT& type)
			{
				switch (IMTConServer::EnServerTypes(type))
				{
				case IMTConServer::EnServerTypes::NET_MAIN_TRADE_SERVER:
				{
					return L"NET_MAIN_TRADE_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_TRADE_SERVER:
				{
					return L"NET_TRADE_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_HISTORY_SERVER:
				{
					return L"NET_HISTORY_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_ACCESS_SERVER:
				{
					return L"NET_ACCESS_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_BACKUP_SERVER:
				{
					return L"NET_BACKUP_SERVER";
				}break;
				case IMTConServer::EnServerTypes::NET_OBSOLETE_SERVER_1:
				{
					return L"NET_OBSOLETE_SERVER_1";
				}break;
				case IMTConServer::EnServerTypes::NET_OBSOLETE_SERVER_2:
				{
					return L"NET_OBSOLETE_SERVER_2";
				}break;
				case IMTConServer::EnServerTypes::NET_ANTIDDOS_SERVER:
				{
					return L"NET_ANTIDDOS_SERVER";
				}break;
				}
				return L"未知";
			}
			static const char* DealEntryStrA(const UINT& entry) {
				switch (entry)
				{
				case IMTDeal::EnDealEntry::ENTRY_IN:
				{
					return "开仓";
				}break;
				case IMTDeal::EnDealEntry::ENTRY_OUT:
				{
					return "平仓";
				}break;
				case IMTDeal::EnDealEntry::ENTRY_INOUT:
				{
					return "对冲";
				}break;
				}
				return "未知";
			}
			static const TCHAR* DealEntryStr(const UINT& entry) {
				switch (entry)
				{
				case IMTDeal::EnDealEntry::ENTRY_IN:
				{
					return _T("开仓");
				}break;
				case IMTDeal::EnDealEntry::ENTRY_OUT:
				{
					return _T("平仓");
				}break;
				case IMTDeal::EnDealEntry::ENTRY_INOUT:
				{
					return _T("对冲");
				}break;
				}
				return _T("未知");
			}
			static const char* DealActionStrA(const UINT& action) {
				switch (action)
				{
				case IMTDeal::EnDealAction::DEAL_BUY:
				{
					return "多单";
				}break;
				case IMTDeal::EnDealAction::DEAL_SELL:
				{
					return "空单";
				}break;
				default:
				{
					return "未知";
				}
				}
				return "";
			}
			static const TCHAR* DealActionStr(const UINT& action) {
				switch (action)
				{
				case IMTDeal::EnDealAction::DEAL_BUY:
				{
					return _T("多单");
				}break;
				case IMTDeal::EnDealAction::DEAL_SELL:
				{
					return _T("空单");
				}break;
				default:
				{
					return _T("未知");
				}
				}
				return _T("");
			}
			static const char* ConGroupMarginModeStrA(const UINT& mode) {
				switch (mode)
				{
				case IMTConGroup::EnMarginMode::MARGIN_MODE_EXCHANGE_DISCOUNT:
				{
					return "MARGIN_MODE_EXCHANGE_DISCOUNT";
				}break;
				case IMTConGroup::EnMarginMode::MARGIN_MODE_RETAIL:
				{
					return "MARGIN_MODE_RETAIL";
				}break;
				case IMTConGroup::EnMarginMode::MARGIN_MODE_RETAIL_HEDGED:
				{
					return "MARGIN_MODE_RETAIL_HEDGED";
				}break;
				default:
				{
					return "UNKNOWN";
				}
				}
				return "";
			}

			static sk::stringa MTGMT(int Zone) {
				// server timezone in minutes (0=GMT;-60=GMT-1;60=GMT+1)
				bool minus = Zone < 0 ? true : false;
				auto value = Zone /= 60;
				if (minus) {
					return sk::stringa("GMT").append("-").append(tostringa(abs(value)));
				}
				else {
					return sk::stringa("GMT").append("+").append(tostringa(abs(value)));
				}
				return "GMT";
			}

			static bool VeirfyComment(const wchar_t* comment) {
				if (!comment) {
					return false;
				}
				auto nSrcLen = wcslen(comment);
				if (nSrcLen > (_countof(FollowField::IMTComment) - 1) || nSrcLen <= 1) {
					return false;
				}
				if (comment[0] == ROUTE_MTCMD_HEAD && comment[nSrcLen - 1] == ROUTE_MTCMD_TAIL) {
					return true;
				}
				return false;
			}









	};



}///namespace shared





/// /*新生联创（上海）**/
/// /*2019年11月15日 20:19:59|374**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_BUSINESS_H__1AB6C_34697472_CEA454390E7D5F5_768E__TAIL__
