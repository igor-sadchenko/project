#include "stdafx.h"
#include "OrderDispatch.h"

namespace proxy {

	OrderDispatch::OrderDispatch(const class System* pSystem) :
		m_pSystem(pSystem)
	{

	}
	OrderDispatch::~OrderDispatch()
	{

	}
	void OrderDispatch::Ack(__in const shared::TradeTaskRouteBridge& route)
	{
		m_ReplyQ.push(route);
	}
	bool OrderDispatch::Res(__in const std::vector<shared::SystemOrder>& orders)
	{
		for (const auto& order : orders)
		{
			m_OrderMappingQ.search_clear(
				order.SystemSerial,
				[&](shared::TradeTaskRouteBridge& route, auto& itclear)
				{
					route.OrderRequest = order;
					if (route.OrderRequest.LPTradeQty > 0)
					{
						shared::SystemIDType::LPVolumeToMTVolume(
							route.ExchangeProductCon.ContractSize,
							route.GatewayProductCon.ContractSize,
							route.OrderRequest.LPTradeQty,
							route.OrderRequest.MTTradeQty);
					}
					Ack(route);
					itclear = true;
				});
		}
		return true;
	}

	bool OrderDispatch::Req(__in const std::vector<shared::SystemOrder>& orders)
	{
		std::vector<shared::TradeTaskRouteBridge> routes;
		for (const auto& order : orders)
			MadeOrder(order, routes);

		for (const auto& route : routes)
		{
			if (SKAPIRES_FAILED(route.OrderRequest.SKRetcode))
			{
				Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->info("{}", SKAPIRES_DESC(route.OrderRequest.SKRetcode));
				Ack(route);
			}
			else
			{
				m_OrderMappingQ.push(route.OrderRequest.SystemSerial, route);
				m_RequestQ.push(route);
			}
		}
		return true;
	}


	SKAPIRES OrderDispatch::VerifyOrder(__in const shared::SystemOrder& in) const
	{
		SKAPIRES result = SKAPIRES::PROXY_RET_SUCCESS;

		if (!in.Verify())
		{
			result = SKAPIRES::PROXY_RET_ERROR_ORDER;
		}
		//!@ 系统产品ID
		if (SKAPIRES_SUCCESS(result) && in.Key.SystemPID <= 0)
		{
			result = SKAPIRES::PROXY_RET_ERROR_INVALID_SYSTEM_PRODUCT_ID;
		}
		//!@ 多空类型
		if (SKAPIRES_SUCCESS(result) &&
			in.Key.OrdSide != shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY && \
			in.Key.OrdSide != shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL)
		{
			result = SKAPIRES::PROXY_RET_ERROR_INVALID_ORDER_SIDE;
		}
		//!@ 终端产品ID
		if (SKAPIRES_SUCCESS(result) &&
			in.GatewayPID <= 0)
		{
			result = SKAPIRES::PROXY_RET_ERROR_INVALID_GATEWAY_PRODUCT_ID;
		}
		//!@ 请求数量
		if (SKAPIRES_SUCCESS(result) &&
			in.MTOrderQty <= 0)
		{
			result = SKAPIRES::PROXY_RET_ERROR_INVALID_ORDER_VOLUME;
		}
		//!@ 终端ID
		if (SKAPIRES_SUCCESS(result) &&
			in.ClientID <= 0)
		{
			result = SKAPIRES::PROXY_RET_ERROR_INVALID_CLIENT_ID;
		}
		return result;
	}

