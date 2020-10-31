#if !defined(AFX_SKSTU_Aggregation_H__5102D_BAA1D78E_4BB77AEECE25177_2A76__HEAD__)
#define AFX_SKSTU_Aggregation_H__5102D_BAA1D78E_4BB77AEECE25177_2A76__HEAD__

namespace proxy {
	class Prices final {
	public:
		bool Push(const shared::marketdata::FinalPrice& obj) {
			return m_Prices.push(obj);
		}
		void Pop() {
			m_Prices.clear();
		}
		void Pop(const shared::marketdata::FinalPrice& lpID) {
			m_Prices.pop(lpID);
		}
		shared::marketdata::FinalPrice Assign(const shared::SystemID& lpID) {
			shared::marketdata::FinalPrice result;
			m_Prices.iterate([&](auto& quote, auto& itbreak)
				{
					if (quote.Source == lpID) {
						result = quote;
						itbreak = true;
					}
				});
			return result;
		}
		shared::marketdata::FinalPrice Max() {
			shared::marketdata::FinalPrice result;
			m_Prices.iterate(
				[&](auto& quote) {
					if (result.Price <= 0) { result = quote; }
					else if (result.Price < quote.Price) { result = quote; }
				});
			return result;
		}
		shared::marketdata::FinalPrice Min() {
			shared::marketdata::FinalPrice result;
			m_Prices.iterate([&](auto& quote) {
				if (result.Price <= 0) { result = quote; }
				else if (result.Price > quote.Price) { result = quote; }
				});
			return result;
		}
	private:
		sk::container::set<shared::marketdata::FinalPrice> m_Prices;
	};

	class Quotes final {
	public:
		Quotes() : m_mutex(std::make_shared<std::mutex>()) {}
	public:
		void PriceAdd(const shared::marketdata::MarketData& md) {
			std::lock_guard<std::mutex> lock(*m_mutex);
			Ask.Push(shared::marketdata::FinalPrice(md.PriceOffer[0], shared::SystemIDType::ExchangeType(md.ID)));
			Bid.Push(shared::marketdata::FinalPrice(md.PriceBid[0], shared::SystemIDType::ExchangeType(md.ID)));
		}
		void PriceDel(const shared::SystemID& lpID) {
			std::lock_guard<std::mutex> lock(*m_mutex);
			Bid.Pop(shared::marketdata::FinalPrice(0, shared::SystemIDType::ExchangeType(lpID)));
			Ask.Pop(shared::marketdata::FinalPrice(0, shared::SystemIDType::ExchangeType(lpID)));
		}
	public:
		Prices Bid;
		Prices Ask;
	private:
		std::shared_ptr<std::mutex> m_mutex;
	};

	class QuoteFinal final {
	public:
		QuoteFinal();
		~QuoteFinal();
	private:
		void Init();
		void UnInit();
	public:
		/*下线指定交易所的报价*/
		void Pop(const shared::SystemID&/*LPID*/);
		void Push(const shared::marketdata::MarketData&);
		shared::marketdata::FinalQuote Get(const shared::SystemID& sysProdID);

		//! 如果sysProdID 包含 ExchangeID 则为指定LP的定单
		shared::marketdata::FinalPrice Get(const shared::SystemID& sysProdID, const shared::tradeorder::EnOrdSide& Side);
#if 0
		//! extend Get if lp offline sel next lp...
		shared::FinalPrice Get(
			const shared::MarketType& sysProdID, 
			const shared::EnFixOrdSide& Side, 
			const std::vector<shared::FinalPrice>& FinalPrices,/*已经找到过但是未在线的LP记录,该缓冲由调用方定义*/
			const std::function<void(const shared::FinalPrice&/*这一次查询到的最优价*/)>&);
#endif
	private:
		std::shared_ptr<std::mutex> m_mutex;
		sk::container::map<shared::SystemID/*系统产品ID/报价产品ID*/, Quotes> m_QuoteQ;
	};





}///namespace proxy





/// /*新生联创（上海）**/
/// /*2020年01月9日 15:31:51|021**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_Aggregation_H__5102D_BAA1D78E_4BB77AEECE25177_2A76__TAIL__
