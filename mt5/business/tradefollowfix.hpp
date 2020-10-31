#if !defined(AFX_SKSTU_TRADE_CALC_H__1E552_8B61DCC3_805AAECB1C0AF74_5F7B__HEAD__)
#define AFX_SKSTU_TRADE_CALC_H__1E552_8B61DCC3_805AAECB1C0AF74_5F7B__HEAD__

namespace shared {
	using namespace sk;

	class TradeFollowFix final {
	public:
		TradeFollowFix() {}
		~TradeFollowFix() = delete;
	public:
		static __inline SKAPIRES FollowFixAuthority
		(
			__in const SignalField&,
			__in const FollowField&
		);
		static __inline SKAPIRES FollowInFixFinal
		(
			__in SignalField&,
			__in const FollowField&,
			const std::function<void(__deref_out SignalField&)>&,
			const std::function<SKAPIRES(__deref_out const SignalField&, __deref_out const FollowField&, __deref_out RequestField&)>&
		);
		static __inline SKAPIRES FollowInVolumeFixFinal
		(
			__deref_inout sk::uint64& VolSrc,
			__in const sk::uint64& VolStep,
			__in const sk::uint64& VolMax,
			__in const sk::uint64& VolMin
		);
		static __inline SKAPIRES FollowEditPositionFixFinal
		(
			__in const PositionField&,
			__in const PositionField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowUpdatePositionFixFinal
		(
			__in const PositionField&,
			__inout PositionField&,
			__in const FollowField&
		);
		static __inline bool IsFollowUpdatePositionFixFinal
		(
			__in const PositionField&,
			__inout PositionField&,
			__in const FollowField&
		);
	public:
		static __inline SKAPIRES FollowInFixPriceSL
		(
			__in const FollowField& followCon,
			__in const UINT& signalAction, 
			__in const UINT& followAction,
			__in const double& signalSL, 
			__in const double& signalTP, 
			__in const UINT& signalDigits, 
			__out double& followSL
		);

		static __inline SKAPIRES FollowInFixPriceTP
		(
			__in const FollowField& followCon,
			__in const UINT& signalAction,
			__in const UINT& followAction,
			__in const double& signalSL,
			__in const double& signalTP,
			__in const UINT& signalDigits,
			__out double& followTP
		);
	private:
		static __inline SKAPIRES FollowInFixSymbol
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowInFixPrice
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowInFixType
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowInFixVolume
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowInFixComment
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowInFixRequestAction
		(
			__in const SignalField&,
			__in const FollowField&,
			__inout RequestField&
		);



	public:
		static __inline SKAPIRES FollowOutFixFinal
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			const std::function<SKAPIRES(__deref_out const SignalField&, __deref const FollowField&, __deref_out const PositionField&, __deref_out RequestField&)>&
		);
		static __inline SKAPIRES FollowOutVolumeFixFinal
		(
			__deref_inout sk::uint64& VolSrc,
			__in const sk::uint64& VolStep,
			__in const sk::uint64& VolMax,
			__in const sk::uint64& VolMin
		);
	private:
		static __inline SKAPIRES FollowOutFixSymbol
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowOutFixPrice
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowOutFixType
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowOutFixVolume
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowOutFixComment
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
		static __inline SKAPIRES FollowOutFixRequestAction
		(
			__in const SignalField&,
			__in const shared::FollowField&,
			__in const PositionField&,
			__inout RequestField&
		);
	public:
		static timestamp
			TimeStampToTimeFrameTimestamp(const timestamp& srcTime/*时差修复后的值*/)
		{
			stringa strTimeCalc = time::Time::TimestampToTimeStrNoDate(srcTime, false);
			if (strTimeCalc == "00:00:00")
			{
				return 90060;
			}
			std::vector<stringa> vrParse;
			Helper::OneUnknownParseA(strTimeCalc, ':', vrParse);
			if (vrParse.size() == 3)
			{
				return strtoll(vrParse.at(0).c_str(), nullptr, 10) * 3600 + strtoll(vrParse.at(1).c_str(), nullptr, 10) * 60 + strtoll(vrParse.at(2).c_str(), nullptr, 10);
			}
			return 0;
		}
	};

