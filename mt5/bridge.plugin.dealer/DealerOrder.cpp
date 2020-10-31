#include "stdafx.h"
#include "DealerOrder.h"

namespace dealer
{
	DealerOrder::DealerOrder(const shared::Serial& serial) :
		m_Serial(serial)
	{

	}

	DealerOrder::~DealerOrder()
	{

	}

	void DealerOrder::Search(const shared::HedgeKey& key, const UINT& reqid, const std::function<void(shared::TradeTaskRouteGateway&)>& search_cb)
	{
		m_DealerOrderQ.search(
			key,
			[&](const auto&, auto& routes)
			{
				routes.search(
					reqid,
					[&](const auto&, auto& route)
					{
						search_cb(route);
					});
			});
	}

	void DealerOrder::Push(const shared::TradeTaskRouteGateway& route)
	{
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.pushpush(
			route.LPRequest.Key,
			[&](auto& reqs)
			{
				reqs.push(route.MTRequest.ID, route);
			});
	}

	void DealerOrder::Packet(__out std::vector<shared::SystemOrder>& out) const
	{
		out.clear();
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.iterate(
			[&](const auto& key, auto& routes, auto& itbreak)
			{
				shared::SystemOrder order;
				routes.iterate(
					[&](const auto&, auto& route, auto&)
					{
						if (order.Key.SystemPID <= 0)
						{
							order = route.LPRequest;
						}
						else
						{
							order.MTOrderQty += route.LPRequest.MTOrderQty;
						}
					});
				if (order.Key.SystemPID > 0) { out.emplace_back(order); }
			});
	}

	bool DealerOrder::Response(__in const shared::SystemOrder& in, __out std::vector<shared::ConfirmField>& out, __out std::vector<shared::TradeTaskRouteGateway>& out2)
	{
		out.clear();
		auto& SystemSetup = Dealer::SetupGet()->BridgeSystemSetup();
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.search_clear(
			in.Key,
			[&](auto& routes, auto& itclear)
			{
				switch (in.MTRetcode)
				{
				case shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_DONE:
				{
					routes.iterate(
						[&](const auto& reqid, auto& route)
						{
							route.MTConfirm.ID = reqid;
							route.MTConfirm.Retcode = MT_RET_REQUEST_DONE;
							route.MTConfirm.Volume = route.MTRequest.Volume;
							route.LPRequest.OrderID = in.OrderID;
							sk::SafeCopyW(route.MTConfirm.PositionExternalID, shared::SystemIDType::SystemOrderIDToWStr(in.OrderID).c_str(), _countof(route.MTConfirm.PositionExternalID));

							Dealer::ConfirmPriceProcess(in, route);
							out.emplace_back(route.MTConfirm);
						});
					itclear = true;
				}break;
				case shared::SystemOrder::EnMTRetType::EN_MTRET_TYPE_CANCEL:
				{
					UINT64 TradeTotal = in.MTTradeQty;
					if (TradeTotal <= 0)
					{
						routes.iterate(
							[&](const auto& reqid, auto& route)
							{
								route.MTConfirm.ID = reqid;
								route.LPRequest.OrderID = in.OrderID;
								sk::SafeCopyW(route.MTConfirm.PositionExternalID, shared::SystemIDType::SystemOrderIDToWStr(in.OrderID).c_str(), _countof(route.MTConfirm.PositionExternalID));
								route.MTConfirm.Retcode = MT_RET_REQUEST_REJECT;
								route.MTConfirm.Volume = route.MTRequest.Volume;
								route.MTConfirm.Price = route.MTRequest.PriceOrder;
								out.emplace_back(route.MTConfirm);
							});
						itclear = true;
					}
					else
					{
						routes.iterate_clear(
							[&](const auto& reqid, auto& route, auto& itbreak, auto& itclear)
							{
								route.TimeCreate = sk::Helper::TickCountGet<std::chrono::milliseconds>();
								route.MTConfirm.ID = reqid;						
								sk::SafeCopyW(route.MTConfirm.PositionExternalID, shared::SystemIDType::SystemOrderIDToWStr(in.OrderID).c_str(), _countof(route.MTConfirm.PositionExternalID));
								route.LPRequest.OrderID = in.OrderID;
								//!@ 如遇继续请求，需指定交易所
								route.LPRequest.Key.ExchangeID = shared::SystemIDType::SystemOrderIDMadeUn(route.LPRequest.OrderID);
								
								Dealer::ConfirmPriceProcess(in, route);
								if (TradeTotal >= route.MTRequest.Volume)
								{
									route.MTConfirm.Volume = route.MTRequest.Volume;
									route.MTConfirm.Retcode = MT_RET_REQUEST_DONE;
									TradeTotal -= route.MTRequest.Volume;
									itclear = true;
									out.emplace_back(route.MTConfirm);
								}
								else if (TradeTotal > 0)
								{
									route.MTConfirm.Volume = TradeTotal;
									route.MTRequest.Volume -= TradeTotal;
									TradeTotal = 0;
									route.MTConfirm.Retcode = MT_RET_REQUEST_DONE;
									Dealer::SharedGet()->m_RerequestCacheQ.push(route.MTRequest.ID, route);
									out.emplace_back(route.MTConfirm);
									if (SystemSetup.DealerContinueToRequestSwitch == shared::BridgeSystemSetup::EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_ENABLE)
									{
										route.MTConfirm.Retcode = MT_RET_REQUEST_REJECT;
										out2.emplace_back(route);
									}
								}
								else
								{
									route.MTConfirm.Volume = route.MTRequest.Volume;
									route.MTConfirm.Retcode = MT_RET_REQUEST_REJECT;
									if (SystemSetup.DealerContinueToRequestSwitch == shared::BridgeSystemSetup::EnDealerContinueToRequestSwitch::EN_DEALER_CONTINUE_TO_REQUEST_ENABLE)
									{
										out2.emplace_back(route);
									}
									else
									{
										out.emplace_back(route.MTConfirm);
									}
								}
							});

						itclear = true;
					}
				}break;
				default:
				{
					routes.iterate(
						[&](const auto& reqid, auto& route)
						{
							route.MTConfirm.ID = reqid;
							if (in.OrderID > 0)
							{
								route.LPRequest.OrderID = in.OrderID;
								sk::SafeCopyW(route.MTConfirm.PositionExternalID, \
									shared::SystemIDType::SystemOrderIDToWStr(in.OrderID).c_str(), \
									_countof(route.MTConfirm.PositionExternalID));
							}
							route.MTConfirm.Retcode = MT_RET_REQUEST_REJECT;
							route.MTConfirm.Volume = route.MTRequest.Volume;
							route.MTConfirm.Price = route.MTRequest.PriceOrder;
							out.emplace_back(route.MTConfirm);
						});
					itclear = true;
				}break;
				}
			});
		return m_DealerOrderQ.empty();
	}





































