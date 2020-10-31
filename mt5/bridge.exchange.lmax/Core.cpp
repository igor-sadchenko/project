#include "stdafx.h"
#include "Core.h"

namespace lmax {
	Core::Core() {
		if (Init()) { sk::Helper::ErrorFatal(__FUNCTION__, nullptr, "%s", "Module initialize failed !"); }
	}
	Core::~Core() {
		UnInit();
	}

	int Core::Init() {
		m_pMarketData = new MarketData();
		m_pTradeOrder = new TradeOrder();
		return 0;
	}
	int Core::UnInit() {
		SK_DELETE_PTR(m_pMarketData);
		SK_DELETE_PTR(m_pTradeOrder);
		return 0;
	}


	int Core::Open() {
		if (m_IsOpen.load()) { return 0; }

		m_Timer.Open();
		m_Timer.TimerAppend(5000, this, OnTimer);

		if (Global::GetSetup()->EnableMarketData()) 
		{
			MarketDataGet()->Open();
		}
		if (Global::GetSetup()->EnableTradeOrder()) 
		{
			TradeOrderGet()->Open();
		}


		m_IsOpenT.store(true);
		m_Threads.emplace_back([this]() {ProcessT(); });

		m_IsOpen.store(true);
		return 0;
	}
	int Core::Close() {
		if (!m_IsOpen.load()) { return 0; }

		if (Global::GetSetup()->EnableMarketData()) {
			MarketDataGet()->Close();
		}
		if (Global::GetSetup()->EnableTradeOrder()) {
			TradeOrderGet()->Close();
		}

		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();

		m_Timer.Close();
		m_IsOpen.store(false);
		return 0;
	}

	void Core::ProcessT() {
		do {
			
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (!m_IsOpenT.load()) {
				break;
			}
		} while (1);
	}

	int Core::OnTimer(const sk::int64& interval, const sk::intptr& coreptr, const sk::int64& currtime)
	{
		auto pCore = reinterpret_cast<Core*>(coreptr);

		switch (interval)
		{
		case 5000:
		{
			if (!pCore->m_BridgeProcessName.empty())
			{
				if (!sk::Helper::GetProcessidFromName(pCore->m_BridgeProcessName.c_str()))
				{
					Global::Log(sk::EnLogType::EN_LOG_TYPE_WARN)->warn(
						"The main process({}) was detected to have exited.",
						sk::StringConvert::WStringToMBytes(pCore->m_BridgeProcessName));
					Global::SystemExit(1);
				}
			}
		}break;
		}///switch
		return 0;
	}


	void Core::TestOrder()
	{
		shared::tradeorder::ExchangeOrder order;
		//order.Symbol = "EUR/USD";
		order.VolumeOrder = 0.1;
		order.OrdType = shared::tradeorder::EnOrdType::EN_ORD_TYPE_MARKET;
		order.Side = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL;
		//order.PositionEffect = shared::tradeorder::EnPositionEffect::EN_POSITION_EFFECT_OPEN;
		order.TimeInForce = shared::tradeorder::EnOrdTimeInForce::EN_ORD_TIMEINFORCE_IOC;
		order.SystemProductID = 0x150A00000FA1;
		order.SystemOrderID = shared::SystemIDType::SystemOrderIDMade(0xA000000000);
		//order.CIOrdID = sk::Log::Format("{:X}", order.SystemOrderID).c_str();
		/*order.SystemOrderID = 0;*/
		TradeOrderGet()->TestOrderRequest(order);
	}
	void Core::TestOrderStatus()
	{
#if 0
		shared::tradeorder::ExchangeOrder order;
		//order.Symbol = "EUR/USD";
		order.SystemProductID = 0x150A00000FA1;
		order.SystemOrderID = strtoull("A005A575CAE76416", nullptr, 16);
		//order.CIOrdID = sk::Log::Format("{:X}", order.SystemOrderID).c_str();
		order.Side = shared::tradeorder::EnOrdSide::EN_ORDER_SIDE_SELL;
		TradeOrderGet()->FixOrderStatusRequest(order);
#endif

		shared::SystemOrder order;
		order.OrderID = shared::SystemIDType::StrToSystemOrderID("174F57BC5A90A");
		order.ExchangePID = 0x900A00000FA9;
		order.Key.OrdSide = shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL;
		TradeOrderGet()->FixOrderStatusRequest(order);

	}
	void Core::TestOrderRecord()
	{
		shared::tradeorder::TradeRecordRequest req;
		req.From = time(0) - 24 * 3600 * 6;
		req.To = time(0);
		TradeOrderGet()->FixTradeCaptureReportRequest(req);
	}
	void Core::TestTestRequest()
	{
		TradeOrderGet()->FixTestRequest();
	}
}///namespace lmax