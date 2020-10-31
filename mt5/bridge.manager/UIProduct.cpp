#include "stdafx.h"
#include "UIProduct.h"

namespace client {
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IExchangeTreeNodeUI::SetID(const shared::SystemID& exchange_id)
	{
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_0270F9078DCD0BA2228FF73351465A0D"));
		if (pCtrl)
		{
			pCtrl->SetText(sk::Log::Format(_T("{:X}"), exchange_id).c_str());
		}
	}
	void IExchangeTreeNodeUI::SetExchange(const sk::stringa& exchange_name)
	{
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_7D2D332BCF41A3DB5A067E9299848AF1"));
		if (pCtrl)
		{
			pCtrl->SetText(sk::StringConvert::MBytesToTString(exchange_name).c_str());
		}
	}
	void IExchangeTreeNodeUI::SetModule(const sk::stringa& module_name)
	{
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_9E21158473D2AA536B269348425BFEC8"));
		if (pCtrl)
		{
			pCtrl->SetText(sk::StringConvert::MBytesToTString(module_name).c_str());
		}
	}
	void IExchangeTreeNodeUI::SetStatus(const sk::stringa& status)
	{
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_C8BEEF69753EE21B7C646D22C942B65B"));
		if (pCtrl)
		{
			pCtrl->SetText(sk::StringConvert::MBytesToTString(status).c_str());
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductTreeNodeUI::SetSymbol(const sk::string& symbol) {
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"));
		if (pCtrl) {
			pCtrl->SetText(symbol.c_str());
		}
	}

	void IProductTreeNodeUI::SetQuotes(const shared::marketdata::MarketData& md, const unsigned int& decimals) {
		//! Ask
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(md.PriceOfferFinal, decimals, false).c_str());
		}
		//! Bid
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_E2C2A58989612AE0F43D81B979A3BE98"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(md.PriceBidFinal, decimals, false).c_str());
		}
		//! Spread
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_5599F20AF76F15A37DAFCAEE5C402466"));
		if (pCtrl) {
			auto val = tostring(static_cast<unsigned int>((abs)(md.PriceOfferFinal * pow(10, decimals) - md.PriceBidFinal * pow(10, decimals))));
			pCtrl->SetText(val.c_str());
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductSystemEditUI::Set(
		const shared::SystemProduct& sysProd, 
		const sk::container::set<shared::LPProduct>& lpProds, 
		const sk::container::set<shared::MTFeederProduct>& mtFeedProds, 
		const sk::container::set<shared::MTGatewayProduct>& mtGatewayProds) 
	{
		SetCache(sk::packet((char*)&sysProd, shared::LENSYSTEMPRODUCT));
		GetSubCtrl<CEditUI>(_T("DEF_8199818406345AC3B8C7D7AF03360488"))->SetText(sk::Log::Format(_T("{:X}"), sysProd.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_E4311B543C917BD2BDFA2FF6D3657B0E"))->SetText(sk::StringConvert::MBytesToTString(sysProd.Symbol.symbol).c_str());

		sk::container::set<shared::SystemID> QuoteSources;
		shared::ProductCon::UnMadeSystemSetup(sysProd.Setup, QuoteSources);

		auto pCombo = GetSubCtrl<CComboUI>(_T("DEF_1D9F879E49082A7DF9DEA84ED6B4D72D"));
		if (pCombo) {
			CDialogBuilder builder;
			lpProds.iterate(
				[&](auto& lpProd, bool& itbreak)
				{
					auto pMDActionUI = builder.Create<IProductSystemEditUI>(_T("UIProductMDSystemAction.xml"));
					pCombo->Add(pMDActionUI);
					auto pExhange = pMDActionUI->GetSubCtrl<CLabelUI>(_T("DEF_2A4D7B3540BA2F4836402DE5BB690869"));
					auto pExchangeAssign = pMDActionUI->GetSubCtrl<CCheckBoxUI>(_T("DEF_A542CF3ECC4C7FB1D97F8D385084F699"));
					if (pExhange && pExchangeAssign)
					{
						pExhange->SetText(sk::Log::Format(_T("{:X}"), shared::SystemIDType::ExchangeType(lpProd.ID)).c_str());
						pExchangeAssign->SetCheck(false);

						if (sysProd.SetupEnable != 0)
						{
							if (QuoteSources.search(shared::SystemIDType::ExchangeType(lpProd.ID)))
							{
								pExchangeAssign->SetCheck(true);
							}				
						}
					}
				});

			auto pEdit = GetSubCtrl<CEditUI>(_T("DEF_614824636501ABD29D5166D168F72CBC"));
			if (pEdit)
				pEdit->SetText(sysProd.SetupEnable  ? _T("已指定") : _T("默认"));
		}
		
	}
	std::shared_ptr<shared::SystemProduct> IProductSystemEditUI::Get() {
		auto cache = GetCache();
		auto pCache = (shared::SystemProduct*)cache.data();
		auto result = std::make_shared<shared::SystemProduct>(*pCache);
		auto pComboQuoteSourceSet = GetSubCtrl<CComboUI>(_T("DEF_1D9F879E49082A7DF9DEA84ED6B4D72D"));
		if (pComboQuoteSourceSet)
		{
			sk::container::set<shared::SystemID> QuoteSources;
			for (int i = 0; i < pComboQuoteSourceSet->GetCount(); ++i)
			{
				auto pElem = (CListContainerElementUI*)pComboQuoteSourceSet->GetItemAt(i);
				if (pElem)
				{
					auto pExchange = pElem->GetSubCtrl<CLabelUI>(_T("DEF_2A4D7B3540BA2F4836402DE5BB690869"));
					auto pExchangeStatus = pElem->GetSubCtrl<CCheckBoxUI>(_T("DEF_A542CF3ECC4C7FB1D97F8D385084F699"));
					if (pExchange && pExchangeStatus && pExchangeStatus->GetCheck())
					{
						QuoteSources.push(_tcstoull(pExchange->GetText().GetData(), nullptr, 16));
					}
				}
			}

			if (QuoteSources.size() == pComboQuoteSourceSet->GetCount() || QuoteSources.empty())
			{
				result->SetupEnable = 0;
				memset(result->Setup, 0x00, _countof(result->Setup));
			}
			else
			{
				auto pak = shared::ProductCon::MadeSystemSetup(QuoteSources);
				if (!pak.empty())
				{
					result->SetupEnable = 1;
					sk::SafeCopyA(result->Setup, pak.data(), _countof(result->Setup));
				}
			}
		}

		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductExchangeEditUI::Set(const shared::LPProduct& lpProd) {
		SetCache(sk::packet((char*)&lpProd, shared::LENLPPRODUCT));
		//!@ ID
		GetSubCtrl<CEditUI>(_T("DEF_2B8769552A23837B2A80ABD17E6830A9"))->SetText(sk::Log::Format(_T("{:X}"), lpProd.ID).c_str());
		//!@ 符号
		GetSubCtrl<CEditUI>(_T("DEF_782FE71EADA80A6608D43FD6F5748E8E"))->SetText(sk::StringConvert::MBytesToTString(lpProd.Symbol.symbol).c_str());
		//!@ 本币
		GetSubCtrl<CEditUI>(_T("DEF_F013433B32A84094D59D8182D9E8BAA0"))->SetText(sk::StringConvert::MBytesToTString(lpProd.QuotedCCY.currency).c_str());
		//!@ 合约大小
		GetSubCtrl<CEditUI>(_T("DEF_F5DCB6944B77D5EF66CCA564C61441C8"))->SetText(sk::Log::Format(_T("{}"), lpProd.ContractSize).c_str());
		//!@ 最小报价单位
		GetSubCtrl<CEditUI>(_T("DEF_B75AAB7E5503C7B5718EE7CEB9711A0E"))->SetText(sk::Helper::FloatToStr(lpProd.TickSize, 6).c_str());
		//!@ 最小交易量
		GetSubCtrl<CEditUI>(_T("DEF_E724368463111A712EF0BC8A28EA0068"))->SetText(sk::Helper::FloatToStr(lpProd.MinOrderSize, 6).c_str());
		//!@ 最大交易量
		GetSubCtrl<CEditUI>(_T("DEF_A45A97E91EBCF634F3F22B1BA09AD978"))->SetText(sk::Helper::FloatToStr(lpProd.MaxOrderSize, 6).c_str());
		//!@ 交易量步长
		GetSubCtrl<CEditUI>(_T("DEF_BBD2423C9D525B438688CB19AC078D62"))->SetText(sk::Helper::FloatToStr(lpProd.StepOrderSize, 6).c_str());
	}
	std::shared_ptr<shared::LPProduct> IProductExchangeEditUI::Get() {
		auto cache = GetCache();
		auto pCache = (shared::LPProduct*)cache.data();

		auto result = std::make_shared<shared::LPProduct>(*pCache);
		//!@ 合约大小
		result->ContractSize = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_F5DCB6944B77D5EF66CCA564C61441C8"))->GetText().GetData());
		//!@ 最小报价单位
		result->TickSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_B75AAB7E5503C7B5718EE7CEB9711A0E"))->GetText().GetData());
		//!@ 最小交易量
		result->MinOrderSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_E724368463111A712EF0BC8A28EA0068"))->GetText().GetData());
		//!@ 最大交易量
		result->MaxOrderSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_A45A97E91EBCF634F3F22B1BA09AD978"))->GetText().GetData());
		//!@ 交易量步长
		result->StepOrderSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_BBD2423C9D525B438688CB19AC078D62"))->GetText().GetData());
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductMTGatewayEditUI::Set(const shared::MTGatewayProduct& gatewayProds, sk::container::set<shared::LPProduct>& lpProds) {
		SetCache(sk::packet((char*)&gatewayProds, shared::LENMTGATEWAYPRODUCT));
		GetSubCtrl<CEditUI>(_T("DEF_3E839BF7789BF0E6DFA24342111D0612"))->SetText(sk::Log::Format(_T("{:X}"), gatewayProds.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_B45EA3435AEEC80E041D2976445EF230"))->SetText(sk::StringConvert::MBytesToTString(gatewayProds.Symbol.symbol).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_8927CF51F002E0F94A944EF9763C6535"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.ContractSize).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.SpreadBid).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.SpreadAsk).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_DD0152AC9693302127869DFDB845EACD"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.SpreadTotal).c_str());
		GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->SetGroup(tostring(gatewayProds.ID).c_str());
		GetSubCtrl<COptionUI>(_T("DEF_11FD062045877D22EA88791731E5BD1E"))->SetGroup(tostring(gatewayProds.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_EC5FBD5A1075CB62DB95F20FCF3138EE"))->SetText(sk::Helper::FloatToStr(gatewayProds.Point, 6).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_964D82686DF3658C1061DE850C8EF781"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.Digits).c_str());
		if (gatewayProds.EnableTrade)
		{
			GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->Selected(true);
		}
		else
		{
			GetSubCtrl<COptionUI>(_T("DEF_11FD062045877D22EA88791731E5BD1E"))->Selected(true);
		}

#if 0//!@ 该功能暂时未启用
		if (GetSubCtrl<CEditUI>(_T("DEF_F63588E183AF8A06F66118F7C1ECE253")))
		{
			GetSubCtrl<CEditUI>(_T("DEF_F63588E183AF8A06F66118F7C1ECE253"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.AutoAdjustPositionSpreadRange).c_str());
		}
		auto pCombo = GetSubCtrl<CComboUI>(_T("DEF_B2738C788AA1D1A64BB8E79CD7F60892"));
		if (pCombo) {
			lpProds.iterate(
				[&](auto& lpProd)
				{
					auto elem = new CListLabelElementUI();
					pCombo->Add(elem);
					elem->SetTagUINT64(shared::SystemIDType::ExchangeType(lpProd.ID));
					elem->SetText(sk::Log::Format(_T("{:X}"), shared::SystemIDType::ExchangeType(lpProd.ID)).c_str());
				});
			for (int i = 0; i < pCombo->GetCount(); ++i)
			{
				if (pCombo->GetItemAt(i)->GetTagUINT64() == gatewayProds.AssignOrderLP)
				{
					pCombo->SelectItem(i);
					break;
				}
			}
		}
#endif
	}
	std::shared_ptr<shared::MTGatewayProduct> IProductMTGatewayEditUI::Get() {
		auto cache = GetCache();
		auto pCache = (shared::MTGatewayProduct*)cache.data();
		auto result = std::make_shared<shared::MTGatewayProduct>(*pCache);
		result->Symbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_B45EA3435AEEC80E041D2976445EF230"))->GetText().GetData()).c_str();
		result->SpreadBid = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9"))->GetText().GetData());
		result->SpreadAsk = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC"))->GetText().GetData());
		result->SpreadTotal = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_DD0152AC9693302127869DFDB845EACD"))->GetText().GetData());
		result->Digits = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_964D82686DF3658C1061DE850C8EF781"))->GetText().GetData());
		if (GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->IsSelected()) {
			result->EnableTrade = true;
		}
		else {
			result->EnableTrade = false;
		}
#if 1//!@ 该功能暂时未启用
		if (GetSubCtrl<CEditUI>(_T("DEF_F63588E183AF8A06F66118F7C1ECE253")))
		{
			result->AutoAdjustPositionSpreadRange = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_F63588E183AF8A06F66118F7C1ECE253"))->GetText().GetData());
		}
		auto pCombo = GetSubCtrl<CComboUI>(_T("DEF_B2738C788AA1D1A64BB8E79CD7F60892"));
		if (pCombo && pCombo->GetCount() > 0 && pCombo->GetCurSel() >= 0) {
			result->AssignOrderLP = pCombo->GetItemAt(pCombo->GetCurSel())->GetTagUINT64();
		}
#endif
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductMTFeederEditUI::Set(const shared::MTFeederProduct& feederProds) {
		SetCache(sk::packet((char*)&feederProds, shared::LENMTFEEDERPRODUCT));
		GetSubCtrl<COptionUI>(_T("DEF_ECACA9AE7F4420437A5DDBCD7CE73E00"))->SetGroup(tostring(feederProds.ID).c_str());
		GetSubCtrl<COptionUI>(_T("DEF_634D8ADBFC97567E581EBC9FB889EBFE"))->SetGroup(tostring(feederProds.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_849D3EF577DDDE724FD40994D408F150"))->SetText(sk::Log::Format(_T("{}"), feederProds.SpreadBid).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_B22B0B4471F195A1BB8FA4BB0557D150"))->SetText(sk::Log::Format(_T("{}"), feederProds.SpreadAsk).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_E6453731ED2FEF9B2A72F6F3E0D954AE"))->SetText(sk::Log::Format(_T("{}"), feederProds.SpreadTotal).c_str());
		if (feederProds.EnableQuote)
		{
			GetSubCtrl<COptionUI>(_T("DEF_ECACA9AE7F4420437A5DDBCD7CE73E00"))->Selected(true);
		}
		else
		{
			GetSubCtrl<COptionUI>(_T("DEF_634D8ADBFC97567E581EBC9FB889EBFE"))->Selected(true);
		}
	}
	std::shared_ptr<shared::MTFeederProduct> IProductMTFeederEditUI::Get() {
		auto cache = GetCache();
		auto pCache = (shared::MTFeederProduct*)cache.data();
		auto result = std::make_shared<shared::MTFeederProduct>(*pCache);
		result->SpreadBid = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_849D3EF577DDDE724FD40994D408F150"))->GetText().GetData());
		result->SpreadAsk = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_B22B0B4471F195A1BB8FA4BB0557D150"))->GetText().GetData());
		result->SpreadTotal = _ttoi(GetSubCtrl<CEditUI>(_T("DEF_E6453731ED2FEF9B2A72F6F3E0D954AE"))->GetText().GetData());
		if (GetSubCtrl<COptionUI>(_T("DEF_ECACA9AE7F4420437A5DDBCD7CE73E00"))->IsSelected()) {
			result->EnableQuote = true;
		}
		else {
			result->EnableQuote = false;
		}
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductEditUI::Parse() {
		sk::packet pak = GetCache();
		if (pak.empty()) return;
		shared::SystemProduct sysProd;
		sk::container::set<shared::LPProduct> lpProds;
		sk::container::set<shared::MTFeederProduct> mtFeederProds;
		sk::container::set<shared::MTGatewayProduct> mtGatewayProds;
		if (shared::ProductCon::DeconBuffer(pak, sysProd, lpProds, mtFeederProds, mtGatewayProds)) {
			return;
		}

		SetTagUINT64(sysProd.ID);
		CDialogBuilder builder;
		auto pSysProdEditCtrl = builder.Create<IProductSystemEditUI>(_T("UIProductsEditSystem.xml"));
		Add(pSysProdEditCtrl);
		pSysProdEditCtrl->Set(sysProd,lpProds,mtFeederProds,mtGatewayProds);
		pSysProdEditCtrl->SetTagUINT64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_SYSTEM);

		lpProds.iterate(
			[&](auto lpProd)
			{
				auto pLPProdEditCtrl = builder.Create<IProductExchangeEditUI>(_T("UIProductsEditExchange.xml"));
				Add(pLPProdEditCtrl);
				pLPProdEditCtrl->Set(lpProd);
				pLPProdEditCtrl->SetTagUINT64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_EXCHANGE);
			});

		mtGatewayProds.iterate(
			[&](auto& gatewayProd)
			{
				auto pGatewayProdEditCtrl = builder.Create<IProductMTGatewayEditUI>(_T("UIProductsEditGateway.xml"));
				Add(pGatewayProdEditCtrl);
				pGatewayProdEditCtrl->Set(gatewayProd, lpProds);
				pGatewayProdEditCtrl->SetTagUINT64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTGATEWAY);

				mtFeederProds.iterate(
					[&](auto& feederProd, bool& itbreak)
					{
						if (gatewayProd.Symbol == feederProd.Symbol)
						{
							auto pFeederProdEditCtrl = builder.Create<IProductMTFeederEditUI>(_T("UIProductsEditFeeder.xml"));
							Add(pFeederProdEditCtrl);
							pFeederProdEditCtrl->Set(feederProd);
							pFeederProdEditCtrl->SetTagUINT64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTFEEDER);
							itbreak = true;
						}
					});
			});
	}
	sk::packet IProductEditUI::Packet() {
		auto nLine = GetCount();
		if (nLine <= 0) return "";

		shared::SystemProduct sysProd;
		sk::container::set<shared::LPProduct> lpProds;
		sk::container::set<shared::MTFeederProduct> mtFeederProds;
		sk::container::set<shared::MTGatewayProduct> mtGatewayProds;

		sk::packet pak;
		for (int i = 0; i < nLine; ++i) {
			auto pTargetLine = GetItemAt(i);
			if (!pTargetLine) continue;

			switch (EnProductEditUIType(GetItemAt(i)->GetTagUINT64())) {
			case EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_SYSTEM: {
				auto pBusinessUI = (IProductSystemEditUI*)pTargetLine;
				auto sysProdObj = pBusinessUI->Get();
				if (sysProdObj) {
					sysProd = *sysProdObj;
				}
			}break;
			case EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_EXCHANGE: {
				auto pBusinessUI = (IProductExchangeEditUI*)pTargetLine;
				auto lpProdObj = pBusinessUI->Get();
				if (lpProdObj) {
					lpProds.push(*lpProdObj);
				}
			}break;
			case EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTFEEDER: {
				auto pBusinessUI = (IProductMTFeederEditUI*)pTargetLine;
				auto feederProdObj = pBusinessUI->Get();
				if (feederProdObj) {
					mtFeederProds.push(*feederProdObj);
				}
			}break;
			case EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTGATEWAY: {
				auto pBusinessUI = (IProductMTGatewayEditUI*)pTargetLine;
				auto gatewayProdObj = pBusinessUI->Get();
				if (gatewayProdObj) {
					mtGatewayProds.push(*gatewayProdObj);
				}
			}break;
			}///switch
		}

		if (sysProd.ID > 0) {

			shared::ProductCon prodConObj("", sysProd);
			lpProds.iterate([&](auto& prod) {prodConObj.EnconProd(prod); });

			mtGatewayProds.iterate(
				[&](shared::MTGatewayProduct& gatewayProd)
				{
					prodConObj.EnconProd(gatewayProd);
				});

			mtFeederProds.iterate(
				[&](shared::MTFeederProduct& feederProd, auto& itbreak)
				{
					prodConObj.EnconProd(feederProd);
				});

			prodConObj.EnconEnd();
			pak = prodConObj.BufferGet();
			Global::SharedGet()->m_SystemSymbolConQ.search(
				sysProd.ID,
				[&](const auto&, auto& con)
				{
					if (!pak.compare(con))
					{
						//pak.clear();
					}
				});
		}
		return pak;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IBridgeTradeRecordEditUI::IBridgeTradeRecordEditUI()
	{

	}
	IBridgeTradeRecordEditUI::~IBridgeTradeRecordEditUI()
	{

	}
	void IBridgeTradeRecordEditUI::Set(const shared::BridgeTradeRecord& record) {
		auto sk = 0;
	}
	std::shared_ptr<shared::BridgeTradeRecord> IBridgeTradeRecordEditUI::Get() {
		auto cache = GetCache();
		auto pCache = (shared::BridgeTradeRecord*)cache.data();
		auto result = std::make_shared<shared::BridgeTradeRecord>(*pCache);
		return result;
	}


}///namespace client