	void OrderDispatch::MadeOrder(__in const shared::SystemOrder& in, __out std::vector<shared::TradeTaskRouteBridge>& out) const
	{
		shared::TradeTaskRouteBridge Route, *pRouteAutoAdjust(nullptr);	
		Route.OrderRequest = in;
		Route.OrderRequest.SystemSerial = Global::SharedGet()->Serial();
		Route.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		Route.OrderRequest.SKRetcode = VerifyOrder(Route.OrderRequest);

		//!说明 : 
		//!注意 : 如果MT订单无价格就填充价格 确认订单时使用
		//!日期 : Tue Sep 29 00:33:12 UTC+0800 2020
		if (Route.OrderRequest.MTOrderPx <= 0)
		{
			Global::SharedGet()->m_MarketDataFinalQ.search(
				Route.OrderRequest.Key.SystemPID,
				[&](const auto&, shared::marketdata::MarketData& md)
				{
					if (Route.OrderRequest.Key.OrdSide == shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY)
					{
						Route.OrderRequest.MTOrderPx = md.PriceOfferFinal;
					}
					else
					{
						Route.OrderRequest.MTOrderPx = md.PriceBidFinal;
					}
				});
		}

		//!说明 : 检查桥系统的准备工作
		//!注意 : 
		//!日期 : Wed Sep 30 07:28:15 UTC+0800 2020
		if (!m_pSystem->Ready())
		{
			Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_PROCESS_READY;
		}

		auto pSymbolCon = m_pSystem->SymbolConGet();
		auto pQuoteFinal = m_pSystem->FinalQuoteGet();
		if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode) \
			&& (!pSymbolCon || !pQuoteFinal))
		{
			Route.OrderRequest.SKRetcode = SKAPIRES::SYSTEM_ERROR_POINTER_NULL;
		}

		if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
		{
			//!@ 网关产品配置
			auto pGatewayProdcon = pSymbolCon->GetMTGatewayProduct(in.GatewayPID);
			if (!pGatewayProdcon)
			{
				Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_PRODUCTCON_NOTFOUND_GATEWAY;
			}
			else
			{
				Route.GatewayProductCon = *pGatewayProdcon;

				//!@ 系统产品配置
				auto pSystemProdcon = pSymbolCon->GetSystemProduct(shared::SystemIDType::SystemProductType(Route.GatewayProductCon.ID));
				if (!pSystemProdcon)
				{
					Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_PRODUCTCON_NOTFOUND_SYSTEM;
				}
				else
				{
					Route.SystemProductCon = *pSystemProdcon;
				}
			}
		}

		if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
		{
			//!@ 新开仓
			if (Route.OrderRequest.Key.ExchangeID <= 0)
			{
				//!@ 聚合报价->指定交易所
				shared::marketdata::FinalPrice FinalPrice;
				FinalPrice = pQuoteFinal->Get(Route.SystemProductCon.ID | Route.GatewayProductCon.AssignOrderLP/*暂未启用 : 0*/, Route.OrderRequest.Key.OrdSide);
				if (FinalPrice.Source <= 0)
				{
					//!@ 默认选择第一个在线的交易所
					Global::GetSetup()->m_LPProcesesMapQ.begin(
						[&](const auto& exchangeID, auto& process)
						{
							if (process->Status())
							{
								FinalPrice.Source = shared::SystemIDType::ExchangeType(exchangeID);
							}
						});
				}

				if (FinalPrice.Source <= 0)
				{
					Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_EXCHANGE_NOTFOUND;
				}

				//!@ 重新指定定单请求价格
				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					if (FinalPrice.Price > 0)
						Route.OrderRequest.MTOrderPx = FinalPrice.Price;
				}

				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					auto pExchangeProdcoon = pSymbolCon->GetLPProduct(Route.SystemProductCon.ID | shared::SystemIDType::ExchangeType(FinalPrice.Source));
					if (!pExchangeProdcoon)
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_PRODUCTCON_NOTFOUND_EXCHANGE;
					}
					else
					{
						Route.ExchangeProductCon = *pExchangeProdcoon;
						Route.OrderRequest.ExchangePID = shared::SystemIDType::ExchangeType(FinalPrice.Source) | Route.ExchangeProductCon.ID;
					}
				}

				//!@ 转换抛单量
				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					if (!shared::SystemIDType::MTVolumeToLPVolume(
						Route.ExchangeProductCon.ContractSize,
						Route.GatewayProductCon.ContractSize,
						Route.ExchangeProductCon.MinOrderSize,
						Route.OrderRequest.MTOrderQty,
						Route.OrderRequest.LPOrderQty))
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_VOLUME;
					}
				}

				//!@ 验证抛单量
				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					//!@ 应该大于或等于最小成交量
					if (Route.ExchangeProductCon.MinOrderSize > 0 && (Route.ExchangeProductCon.MinOrderSize > Route.OrderRequest.LPOrderQty))
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_VOLUME_MINLIMIT;
					}
					//!@ 应该小于或等于最大成交量
					else if (Route.ExchangeProductCon.MaxOrderSize > 0 && Route.ExchangeProductCon.MaxOrderSize < Route.OrderRequest.LPOrderQty)
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_VOLUME_MAXLIMIT;
					}
				}
			}
			//!@ 平仓 | 减仓 | 继续请求
			else
			{
				//! 交易所产品配置
				auto pExchangeProdcon = pSymbolCon->GetLPProduct(Route.SystemProductCon.ID | shared::SystemIDType::ExchangeType(Route.OrderRequest.Key.ExchangeID));
				if (!pExchangeProdcon)
				{
					Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_PRODUCTCON_NOTFOUND_EXCHANGE;
				}
				else
				{
					Route.ExchangeProductCon = *pExchangeProdcon;
					Route.OrderRequest.ExchangePID = shared::SystemIDType::ExchangeType(Route.OrderRequest.Key.ExchangeID) | Route.ExchangeProductCon.ID;
				}

				//!@ 平仓的交易所ID
				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					if (Route.OrderRequest.ExchangePID <= 0)
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_ORDER_ID;
					}
				}

				//!@ 转换抛单量
				if (SKAPIRES_SUCCESS(Route.OrderRequest.SKRetcode))
				{
					if (!shared::SystemIDType::MTVolumeToLPVolume(
						Route.ExchangeProductCon.ContractSize,
						Route.GatewayProductCon.ContractSize,
						Route.ExchangeProductCon.MinOrderSize,
						Route.OrderRequest.MTOrderQty,
						Route.OrderRequest.LPOrderQty))
					{
						Route.OrderRequest.SKRetcode = SKAPIRES::PROXY_RET_ERROR_VOLUME;
					}
				}

				//!@ 平仓|减仓 自动调仓
				if (Global::GetSetup()->SystemSetupGet().AutoAdjustPositionSwitch == \
					shared::BridgeSystemSetup::EnAutoAdjustPositionSwitch::EN_AUTO_ADJUST_POSITION_SWITCH_ENABLE)
				{
					//!@ 选择交易所
					shared::marketdata::FinalPrice FinalPrice = \
						pQuoteFinal->Get(Route.SystemProductCon.ID | 0, Route.OrderRequest.Key.OrdSide);
					if (FinalPrice.Source && \
						shared::SystemIDType::ExchangeType(FinalPrice.Source) != \
						shared::SystemIDType::ExchangeType(Route.OrderRequest.ExchangePID))
					{
						decltype(Route) RouteAutoAdjust = Route;
						RouteAutoAdjust.OrderRequest.ExchangePID = shared::SystemIDType::ExchangeType(FinalPrice.Source);
						//! 交易所产品配置
						auto pAdjustExchangeProdcon = pSymbolCon->GetLPProduct(Route.SystemProductCon.ID | RouteAutoAdjust.OrderRequest.ExchangePID);
						if (pAdjustExchangeProdcon)
						{
							RouteAutoAdjust.ExchangeProductCon = *pAdjustExchangeProdcon;
							RouteAutoAdjust.OrderRequest.ExchangePID |= Route.ExchangeProductCon.ID;
							RouteAutoAdjust.OrderRequest.SystemSerial = Global::SharedGet()->Serial();
							RouteAutoAdjust.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();
							RouteAutoAdjust.OrderRequest.OrdRequestType |= \
								(UINT64)shared::SystemOrder::EnOrdRequestType::EN_ORD_REQUEST_TYPE_POSITION_ADJUST;
							//!@ 自动调仓 市价定单
							RouteAutoAdjust.OrderRequest.OrdType = shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET;
							//!@ 自动调仓 IOC固定模式
							RouteAutoAdjust.OrderRequest.OrdTimeInForce = shared::tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC;
							//!@ 自动调仓 方向取反
							shared::tradeorder::Misc::OrdSideNot(RouteAutoAdjust.OrderRequest.Key.OrdSide);
							pRouteAutoAdjust = &RouteAutoAdjust;
							//Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("自动调仓触发(GatewayPID({:X}),LPOrderQty({}))", RouteAutoAdjust.OrderRequest.GatewayPID, RouteAutoAdjust.OrderRequest.LPOrderQty);
						}




					}
				}



			}
		}

		out.emplace_back(Route);
		if (pRouteAutoAdjust)
			out.emplace_back(*pRouteAutoAdjust);
	}
}///namespace proxy