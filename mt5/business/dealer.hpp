#if !defined(AFX_SK_H__9A81410D_6CC1_4688_81C0_78911670BB21__HEAD__)
#define AFX_SK_H__9A81410D_6CC1_4688_81C0_78911670BB21__HEAD__

namespace shared {

	class Dealer final {
	public:
		Dealer() {}
		~Dealer() {}
	public:
		//!签名 : GetOrderActionByRequestAction
		//!说明 : 设置定单请求类型
		//!注意 : 
		//!日期 : Sun May 24 00:40:16 UTC+0800 2020
		static void GetOrderActionByRequestAction(const UINT& mt_request_action, shared::tradeorder::EnOrderAction& trade_order_action)
		{
			switch (mt_request_action)
			{
			case IMTRequest::TA_PRICE:
			case IMTRequest::TA_INSTANT:
			case IMTRequest::TA_MARKET:
			case IMTRequest::TA_EXCHANGE:
			case IMTRequest::TA_PENDING:
			case IMTRequest::TA_ACTIVATE:
			case IMTRequest::TA_ACTIVATE_SL:
			case IMTRequest::TA_ACTIVATE_TP:
			case IMTRequest::TA_ACTIVATE_STOPLIMIT:
			case IMTRequest::TA_STOPOUT_POSITION:
			case IMTRequest::TA_DEALER_POS_EXECUTE:
			case IMTRequest::TA_DEALER_ORD_PENDING:
			{
				trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_NEW;
			}break;
			case IMTRequest::TA_MODIFY:
			case IMTRequest::TA_DEALER_ORD_MODIFY:
			{
				trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_MODIFY;
			}break;
			case IMTRequest::TA_EXPIRATION:
			case IMTRequest::TA_REMOVE:
			case IMTRequest::TA_STOPOUT_ORDER:
			case IMTRequest::TA_DEALER_ORD_REMOVE:
			{
				trade_order_action = shared::tradeorder::EnOrderAction::EN_ORDER_ACTION_CANCEL;
			}break;
			}///switch
		}
		//!签名 : GatewayProcessAuto
		//!说明 : 判断由平台自动确认处理定单类型/请求类型
		//!注意 : MT5 Admin 路由选项 请求类型(所有)/订单类型(所有)
		//!日期 : Sun May 24 00:38:00 UTC+0800 2020
		static bool GatewayProcessAuto(const IMTRequest* request)
		{
			if (request->ResultOrder() <= 0)
			{
				return true;
			}

			bool Stop = true;
			switch (request->Type())
			{
			case IMTOrder::EnOrderType::OP_BUY:
			case IMTOrder::EnOrderType::OP_SELL:
			case IMTOrder::EnOrderType::OP_BUY_LIMIT:
			case IMTOrder::EnOrderType::OP_SELL_LIMIT:
			case IMTOrder::EnOrderType::OP_CLOSE_BY:
			{
				if ((request->Flags() & IMTRequest::EnTradeActionFlags::TA_FLAG_CLOSE ||
					request->Flags() & IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET) &&
					(request->Action() == IMTRequest::EnTradeActions::TA_ACTIVATE_SL || request->Action() == IMTRequest::EnTradeActions::TA_ACTIVATE_TP)
					)
				{
					return true;
				}
				Stop = false;
			}break;
			case IMTOrder::EnOrderType::OP_BUY_STOP:
			case IMTOrder::EnOrderType::OP_SELL_STOP:
			case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
			case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
			{
				Stop = true;
			}break;
			}

			bool Atuo = Stop;

			if (request->Action() >= IMTRequest::EnTradeActions::TA_CLIENT_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_CLIENT_LAST)
			{
				switch (IMTRequest::EnTradeActions(request->Action()))
				{
				case IMTRequest::EnTradeActions::TA_PRICE:
				case IMTRequest::EnTradeActions::TA_REQUEST:
				case IMTRequest::EnTradeActions::TA_INSTANT:
				case IMTRequest::EnTradeActions::TA_MARKET:
				case IMTRequest::EnTradeActions::TA_EXCHANGE:
				case IMTRequest::EnTradeActions::TA_PENDING:
				case IMTRequest::EnTradeActions::TA_SLTP:
				case IMTRequest::EnTradeActions::TA_MODIFY:
				case IMTRequest::EnTradeActions::TA_REMOVE:
				case IMTRequest::EnTradeActions::TA_TRANSFER:
				case IMTRequest::EnTradeActions::TA_CLOSE_BY:
				{

				}break;
				}///switch
			}
			else if (request->Action() >= IMTRequest::EnTradeActions::TA_SERVER_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_SERVER_LAST)
			{
				switch (IMTRequest::EnTradeActions(request->Action()))
				{
				case IMTRequest::EnTradeActions::TA_ACTIVATE:
				case IMTRequest::EnTradeActions::TA_ACTIVATE_STOPLIMIT:
				case IMTRequest::EnTradeActions::TA_STOPOUT_ORDER:
				case IMTRequest::EnTradeActions::TA_STOPOUT_POSITION:
				case IMTRequest::EnTradeActions::TA_EXPIRATION:
				{

				}break;
				case IMTRequest::EnTradeActions::TA_ACTIVATE_SL:
				case IMTRequest::EnTradeActions::TA_ACTIVATE_TP:
				{
					auto flag = request->Flags();
					if (flag & IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET ||
						flag & IMTRequest::EnTradeActionFlags::TA_FLAG_CLOSE)
					{

					}
				}break;
				}///switch
			}
			else if (request->Action() >= IMTRequest::EnTradeActions::TA_DEALER_FIRST && request->Action() <= IMTRequest::EnTradeActions::TA_DEALER_LAST)
			{
				switch (IMTRequest::EnTradeActions(request->Action()))
				{
				case IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_PENDING:
				case IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_MODIFY:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_REMOVE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_ACTIVATE:
				case IMTRequest::EnTradeActions::TA_DEALER_BALANCE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_SLIMIT:
				case IMTRequest::EnTradeActions::TA_DEALER_CLOSE_BY:
				{

				}break;
				}///switch
			}
			return Atuo;
		}
		static bool GatewayProcessAuto(const shared::RequestField& reqField)
		{
			if (reqField.ResultOrder <= 0)
			{
				return true;
			}
			bool Stop = true;
			switch (reqField.Type)
			{
			case IMTOrder::EnOrderType::OP_BUY:
			case IMTOrder::EnOrderType::OP_SELL:
			case IMTOrder::EnOrderType::OP_BUY_LIMIT:
			case IMTOrder::EnOrderType::OP_SELL_LIMIT:
			case IMTOrder::EnOrderType::OP_CLOSE_BY:
			{
				Stop = false;
			}break;
			case IMTOrder::EnOrderType::OP_BUY_STOP:
			case IMTOrder::EnOrderType::OP_SELL_STOP:
			case IMTOrder::EnOrderType::OP_BUY_STOP_LIMIT:
			case IMTOrder::EnOrderType::OP_SELL_STOP_LIMIT:
			{
				Stop = true;
			}break;
			}

			bool Atuo = Stop;

			if (reqField.Action >= IMTRequest::EnTradeActions::TA_CLIENT_FIRST && reqField.Action <= IMTRequest::EnTradeActions::TA_CLIENT_LAST)
			{
				switch (IMTRequest::EnTradeActions(reqField.Action))
				{
				case IMTRequest::EnTradeActions::TA_PRICE:
				case IMTRequest::EnTradeActions::TA_REQUEST:
				case IMTRequest::EnTradeActions::TA_INSTANT:
				case IMTRequest::EnTradeActions::TA_MARKET:
				case IMTRequest::EnTradeActions::TA_EXCHANGE:
				case IMTRequest::EnTradeActions::TA_PENDING:
				case IMTRequest::EnTradeActions::TA_SLTP:
				case IMTRequest::EnTradeActions::TA_MODIFY:
				case IMTRequest::EnTradeActions::TA_REMOVE:
				case IMTRequest::EnTradeActions::TA_TRANSFER:
				case IMTRequest::EnTradeActions::TA_CLOSE_BY:
				{

				}break;
				}///switch
			}
			else if (reqField.Action >= IMTRequest::EnTradeActions::TA_SERVER_FIRST && reqField.Action <= IMTRequest::EnTradeActions::TA_SERVER_LAST)
			{
				switch (IMTRequest::EnTradeActions(reqField.Action))
				{
				case IMTRequest::EnTradeActions::TA_ACTIVATE:
				case IMTRequest::EnTradeActions::TA_ACTIVATE_STOPLIMIT:
				case IMTRequest::EnTradeActions::TA_STOPOUT_ORDER:
				case IMTRequest::EnTradeActions::TA_STOPOUT_POSITION:
				case IMTRequest::EnTradeActions::TA_EXPIRATION:
				{

				}break;
				case IMTRequest::EnTradeActions::TA_ACTIVATE_SL:
				case IMTRequest::EnTradeActions::TA_ACTIVATE_TP:
				{
					if (reqField.Flags & IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET ||
						reqField.Flags & IMTRequest::EnTradeActionFlags::TA_FLAG_CLOSE)
					{

					}
				}break;
				}///switch
			}
			else if (reqField.Action >= IMTRequest::EnTradeActions::TA_DEALER_FIRST && reqField.Action <= IMTRequest::EnTradeActions::TA_DEALER_LAST)
			{
				switch (IMTRequest::EnTradeActions(reqField.Action))
				{
				case IMTRequest::EnTradeActions::TA_DEALER_POS_EXECUTE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_PENDING:
				case IMTRequest::EnTradeActions::TA_DEALER_POS_MODIFY:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_MODIFY:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_REMOVE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_ACTIVATE:
				case IMTRequest::EnTradeActions::TA_DEALER_BALANCE:
				case IMTRequest::EnTradeActions::TA_DEALER_ORD_SLIMIT:
				case IMTRequest::EnTradeActions::TA_DEALER_CLOSE_BY:
				{

				}break;
				}///switch
			}
			return Atuo;
		}





	};



}///namespace shared







///!@ 新生联创®（上海）
///!@ Wed Jun 10 12:36:41 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__9A81410D_6CC1_4688_81C0_78911670BB21__HEAD__
