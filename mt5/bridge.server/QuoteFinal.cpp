#include "stdafx.h"
#include "QuoteFinal.h"

namespace proxy {

	QuoteFinal::QuoteFinal() {
		m_mutex = std::make_shared<std::mutex>();
		Init();
	}

	QuoteFinal::~QuoteFinal() {
		UnInit();
	}

	void QuoteFinal::Init() {
	}

	void QuoteFinal::UnInit() {
	}

	void QuoteFinal::Pop(const shared::SystemID& lpID) {
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_QuoteQ.iterate(
			[&](const auto&, auto& quotes)
			{
				quotes.PriceDel(lpID);
			});
	}
	void QuoteFinal::Push(const shared::marketdata::MarketData& md) {
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_QuoteQ.pushpush(shared::SystemIDType::SystemProductType(md.ID), [&](auto& obj) {obj.PriceAdd(md); });
	}
	shared::marketdata::FinalQuote QuoteFinal::Get(const shared::SystemID& sysProdID) {
		shared::marketdata::FinalQuote result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_QuoteQ.search(
			shared::SystemIDType::SystemProductType(sysProdID),
			[&](const auto&, auto& quotes)
			{
				result.Bid = quotes.Bid.Max();//!@ 银行买入 选价高
				result.Ask = quotes.Ask.Min();//@ 银行卖出 选价低
			});
		return result;
	}

	shared::marketdata::FinalPrice QuoteFinal::Get(
		const shared::SystemID& sysProdID,
		const shared::tradeorder::EnOrdSide& Side)
	{
		shared::marketdata::FinalPrice result;
		std::lock_guard<std::mutex> lock(*m_mutex);
		auto lpID = shared::SystemIDType::ExchangeType(sysProdID);
		if (lpID > 0) 
		{//! 指定LP
			m_QuoteQ.search(
				shared::SystemIDType::SystemProductType(sysProdID),
				[&](const auto&, auto& quotes)
				{
					switch (Side) {
					case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY: 
					{
						result = quotes.Ask.Assign(lpID);
					}break;
					case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL: 
					{
						result = quotes.Bid.Assign(lpID);
					}break;
					}///switch
				});
			return result;
		}

		m_QuoteQ.search(
			sysProdID,
			[&](const auto&, auto& quotes)
			{
				switch (Side) {
				case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_BUY: 
				{
					result = quotes.Ask.Min();
				}break;
				case shared::tradeorder::EnOrdSide::EN_ORD_SIDE_SELL: 
				{
					result = quotes.Bid.Max();
				}break;
				}///switch
			});
		return result;
	}
}///namespace proxy