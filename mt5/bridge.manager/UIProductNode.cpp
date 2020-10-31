#include "stdafx.h"
#include "UIProductNode.h"

namespace client {
#if 0
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UIProductNodeSystem::UIProductNodeSystem(CContainerUI* pParentUI, const shared::MarketType& id, const shared::SymbolA& symbol, const sk::container::set<shared::Product>& products) :
		m_ProductQ(products),
		IUIProductNode(pParentUI, id, symbol) {
		InitUI();
	}
	UIProductNodeSystem::~UIProductNodeSystem() {
	}
	void UIProductNodeSystem::InitUI() {
		CDialogBuilder builder;
		auto pSysProductNode = (CListContainerElementUI*)builder.Create(_T("UIProductsTreeNode.xml"));
		pSysProductNode->SetTag(m_ProductID);
		m_pParentUI->Add(pSysProductNode);
		m_pLabel_Bid = pSysProductNode->GetSubCtrl<CLabelUI>(_T("DEF_E2C2A58989612AE0F43D81B979A3BE98"));
		m_pLabel_Ask = pSysProductNode->GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"));
		m_pLabel_Spread = pSysProductNode->GetSubCtrl<CLabelUI>(_T("DEF_5599F20AF76F15A37DAFCAEE5C402466"));
		m_pLabel_Symbol = pSysProductNode->GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"));
		m_pLabel_ProductID = pSysProductNode->GetSubCtrl<CLabelUI>(_T("DEF_526AFEE9F7A61493A0323DE201893749"));

		Symbol(m_Symbol);
		ProductID(m_ProductID);
	}
#endif
}///namespace client