	std::vector<shared::TradeTaskRouteGateway> DealerOrder::Pop(const shared::HedgeKey& key)
	{
		std::vector<shared::TradeTaskRouteGateway> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.pop(key, [&](const auto&, auto& routes) {result = routes.Vector(); });
		return result;
	}
	void DealerOrder::Clear()
	{
		m_DealerOrderQ.clear();
	}
	void DealerOrder::ClearAt(const shared::HedgeKey& key)
	{
		m_DealerOrderQ.pop(key, [&](const auto&, auto&) {});
	}
	size_t DealerOrder::Count() const
	{
		return m_DealerOrderQ.size();
	}
	size_t DealerOrder::CountAt(const shared::HedgeKey& key) const
	{
		size_t result = 0;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.iterate(
			[&](const auto& hkey, auto& routes, auto& itbreak)
			{
				if (hkey == key)
				{
					result = routes.size();
					itbreak = true;
				}
			});
		return result;
	}
	size_t DealerOrder::Total() const
	{
		size_t result = 0;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.iterate(
			[&](const auto& hkey, auto& routes, auto& itbreak)
			{
				result += routes.size();
			});
		return result;
	}
	void DealerOrder::Timeout(const std::function<bool(const shared::TradeTaskRouteGateway&)>& timeout_cb)
	{
		const auto check = sk::Helper::TickCountGet<std::chrono::milliseconds>();
		const auto timeout = (time_t)Dealer::SetupGet()->BridgeSystemSetup().TimeoutDealerOrder;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.iterate(
			[&](const auto& serial, const auto& routes, auto&)
			{
				routes.iterate(
					[&](const auto&, const shared::TradeTaskRouteGateway& route, auto&)
					{
						if (check - route.TimeCreate >= timeout)
						{
							if (timeout_cb(route))
							{
							}
							else
							{

							}
						}
					});
			});
	}
	std::vector<shared::ConfirmField> DealerOrder::Finish(__in const EnMTAPIRetcode& mtretcode)
	{
		std::vector<shared::ConfirmField> result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_DealerOrderQ.iterate(
			[&](const auto& serial, auto& routes)
			{
				switch (mtretcode)
				{
				case EnMTAPIRetcode::MT_RET_REQUEST_TIMEOUT:
					[[fallthrough]];
				case EnMTAPIRetcode::MT_RET_REQUEST_DONE:
					[[fallthrough]];
				case EnMTAPIRetcode::MT_RET_REQUEST_REJECT:
				{
					routes.iterate(
						[&](const auto& reqid, shared::TradeTaskRouteGateway& route)
						{
							memset(&route.MTConfirm, 0x00, shared::LENCONFIRMFIELD);
							route.MTConfirm.ID = reqid;
							route.MTConfirm.Price = route.MTRequest.PriceOrder;
							route.MTConfirm.Volume = route.MTRequest.Volume;
							route.MTConfirm.Retcode = mtretcode;
							result.emplace_back(route.MTConfirm);
						});
				}break;
				default:
				{
					routes.iterate(
						[&](const auto& reqid, shared::TradeTaskRouteGateway& route)
						{
							memset(&route.MTConfirm, 0x00, shared::LENCONFIRMFIELD);
							route.MTConfirm.ID = reqid;
							route.MTConfirm.Price = route.MTRequest.PriceOrder;
							route.MTConfirm.Volume = route.MTRequest.Volume;
							route.MTConfirm.Retcode = MT_RET_REQUEST_ERROR;
							result.emplace_back(route.MTConfirm);
						});
				}break;
				}///switch
			});
		return result;
	}



}///namespace gateway