#include "stdafx.h"
#include "Feeder.h"

namespace feeder {

	MTAPIRES Feeder::HookTick(const int feeder, MTTick& tick)
	{
		auto& SystemSetup = Feeder::SetupGet()->BridgeSystemSetup();

		if (SystemSetup.FeederSpreadMode == shared::BridgeSystemSetup::EnFeederSpreadMode::EN_FEEDER_SPREAD_MODE_PLATFORM)
		{
			shared::Instrument inst = sk::StringConvert::WStringToMBytes(tick.symbol).c_str();
			Feeder::SharedGet()->m_MTSymbolProdconQ.search(
				inst,
				[&](const auto& symbol, shared::MTFeederProduct& prod)
				{
					if (prod.EnableQuote)
					{
						Feeder::SharedGet()->m_MTSymbolQ.search(
							tick.symbol,
							[&](const auto&, shared::ConSymbolField& symbolField)
							{
								auto totalOffset = prod.SpreadTotal * symbolField.Point;
								auto askSpread = prod.SpreadAsk * symbolField.Point;
								auto bidSpread = prod.SpreadBid * symbolField.Point;
								//! 买价
								tick.bid += (totalOffset + bidSpread);
								//! 卖价
								tick.ask += (totalOffset + askSpread);
							});
						if (prod.Symbol == "EURUSD")
						{
							Feeder::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("feeder({}),Symbol({}),Ask({}),Bid({}),Volume({})", feeder,prod.Symbol.symbol, tick.ask, tick.bid, tick.volume);
						}
					}
				});
		}

		return MT_RET_OK;
	}


	MTAPIRES Feeder::HookTickStat(const int, MTTickStat&)
	{

		return MT_RET_OK;
	}
	void Feeder::OnTick(LPCWSTR, const MTTickShort&)
	{

	}
	void Feeder::OnTickStat(const MTTickStat&)
	{

	}
	void Feeder::OnTick(const int feeder, const MTTick& tick)
	{
		auto symbol = sk::StringConvert::WStringToMBytes(tick.symbol);
		if (symbol == "EURUSD" ||
			symbol =="AUDCAD" ||
			symbol == "GBPUSD")
		{
			//Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("({})(feeder({}),tick(symbol({}),ask({}),bid({})))", __FUNCTION__,feeder, sk::StringConvert::WStringToMBytes(tick.symbol), tick.ask, tick.bid);
		}
	}
	void Feeder::OnTickStat(const int, const MTTickStat&)
	{

	}


	void Feeder::OnTimeUpdate(const IMTConTime* config)
	{
		if (!config) return;
		m_ServerTime.store(ServerApi()->TimeCurrent());
		m_ServerTimeDiff.store(m_ServerTime.load() - ::time(0));
		Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info(
			"系统时间更新: 时区[{}],时差[{}],时间[{}]",
			shared::Business::MTGMT(config->TimeZone()),
			m_ServerTimeDiff.load(),
			sk::time::Time::TimestampToTimeStr(m_ServerTime.load()));

		License(ServerApi()->TimeCurrent());
	}


}///namespace feeder