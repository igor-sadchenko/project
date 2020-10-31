#include "stdafx.h"
#include "UIProduct.h"

namespace client {
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductTreeNodeUI::SetSymbol(const sk::string& symbol) {
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"));
		if (pCtrl) {
			pCtrl->SetText(symbol.c_str());
		}
	}
	void IProductTreeNodeUI::SetQuotes(const shared::MarketData& md, const unsigned int& decimals) {
		//! Ask
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(md.PriceAskFinal, decimals,false).c_str());
		}
		//! Bid
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_E2C2A58989612AE0F43D81B979A3BE98"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(md.PriceBidFinal, decimals,false).c_str());
		}
		//! Spread
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_5599F20AF76F15A37DAFCAEE5C402466"));
		if (pCtrl) {
			pCtrl->SetText(tostring(static_cast<unsigned int>((abs)(md.PriceAskFinal * pow(10, decimals) - md.PriceBidFinal * pow(10, decimals)))).c_str());
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductSystemEditUI::Set(const shared::SystemProduct& sysProd, sk::container::set<shared::LPProduct>& lpProds) {
		GetSubCtrl<CEditUI>(_T("DEF_8199818406345AC3B8C7D7AF03360488"))->SetText(sk::Log::Format(_T("{:X}"), sysProd.ID.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_E4311B543C917BD2BDFA2FF6D3657B0E"))->SetText(sk::StringConvert::MBytesToTString(sysProd.Symbol.symbol).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_15CE628EC9E44E5E447CD0F8BEB7C1A2"))->SetText(sk::Log::Format(_T("{}"), sysProd.Decimals).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_A7FC9AACE8DCF522C6D377BAFAFBD376"))->SetText(sk::Log::Format(_T("{}"), sysProd.SpreadAsk).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_2D2CF54FB062AF57E4CEC73B1699CBB0"))->SetText(sk::Log::Format(_T("{}"), sysProd.SpreadBid).c_str());
		auto pCombo = GetSubCtrl<CComboUI>(_T("DEF_B2738C788AA1D1A64BB8E79CD7F60892"));
		if (pCombo) {
			lpProds.iterate(
				[&](auto& lpProd)
				{
					auto elem = new CListLabelElementUI();
					pCombo->Add(elem);
					elem->SetTag64(lpProd.ID.ID);
					elem->SetText(sk::Log::Format(_T("{:X}"), shared::MarketType::ExchangeType(lpProd.ID).ID).c_str());
				});
		}
	}
	std::shared_ptr<shared::SystemProduct> IProductSystemEditUI::Get() {
		auto result = std::make_shared<shared::SystemProduct>();
		result->ID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_8199818406345AC3B8C7D7AF03360488"))->GetText().GetData(), nullptr, 16);
		result->Symbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_E4311B543C917BD2BDFA2FF6D3657B0E"))->GetText().GetData()).c_str();
		result->Decimals = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_15CE628EC9E44E5E447CD0F8BEB7C1A2"))->GetText().GetData());
		result->SpreadAsk = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_15CE628EC9E44E5E447CD0F8BEB7C1A2"))->GetText().GetData());
		result->SpreadBid = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_2D2CF54FB062AF57E4CEC73B1699CBB0"))->GetText().GetData());
		auto pCombo = GetSubCtrl<CComboUI>(_T("DEF_B2738C788AA1D1A64BB8E79CD7F60892"));
		if (pCombo && pCombo->GetCount() > 0 && pCombo->GetCurSel() >= 0) {
			result->AssignOrderLP = pCombo->GetItemAt(pCombo->GetCurSel())->GetTag64();
		}
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductExchangeEditUI::Set(const shared::LPProduct& lpProd) {
		GetSubCtrl<CEditUI>(_T("DEF_2B8769552A23837B2A80ABD17E6830A9"))->SetText(sk::Log::Format(_T("{:X}"), lpProd.ID.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_782FE71EADA80A6608D43FD6F5748E8E"))->SetText(sk::StringConvert::MBytesToTString(lpProd.Symbol.symbol).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_F013433B32A84094D59D8182D9E8BAA0"))->SetText(sk::StringConvert::MBytesToTString(lpProd.Currency.currency).c_str());

		GetSubCtrl<CEditUI>(_T("DEF_F5DCB6944B77D5EF66CCA564C61441C8"))->SetText(sk::Log::Format(_T("{}"), lpProd.ContractMultiplier).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_B75AAB7E5503C7B5718EE7CEB9711A0E"))->SetText(sk::Helper::FloatToStr(lpProd.TickSize,6).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_BBD2423C9D525B438688CB19AC078D62"))->SetText(sk::Helper::FloatToStr(lpProd.MinOrderSize,6).c_str());

		GetSubCtrl<CEditUI>(_T("DEF_DF3DB43DBACF9CB65DE9D5383FF5CC53"))->SetText(sk::Log::Format(_T("{}"), lpProd.SpreadAsk).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_A4B1DD4431B12657EF423BA68D84CCBC"))->SetText(sk::Log::Format(_T("{}"), lpProd.SpreadBid).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_D4ADB0ABC446A84BA45D4DEF1D84F061"))->SetText(sk::StringConvert::MBytesToTString(lpProd.SystemSymbol.symbol).c_str());
	}
	std::shared_ptr<shared::LPProduct> IProductExchangeEditUI::Get() {
		auto result = std::make_shared<shared::LPProduct>();
		result->ID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_2B8769552A23837B2A80ABD17E6830A9"))->GetText().GetData(), nullptr, 16);
		result->Symbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_782FE71EADA80A6608D43FD6F5748E8E"))->GetText().GetData()).c_str();
		result->SystemSymbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_D4ADB0ABC446A84BA45D4DEF1D84F061"))->GetText().GetData()).c_str();
		result->Currency = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_F013433B32A84094D59D8182D9E8BAA0"))->GetText().GetData()).c_str();

		result->ContractMultiplier = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_F5DCB6944B77D5EF66CCA564C61441C8"))->GetText().GetData());
		result->SpreadAsk = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_DF3DB43DBACF9CB65DE9D5383FF5CC53"))->GetText().GetData());
		result->SpreadBid = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_A4B1DD4431B12657EF423BA68D84CCBC"))->GetText().GetData());

		result->TickSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_B75AAB7E5503C7B5718EE7CEB9711A0E"))->GetText().GetData());
		result->MinOrderSize = std::stod(GetSubCtrl<CEditUI>(_T("DEF_BBD2423C9D525B438688CB19AC078D62"))->GetText().GetData());
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductMTGatewayEditUI::Set(const shared::MTGatewayProduct& gatewayProds) {
		GetSubCtrl<CEditUI>(_T("DEF_3E839BF7789BF0E6DFA24342111D0612"))->SetText(sk::Log::Format(_T("{:X}"), gatewayProds.ID.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_B45EA3435AEEC80E041D2976445EF230"))->SetText(sk::StringConvert::MBytesToTString(gatewayProds.Symbol.symbol).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_5DA664B2E3667D44352F6DC1C8B99DC7"))->SetText(sk::Log::Format(_T("{:X}"), gatewayProds.SystemProductID.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.SpreadAsk).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC"))->SetText(sk::Log::Format(_T("{}"), gatewayProds.SpreadBid).c_str());

		GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->SetGroup(GetSubCtrl<CEditUI>(_T("DEF_3E839BF7789BF0E6DFA24342111D0612"))->GetText());
		GetSubCtrl<COptionUI>(_T("DEF_11FD062045877D22EA88791731E5BD1E"))->SetGroup(GetSubCtrl<CEditUI>(_T("DEF_3E839BF7789BF0E6DFA24342111D0612"))->GetText());

		if (gatewayProds.EnableTrade) {
			GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->Selected(true);
		}
		else {
			GetSubCtrl<COptionUI>(_T("DEF_11FD062045877D22EA88791731E5BD1E"))->Selected(true);
		}
	}
	std::shared_ptr<shared::MTGatewayProduct> IProductMTGatewayEditUI::Get() {
		auto result = std::make_shared<shared::MTGatewayProduct>();
		result->ID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_3E839BF7789BF0E6DFA24342111D0612"))->GetText().GetData(), nullptr, 16);
		result->SystemProductID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_5DA664B2E3667D44352F6DC1C8B99DC7"))->GetText().GetData(), nullptr, 16);
		result->Symbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_B45EA3435AEEC80E041D2976445EF230"))->GetText().GetData()).c_str();
		result->SpreadAsk = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_2BA0957EF0EA97174AB51EB0CEE81AA9"))->GetText().GetData());
		result->SpreadBid = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_E7CD4A879AC2AF83E8BCC972FA9440DC"))->GetText().GetData());
		if (GetSubCtrl<COptionUI>(_T("DEF_7F0D2E6D60A447C8B02AEC043D83BE10"))->IsSelected()) {
			result->EnableTrade = true;
		}
		else {
			result->EnableTrade = false;
		}
		return result;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void IProductMTFeederEditUI::Set(const shared::MTFeederProduct& feederProds) {
		GetSubCtrl<CEditUI>(_T("DEF_91BC201E3A9E8B97DAAAC548D7E15631"))->SetText(sk::Log::Format(_T("{:X}"), feederProds.ID.ID).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_EE3D9416EB92A67CC29EC2D7AEF91BBF"))->SetText(sk::StringConvert::MBytesToTString(feederProds.Symbol.symbol).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_888F53E1F0A47AD1FFB2525BA51E471D"))->SetText(sk::Log::Format(_T("{:X}"), feederProds.SystemProductID.ID).c_str());


		GetSubCtrl<COptionUI>(_T("DEF_C630DF0B7BBFF1BEAC51E8A4B99B8876"))->SetGroup(GetSubCtrl<CEditUI>(_T("DEF_91BC201E3A9E8B97DAAAC548D7E15631"))->GetText());
		GetSubCtrl<COptionUI>(_T("DEF_5E6DC2B241F54484E2AAFEDE5CE4BBE9"))->SetGroup(GetSubCtrl<CEditUI>(_T("DEF_91BC201E3A9E8B97DAAAC548D7E15631"))->GetText());
		if (feederProds.EnableQuote) {
			GetSubCtrl<COptionUI>(_T("DEF_C630DF0B7BBFF1BEAC51E8A4B99B8876"))->Selected(true);
		}
		else {
			GetSubCtrl<COptionUI>(_T("DEF_5E6DC2B241F54484E2AAFEDE5CE4BBE9"))->Selected(true);
		}
	}
	std::shared_ptr<shared::MTFeederProduct> IProductMTFeederEditUI::Get() {
		auto result = std::make_shared<shared::MTFeederProduct>();
		result->ID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_91BC201E3A9E8B97DAAAC548D7E15631"))->GetText().GetData(), nullptr, 16);
		result->SystemProductID = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_888F53E1F0A47AD1FFB2525BA51E471D"))->GetText().GetData(), nullptr, 16);
		result->Symbol = sk::StringConvert::WStringToMBytes(GetSubCtrl<CEditUI>(_T("DEF_EE3D9416EB92A67CC29EC2D7AEF91BBF"))->GetText().GetData()).c_str();
		if (GetSubCtrl<COptionUI>(_T("DEF_C630DF0B7BBFF1BEAC51E8A4B99B8876"))->IsSelected()) {
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
		
		CDialogBuilder builder;
		auto pSysProdEditCtrl = builder.Create<IProductSystemEditUI>(_T("UIProductsEditSystem.xml"));
		Add(pSysProdEditCtrl);
		pSysProdEditCtrl->Set(sysProd, lpProds);
		pSysProdEditCtrl->SetTag64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_SYSTEM);

		lpProds.iterate(
			[&](auto lpProd) 
			{
				auto pLPProdEditCtrl = builder.Create<IProductExchangeEditUI>(_T("UIProductsEditExchange.xml"));
				Add(pLPProdEditCtrl);
				pLPProdEditCtrl->Set(lpProd);
				pLPProdEditCtrl->SetTag64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_EXCHANGE);
			});
		mtFeederProds.iterate(
			[&](auto feederProd)
			{
				auto pFeederProdEditCtrl = builder.Create<IProductMTFeederEditUI>(_T("UIProductsEditFeeder.xml"));
				Add(pFeederProdEditCtrl);
				pFeederProdEditCtrl->Set(feederProd);
				pFeederProdEditCtrl->SetTag64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTFEEDER);
			});
		mtGatewayProds.iterate(
			[&](auto gatewayProd)
			{
				auto pGatewayProdEditCtrl = builder.Create<IProductMTGatewayEditUI>(_T("UIProductsEditGateway.xml"));
				Add(pGatewayProdEditCtrl);
				pGatewayProdEditCtrl->Set(gatewayProd);
				pGatewayProdEditCtrl->SetTag64((__int64)EnProductEditUIType::EN_PRODUCT_EDIT_UI_TYPE_MTGATEWAY);
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

			switch (EnProductEditUIType(GetItemAt(i)->GetTag64())) {
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

		if (sysProd.ID.Verify()) {
			shared::ProductCon prodConObj("", sysProd);
			lpProds.iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
			mtFeederProds.iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
			mtGatewayProds.iterate([&](auto& prod) {prodConObj.EnconProd(prod); });
			prodConObj.EnconEnd();
			pak = prodConObj.BufferGet();
			Global::SharedGet()->m_SystemSymbolConQ.search(
				sysProd.ID,
				[&](const auto&, auto& con)
				{			
					if (!pak.compare(con)) {
						pak.clear();
					}
				});
		}
		return pak;
	}
}///namespace client