	__inline SKAPIRES
		TradeFollowFix::FollowInFixFinal
		(
			__in SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			const std::function<void(__deref_out SignalField&)>& _CbFollowBegin,
			const std::function<SKAPIRES(__deref_out const SignalField&, __deref_out const FollowField&, __deref_out RequestField&)>& _CbFollowEnd
		)
	{

		if (!_CbFollowBegin || !_CbFollowEnd)
		{
			return SKAPIRES::SYSTEM_ERROR_POINTER_NULL;
		}

		_CbFollowBegin(_ObjSignal);

		auto Result = SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;

		RequestField _ObjRequest;

		_ObjRequest.Login = _ObjFollow.LoginFollow;

		if ((Result = FollowFixAuthority(_ObjSignal, _ObjFollow)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixSymbol(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixType(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixVolume(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixPrice(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixComment(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		if ((Result = FollowInFixRequestAction(_ObjSignal, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS)
		{
			return Result;
		}
		return _CbFollowEnd(_ObjSignal, _ObjFollow, _ObjRequest);
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutFixFinal
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			const std::function<SKAPIRES(__deref_out const SignalField&, __deref const FollowField&, __deref_out const PositionField&, __deref_out RequestField&)>& _CbFollowEnd
		)
	{
		if (!_CbFollowEnd)
		{
			return SKAPIRES::SYSTEM_ERROR_POINTER_NULL;
		}

		RequestField _ObjRequest;

		_ObjRequest.Login = _ObjFollow.Login;

		_ObjRequest.Position = _ObjFollow.Position;

		_ObjRequest.Digits = _ObjFollow.Digits;

		auto Result = SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;

		if ((Result = FollowOutFixSymbol(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}
		if ((Result = FollowOutFixType(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}
		if ((Result = FollowOutFixVolume(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}
		if ((Result = FollowOutFixPrice(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}
		if ((Result = FollowOutFixComment(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}
		if ((Result = FollowOutFixRequestAction(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest)) != SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS) {
			return Result;
		}

		return _CbFollowEnd(_ObjSignal, _FollowCon, _ObjFollow, _ObjRequest);
	}



	__inline SKAPIRES
		TradeFollowFix::FollowFixAuthority
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow
		)
	{
		if (_ObjFollow.FollowFlag == FollowField::EnFollowFlag::EN_TRADE_FLAG_DISABLE)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_WARN_FOLLOW_DISABLE;
		}

		if (_ObjFollow.PositionOverMax > 0)
		{
			if (_ObjSignal.FollowPositionTotal >= _ObjFollow.PositionOverMax)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_WARN_POSITION_COUNT_OVERRUN;
			}
		}

		if (_ObjFollow.TypeType == FollowField::EnTypeType::EN_TYPE_TYPE_BUY)
		{
			if (_ObjSignal.Action != IMTDeal::EnDealAction::DEAL_BUY)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TYPETYPE_BROKEN;
			}
		}
		else if (_ObjFollow.TypeType == FollowField::EnTypeType::EN_TYPE_TYPE_SELL)
		{
			if (_ObjSignal.Action != IMTDeal::EnDealAction::DEAL_SELL)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TYPETYPE_BROKEN;
			}
		}

		if (_ObjFollow.TimeFrameFollowFlag == shared::FollowField::EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_ENABLE)
		{
			sk::timestamp TimeFrameTimestamp = TimeStampToTimeFrameTimestamp(_ObjSignal.TimeLocal);
			if (!(_ObjFollow.TimeFrameFollowBegin[0] <= 0 && _ObjFollow.TimeFrameFollowEnd[0] >= 90060))
			{
				if (_ObjFollow.TimeFrameFollowBegin[0] <= TimeFrameTimestamp && TimeFrameTimestamp <= _ObjFollow.TimeFrameFollowEnd[0])
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TIMEFRAME_BROKEN;
				}
			}
			if (!(_ObjFollow.TimeFrameFollowBegin[1] <= 0 && _ObjFollow.TimeFrameFollowEnd[1] >= 90060)) {
				if (_ObjFollow.TimeFrameFollowBegin[1] <= TimeFrameTimestamp && TimeFrameTimestamp <= _ObjFollow.TimeFrameFollowEnd[1])
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TIMEFRAME_BROKEN;
				}
			}
			if (!(_ObjFollow.TimeFrameFollowBegin[2] <= 0 && _ObjFollow.TimeFrameFollowEnd[2] >= 90060)) {
				if (_ObjFollow.TimeFrameFollowBegin[2] <= TimeFrameTimestamp && TimeFrameTimestamp <= _ObjFollow.TimeFrameFollowEnd[2])
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_TIMEFRAME_BROKEN;
				}
			}
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutFixSymbol
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		SafeCopyW(_ObjRequest.Symbol, _ObjFollow.Symbol, _countof(_ObjRequest.Symbol));
		if (_ObjRequest.Symbol[0] == 0)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_SYMBOL_NULL;
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowInFixSymbol(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{

		if (_ObjFollow.SymbolReject[0] == '*')
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_WARN_SYMBOL_NOTFOLLOW;
		}

		sk::stringw
			SymbolSrc(_ObjSignal.Symbol),
			SymbolHostSignal, /*信号源主路径品种*/
			SymbolSuffixSignal, /*信号源品种后缀*/
			SymbolSuffixFollow(_ObjFollow.SymbolSuffix),/*跟单者需求后缀*/
			SymbolResult;/*结果品种*/

		size_t pos = SymbolSrc.find('.');
		if (pos != sk::stringw::npos)
		{
			SymbolSuffixSignal = SymbolSrc.substr(pos, SymbolSrc.length() - pos);
			SymbolHostSignal = SymbolSrc.substr(0, SymbolSrc.find(SymbolSuffixSignal));
		}
		else {
			SymbolHostSignal = SymbolSrc;
		}

		//! 校正跟单者后缀 
		//! 如果配置未设置，*则跟随信号源主路径品种
		SymbolSuffixFollow.empty() ? \
			SymbolResult = SymbolHostSignal : \
			SymbolResult = SymbolHostSignal + SymbolSuffixFollow;

		SafeCopyW(_ObjRequest.Symbol, SymbolResult.c_str(), _countof(_ObjRequest.Symbol));

		/*检查不跟随品种列表*/
		if (_ObjFollow.SymbolReject[0] != 0)
		{
			if (nullptr != wcschr(_ObjFollow.SymbolReject, ','))
			{
				if (nullptr != wcsstr(_ObjFollow.SymbolReject, (SymbolResult + L",").c_str()))
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_SYMBOL_NOTFOLLOW;
				}
			}
			else
			{
				if (nullptr != wcsstr(_ObjFollow.SymbolReject, SymbolResult.c_str()))
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_SYMBOL_NOTFOLLOW;
				}
			}
		}
		/*检查只跟随品种列表*/
		if (_ObjFollow.SymbolAllow[0] != 0) {
			if (nullptr != wcschr(_ObjFollow.SymbolAllow, ','))
			{
				if (nullptr == wcsstr(_ObjFollow.SymbolAllow, (SymbolResult + L",").c_str()))
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_SYMBOL_ONLYFOLLOW;
				}
			}
			else {
				if (nullptr == wcsstr(_ObjFollow.SymbolAllow, SymbolResult.c_str()))
				{
					return SKAPIRES::TRADE_FOLLOWOBJ_WARN_SYMBOL_ONLYFOLLOW;
				}
			}
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutFixPrice
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		_ObjRequest.PriceOrder = 0/*_ObjFollow.Price*/;
		_ObjRequest.Flags = IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET;
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowUpdatePositionFixFinal
		(
			__in const PositionField& _ObjPositionSignal,
			__inout PositionField& _ObjPositionFollow,
			__in const FollowField& _ObjFollow
		)
	{
		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE != _ObjFollow.PointSLTPFlag)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_WARN_EDITPOSSLTP_DISABLE;
		}

		if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_BUY)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				_ObjPositionFollow.PriceSL = _ObjPositionSignal.PriceSL;
				_ObjPositionFollow.PriceTP = _ObjPositionSignal.PriceTP;
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				_ObjPositionFollow.PriceSL = _ObjPositionSignal.PriceTP;
				_ObjPositionFollow.PriceTP = _ObjPositionSignal.PriceSL;
			}
		}
		else if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_SELL)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				_ObjPositionFollow.PriceSL = _ObjPositionSignal.PriceTP;
				_ObjPositionFollow.PriceTP = _ObjPositionSignal.PriceSL;
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				_ObjPositionFollow.PriceSL = _ObjPositionSignal.PriceSL;
				_ObjPositionFollow.PriceTP = _ObjPositionSignal.PriceTP;
			}
		}
		if (_ObjPositionFollow.PriceTP > 0) {
			_ObjPositionFollow.PriceTP += _ObjFollow.PointSLTP / pow(10, _ObjPositionFollow.Digits);
		}
		if (_ObjPositionFollow.PriceSL > 0) {
			_ObjPositionFollow.PriceSL += _ObjFollow.PointSLTP / pow(10, _ObjPositionFollow.Digits);
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline bool
		TradeFollowFix::IsFollowUpdatePositionFixFinal
		(
			__in const PositionField& _ObjPositionSignal,
			__inout PositionField& _ObjPositionFollow,
			__in const FollowField& _ObjFollow
		)
	{
		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE != _ObjFollow.PointSLTPFlag)
		{
			return false;
		}

		auto patch = _ObjFollow.PointSLTP / pow(10, _ObjPositionFollow.Digits);

		if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_BUY)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				if (_ObjPositionFollow.PriceSL != (_ObjPositionSignal.PriceSL - patch) || _ObjPositionFollow.PriceTP != (_ObjPositionSignal.PriceTP - patch))
				{
					return true;
				}
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				if (_ObjPositionFollow.PriceSL != (_ObjPositionSignal.PriceTP - patch) || _ObjPositionFollow.PriceTP != (_ObjPositionSignal.PriceSL - patch))
				{
					return true;
				}
			}
		}
		else if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_SELL)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				if (_ObjPositionFollow.PriceSL != (_ObjPositionSignal.PriceTP - patch) || _ObjPositionFollow.PriceTP != (_ObjPositionSignal.PriceSL - patch))
				{
					return true;
				}
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				if (_ObjPositionFollow.PriceSL != (_ObjPositionSignal.PriceSL - patch) || _ObjPositionFollow.PriceTP != (_ObjPositionSignal.PriceTP - patch))
				{
					return true;
				}
			}
		}
		return false;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowEditPositionFixFinal
		(
			__in const PositionField& _ObjPositionSignal,
			__in const PositionField& _ObjPositionFollow,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _OutRequest
		)
	{
		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE != _ObjFollow.PointSLTPFlag)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_WARN_EDITPOSSLTP_DISABLE;
		}

		_OutRequest.Login = _ObjPositionFollow.Login;
		SafeCopyW(_OutRequest.Symbol, _ObjPositionFollow.Symbol, _countof(_OutRequest.Symbol));
		_OutRequest.Position = _ObjPositionFollow.Position;
		_OutRequest.TimeExpiration = 0;
		_OutRequest.TypeTime = IMTOrder::EnOrderTime::ORDER_TIME_GTC;
		_OutRequest.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY;
		sk::stringw sComment;
		sComment.append(1, ROUTE_MTCMD_HEAD).append(L"#").append(tostringw(_ObjPositionFollow.Position)).append(L"#").append(tostringw(_ObjPositionSignal.Position)).append(1, ROUTE_MTCMD_TAIL);
		wmemcpy(_OutRequest.Comment, sComment.c_str(), sComment.length());

		if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_BUY)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				_OutRequest.PriceSL = _ObjPositionSignal.PriceSL;
				_OutRequest.PriceTP = _ObjPositionSignal.PriceTP;
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				_OutRequest.PriceSL = _ObjPositionSignal.PriceTP;
				_OutRequest.PriceTP = _ObjPositionSignal.PriceSL;
			}
		}
		else if (_ObjPositionFollow.Action == IMTPosition::EnPositionAction::POSITION_SELL)
		{
			if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_BUY)
			{
				_OutRequest.PriceSL = _ObjPositionSignal.PriceTP;
				_OutRequest.PriceTP = _ObjPositionSignal.PriceSL;
			}
			else if (_ObjPositionSignal.Action == IMTPosition::EnPositionAction::POSITION_SELL)
			{
				_OutRequest.PriceSL = _ObjPositionSignal.PriceSL;
				_OutRequest.PriceTP = _ObjPositionSignal.PriceTP;
			}
		}
		if (_OutRequest.PriceTP > 0) {
			_OutRequest.PriceTP += _ObjFollow.PointSLTP / pow(10, _ObjPositionFollow.Digits);
		}
		if (_OutRequest.PriceSL > 0) {
			_OutRequest.PriceSL += _ObjFollow.PointSLTP / pow(10, _ObjPositionFollow.Digits);
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES 
		TradeFollowFix::FollowInFixPriceSL
	(
		__in const FollowField& _ObjFollow,
		__in const UINT& signalAction,
		__in const UINT& followAction,
		__in const double& signalSL,
		__in const double& signalTP,
		__in const UINT& signalDigits,
		__out double& followSL
	)
	{
		followSL = 0;

		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE == _ObjFollow.PointSLTPFlag)
		{
			if (followAction == IMTOrder::EnOrderType::OP_BUY)
			{
				if (signalAction == IMTDeal::EnDealAction::DEAL_BUY)
				{
					followSL = signalSL;
				}
				else if (signalAction == IMTDeal::EnDealAction::DEAL_SELL)
				{
					followSL = signalTP;
				}
			}
			else if (followAction == IMTOrder::EnOrderType::OP_SELL)
			{
				if (signalAction == IMTDeal::EnDealAction::DEAL_BUY)
				{
					followSL = signalTP;
				}
				else if (signalAction == IMTDeal::EnDealAction::DEAL_SELL)
				{
					followSL = signalSL;
				}
			}

			if (followSL > 0)
			{
				followSL += _ObjFollow.PointSLTP / pow(10, signalDigits);
			}
		}

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}
	__inline SKAPIRES
		TradeFollowFix::FollowInFixPriceTP
		(
			__in const FollowField& _ObjFollow,
			__in const UINT& signalAction,
			__in const UINT& followAction,
			__in const double& signalSL,
			__in const double& signalTP,
			__in const UINT& signalDigits,
			__out double& followTP
		)
	{
		followTP = 0;
		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE == _ObjFollow.PointSLTPFlag)
		{
			if (followAction == IMTOrder::EnOrderType::OP_BUY)
			{
				if (signalAction == IMTDeal::EnDealAction::DEAL_BUY)
				{
					followTP = signalTP;
				}
				else if (signalAction == IMTDeal::EnDealAction::DEAL_SELL)
				{
					followTP = signalSL;
				}
			}
			else if (followAction == IMTOrder::EnOrderType::OP_SELL)
			{
				if (signalAction == IMTDeal::EnDealAction::DEAL_BUY)
				{
					followTP = signalSL;
				}
				else if (signalAction == IMTDeal::EnDealAction::DEAL_SELL)
				{
					followTP = signalTP;
				}
			}

			if (followTP > 0)
			{
				followTP += _ObjFollow.PointSLTP / pow(10, signalDigits);
			}
		}

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}
	__inline SKAPIRES
		TradeFollowFix::FollowInFixPrice
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		_ObjRequest.PriceOrder = _ObjSignal.Price;
		if (FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE == _ObjFollow.PointSLTPFlag) {

			_ObjRequest.PriceSL = 0;
			_ObjRequest.PriceTP = 0;
			_ObjRequest.Flags = IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET;

			/*if (_ObjFollow.TypeDirection == FollowField::EnTypeDriection::EN_TYPE_DRIECTION_PROS) {
				_ObjRequest.PriceTP = _ObjSignal.PriceTP;
				_ObjRequest.PriceSL = _ObjSignal.PriceSL;
			}
			else {

			}*/

			if (_ObjRequest.Type == IMTOrder::EnOrderType::OP_BUY)
			{
				if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_BUY)
				{
					_ObjRequest.PriceSL = _ObjSignal.PriceSL;
					_ObjRequest.PriceTP = _ObjSignal.PriceTP;
				}
				else if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_SELL)
				{
					_ObjRequest.PriceSL = _ObjSignal.PriceTP;
					_ObjRequest.PriceTP = _ObjSignal.PriceSL;
				}
			}
			else if (_ObjRequest.Type == IMTOrder::EnOrderType::OP_SELL)
			{
				if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_BUY)
				{
					_ObjRequest.PriceSL = _ObjSignal.PriceTP;
					_ObjRequest.PriceTP = _ObjSignal.PriceSL;
				}
				else if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_SELL)
				{
					_ObjRequest.PriceSL = _ObjSignal.PriceSL;
					_ObjRequest.PriceTP = _ObjSignal.PriceTP;
				}
			}

			if (_ObjRequest.PriceTP > 0)
			{
				_ObjRequest.PriceTP += _ObjFollow.PointSLTP / pow(10, _ObjSignal.Digits);
			}

			if (_ObjRequest.PriceSL > 0)
			{
				_ObjRequest.PriceSL += _ObjFollow.PointSLTP / pow(10, _ObjSignal.Digits);
			}
		}

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}


	__inline SKAPIRES
		TradeFollowFix::FollowOutFixType
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		if (_ObjFollow.Action == IMTPosition::EnPositionAction::POSITION_BUY)
		{
			_ObjRequest.Type = IMTOrder::EnOrderType::OP_SELL;
		}
		else if (_ObjFollow.Action == IMTPosition::EnPositionAction::POSITION_SELL)
		{
			_ObjRequest.Type = IMTOrder::EnOrderType::OP_BUY;
		}
		else
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_NOTSUPPORTED_POSITION_ACTION;
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowInFixType
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		if (FollowField::EnTypeDriection::EN_TYPE_DRIECTION_PROS == _ObjFollow.TypeDirection)
		{
			if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_BUY)
			{
				_ObjRequest.Type = IMTOrder::EnOrderType::OP_BUY;
			}
			else if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_SELL)
			{
				_ObjRequest.Type = IMTOrder::EnOrderType::OP_SELL;
			}
		}
		else if (FollowField::EnTypeDriection::EN_TYPE_DRIECTION_CONS == _ObjFollow.TypeDirection)
		{
			if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_BUY)
			{
				_ObjRequest.Type = IMTOrder::EnOrderType::OP_SELL;
			}
			else if (_ObjSignal.Action == IMTDeal::EnDealAction::DEAL_SELL)
			{
				_ObjRequest.Type = IMTOrder::EnOrderType::OP_BUY;
			}
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutFixVolume
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		if (_ObjSignal.VolumeTotal <= 0)
		{
			_ObjRequest.Volume = _ObjFollow.Volume;
		}
		else
		{
			auto SignalOutRatio = static_cast<sk::real64>(_ObjSignal.VolumeClosed) / static_cast<sk::real64>(_ObjSignal.VolumeClosed + _ObjSignal.VolumeTotal);
			_ObjRequest.Volume = static_cast<decltype(_ObjRequest.Volume)>(_ObjFollow.Volume * SignalOutRatio);
		}

		if (_ObjRequest.Volume <= 0)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_VOLUME_NULL;
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}


	__inline SKAPIRES
		TradeFollowFix::FollowInFixVolume
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		if (_ObjFollow.FollowMode == FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_FIXED)
		{
			_ObjRequest.Volume = SMTMath::VolumeToInt(_ObjFollow.VolumeFixed);
		}
		else if (_ObjFollow.FollowMode == FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO)
		{
			_ObjRequest.Volume = static_cast<decltype(_ObjRequest.Volume)>(_ObjFollow.VolumeRatio * _ObjSignal.Volume);
		}
		else if (_ObjFollow.FollowMode == FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RANGE)
		{
			_ObjRequest.Volume = static_cast<decltype(_ObjRequest.Volume)>(_ObjFollow.VolumeRatio * _ObjSignal.Volume);
			if (_ObjRequest.Volume < SMTMath::VolumeToInt(_ObjFollow.VolumeMin))
			{
				_ObjRequest.Volume = SMTMath::VolumeToInt(_ObjFollow.VolumeMin);
			}
			else if (_ObjRequest.Volume > SMTMath::VolumeToInt(_ObjFollow.VolumeMax))
			{
				_ObjRequest.Volume = SMTMath::VolumeToInt(_ObjFollow.VolumeMax);
			}
		}

		if (_ObjFollow.VolumeOverMax > 0)
		{
			if (_ObjSignal.Volume > SMTMath::VolumeToInt(_ObjFollow.VolumeOverMax))
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_WARN_OVERRUN_VOLUME;
			}
		}

		if (_ObjFollow.VolumeOverMin > 0)
		{
			if (_ObjSignal.Volume < SMTMath::VolumeToInt(_ObjFollow.VolumeOverMin))
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_WARN_OVERRUN_VOLUME;
			}
		}


		if (_ObjRequest.Volume <= 0)
		{
			return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_VOLUME_NULL;
		}

		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}


	__inline SKAPIRES
		TradeFollowFix::FollowOutFixComment
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		sk::stringw sComment;
		sComment.append(1, ROUTE_MTCMD_HEAD).append(1, ROUTE_MTCMD_TAIL);
		wmemcpy(_ObjRequest.Comment, sComment.c_str(), sComment.length());
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}


	__inline SKAPIRES
		TradeFollowFix::FollowInFixComment
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		sk::stringw sComment;
		if (_ObjFollow.IMTComment[0] != 0)
		{
			sComment = _ObjFollow.IMTComment;
		}
		else
		{
			//! Deal 为零的情况
			//! 强跟/补单 进场的时候信号源没有Deal
			if (_ObjSignal.Deal <= 0)
			{
				sComment.append(L"#").append(tostringw(_ObjSignal.Position));
			}
			else
			{
				sComment.append(L"#").append(tostringw(_ObjSignal.Deal)).append(L"#").append(tostringw(_ObjSignal.Position));
			}
		}
		sComment.insert(0, 1, ROUTE_MTCMD_HEAD);
		if (sComment.length() >= _countof(FollowField::IMTComment))
		{
			sComment[_countof(FollowField::IMTComment) - 2] = ROUTE_MTCMD_TAIL;
			sComment[_countof(FollowField::IMTComment) - 1] = 0;
		}
		else
		{
			sComment.push_back(ROUTE_MTCMD_TAIL);
		}
		wmemcpy(_ObjRequest.Comment, sComment.c_str(), sComment.length());
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowInFixRequestAction
		(
			__in const SignalField& _ObjSignal,
			__in const FollowField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		_ObjRequest.Flags = IMTRequest::EnTradeActionFlags::TA_FLAG_ALL;
		_ObjRequest.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE;
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutFixRequestAction
		(
			__in const SignalField& _ObjSignal,
			__in const shared::FollowField& _FollowCon,
			__in const PositionField& _ObjFollow,
			__inout RequestField& _ObjRequest
		)
	{
		_ObjRequest.Action = IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE/*TA_DEALER_CLOSE_BY*/;
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}


















	__inline SKAPIRES
		TradeFollowFix::FollowInVolumeFixFinal
		(
			__deref_inout sk::uint64& VolSrc,
			__in const sk::uint64& VolStep,
			__in const sk::uint64& VolMax,
			__in const sk::uint64& VolMin
		)
	{
		if (VolStep > 0)
		{
			if (VolSrc < VolStep)
			{
				VolSrc = VolStep;
			}
			if ((VolSrc % VolStep) >= VolStep / 2)
			{
				VolSrc += VolStep;
			}
			else
			{
				VolSrc = VolSrc - VolSrc % VolStep;
			}

			VolSrc = VolSrc - (VolSrc % VolStep);

			if (VolSrc <= 0)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_VOLUME_NULL;
			}
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}

	__inline SKAPIRES
		TradeFollowFix::FollowOutVolumeFixFinal
		(
			__deref_inout sk::uint64& VolSrc,
			__in const sk::uint64& VolStep,
			__in const sk::uint64& VolMax,
			__in const sk::uint64& VolMin
		)
	{
		if (VolStep > 0)
		{
			if (VolSrc < VolStep)
			{
				VolSrc = VolStep;
			}
			if ((VolSrc % VolStep) >= VolStep / 2)
			{
				VolSrc += VolStep;
			}
			else
			{
				VolSrc = VolSrc - VolSrc % VolStep;
			}
			VolSrc = VolSrc - (VolSrc % VolStep);
			if (VolSrc <= 0)
			{
				return SKAPIRES::TRADE_FOLLOWOBJ_ERROR_VOLUME_NULL;
			}
		}
		return SKAPIRES::TRADE_FOLLOWOBJ_SUCCESS;
	}















}///namespace shared




/// /*新生联创（上海）**/
/// /*2019年08月7日 9:50:21|294**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_TRADE_CALC_H__1E552_8B61DCC3_805AAECB1C0AF74_5F7B__TAIL__