#if !defined(AFX_SKSTU_UIProductNode_H__C9441_8F46A836_628EC1AED589836_A82C__HEAD__)
#define AFX_SKSTU_UIProductNode_H__C9441_8F46A836_628EC1AED589836_A82C__HEAD__

namespace client {
#if 0
	class IUIProductNode {
	protected:
		IUIProductNode(CContainerUI* pParentUI, const shared::MarketType& id, const shared::SymbolA& symbol) :
			m_pParentUI(pParentUI),
			m_Symbol(symbol),
			m_ProductID(id) {
		}
		~IUIProductNode() {}
	protected:
		virtual void InitUI() = 0;
	public:
		void ProductID(const shared::MarketType& id) { 
			m_ProductID = id;
			if (m_pLabel_ProductID) {
				m_pLabel_ProductID->SetAttribute(_T("text"), tostring(m_ProductID).c_str());
			}
		}
		void Symbol(const shared::SymbolA& symbol) { 
			m_Symbol = symbol; 
			if (m_pLabel_Symbol) {
				m_pLabel_Symbol->SetAttribute(_T("text"), sk::StringConvert::MBytesToWString(m_Symbol.symbol).c_str());
			}
		}
		shared::SymbolA Symbol() const { return m_Symbol; }
		shared::MarketType ProductID() const { return m_ProductID; }
		void QuoteSet(const shared::MarketData& quote) {
			if (m_pLabel_Bid && m_pLabel_Ask) {
				m_pLabel_Ask->SetAttribute(_T("text"), sk::Helper::FloatToStr(quote.PriceOffer[0], 5, false).c_str());
				m_pLabel_Bid->SetAttribute(_T("text"), sk::Helper::FloatToStr(quote.PriceBid[0], 5, false).c_str());
				m_pLabel_Spread->SetAttribute(_T("text"), tostring((int)abs(quote.PriceBid[0] * 100000 - quote.PriceOffer[0] * 100000)).c_str());
			}
		}
	protected:
		CContainerUI* m_pParentUI = nullptr;
		shared::SymbolA m_Symbol;
		shared::MarketType m_ProductID = 0;

		CLabelUI* m_pLabel_Bid = nullptr;
		CLabelUI* m_pLabel_Ask = nullptr;
		CLabelUI* m_pLabel_Spread = nullptr;
		CLabelUI* m_pLabel_Symbol = nullptr;
		CLabelUI* m_pLabel_ProductID = nullptr;
	};

	class UIProductNodeSystem final : public IUIProductNode {
	public:
		UIProductNodeSystem(CContainerUI*, const shared::MarketType&, const shared::SymbolA&, const sk::container::set<shared::Product>&);
		~UIProductNodeSystem();
	private:
		void InitUI() override final;
	public:
		sk::container::set<shared::Product> m_ProductQ;

	};
#endif

}///namespace client




/// /*新生联创（上海）**/
/// /*2019年12月30日 15:11:20|217**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_UIProductNode_H__C9441_8F46A836_628EC1AED589836_A82C__TAIL__