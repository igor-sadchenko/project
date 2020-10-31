#include "stdafx.h"
#include "UIFollowEdit.h"

namespace client {

	///////////////////////////////////////////////////////////////////////////////////////////
	void IFollowTreeNodeUI::Set(const shared::FollowField& followObj) {
		//!@ 跟单关系
		auto pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_A342DF16ECC84D60E04A1ED50608896B"));
		if (pCtrl) {
			pCtrl->SetText(sk::StringConvert::MBytesToWString(followObj.GetKey().Get()).c_str());
		}
		//!@ 跟单开关
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_963EFA69E278CD1155AC76C4EBEC9376"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetFollowFlagStr());
		}
		//!@ 强跟开关
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_FA9A5A6CB1914A323C7D89C04876649A"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetFollowAddToFlagStr());
		}
		//!@ 方向类型
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_6ECC31A5BDCB11AB5AA7738A6C6396B9"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetTypeDirectionStr());
		}
		//!@ 多空类型
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_3F346FFEDFE756B92F5A03E57425BE26"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetTypeTypeStr());
		}
		//!@ 最大阀值
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_53FA8DFCF070E33B7F97A3F997BB42BE"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeOverMax, 5).c_str());
		}
		//!@ 最小阀值
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_951A56DAB5F4544C403268BB1C2B26D6"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeOverMin, 5).c_str());
		}
		//!@ 跟随类型
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_4D8D63E4C550650B3F87ED63671E6AEB"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetFollowModeStr());
		}
		//!@ 下单比例
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_1175CA3BC615F601DA5A2639F080E8D5"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeRatio, 5).c_str());
		}
		//!@ 固定手数
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_F6695B34D484B2131BBFEFFE6E12CB67"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeFixed, 5).c_str());
		}
		//!@ 最小手数
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_EAE26B56A97B758401B9062686536794"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeMin, 5).c_str());
		}
		//!@ 最大手数
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_B19FD83748084589C79EFF1A92DB7735"));
		if (pCtrl) {
			pCtrl->SetText(sk::Helper::FloatToStr(followObj.VolumeMax, 5).c_str());
		}
		//!@ 止盈止损
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_75D730AD3D0F0E56363C54565092ED6D"));
		if (pCtrl) {
			pCtrl->SetText(followObj.GetSLTPLimitFlagStr());
		}
		//!@ 点差校正
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_F852E36B7F57C543456D12F2AE99ACF6"));
		if (pCtrl) {
			pCtrl->SetText(tostring(followObj.PointSLTP).c_str());
		}
		//!@ 更新时间
		pCtrl = GetSubCtrl<CLabelUI>(_T("DEF_76F0E723BE7CC7B674C20C18DC652E9A"));
		if (pCtrl) {
			pCtrl->SetText(sk::StringConvert::MBytesToWString(sk::time::Time::TimestampToTimeStr(followObj.TimeUpdate)).c_str());
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	void IFollowConfigureEditUI::Set(const shared::FollowField& con) {
		//!@ 跟单开关
		switch (con.FollowFlag) {
		case shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_ENABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_F2B7998118BCFA412A5F489CCAF444F2"))->Selected(true);
		}break;
		case shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_DISABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_9B73723DD4753563489F46CD07B5A0CF"))->Selected(true);
		}break;
		}
		//!@ 强跟开关
		switch (con.FollowAddToFlag) {
		case shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_2C29C48A8359E0EC265347A7E89B2F16"))->Selected(true);
		}break;
		case shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_DISABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_3AEF3AF319B23570C63DCA46878B4CDE"))->Selected(true);
		}break;
		}
		//!@ 信号帐户
		GetSubCtrl<CEditUI>(_T("DEF_3F7E9179BC7A289837CAEC89B009C958"))->SetText(tostring(con.LoginSignal).c_str());
		//!@ 跟单帐户
		GetSubCtrl<CEditUI>(_T("DEF_6D9D4D263116926A47CA77ACB833BF1C"))->SetText(tostring(con.LoginFollow).c_str());
		//!@ 跟随方向
		switch (con.TypeDirection) {
		case shared::FollowField::EnTypeDriection::EN_TYPE_DRIECTION_CONS: {
			GetSubCtrl<COptionUI>(_T("DEF_73F8C0D042DAA967DC47ACA426EE377D"))->Selected(true);
		}break;
		case shared::FollowField::EnTypeDriection::EN_TYPE_DRIECTION_PROS: {
			GetSubCtrl<COptionUI>(_T("DEF_AF871C850A02A4E34EF9DDAFED991BE9"))->Selected(true);
		}break;
		}
		//!@ 多空方向
		switch (con.TypeType) {
		case shared::FollowField::EnTypeType::EN_TYPE_TYPE_ALL: {
			GetSubCtrl<COptionUI>(_T("DEF_9F3757C4741E95403CC4F7877F7A66C3"))->Selected(true);
		}break;
		case shared::FollowField::EnTypeType::EN_TYPE_TYPE_BUY: {
			GetSubCtrl<COptionUI>(_T("DEF_B705A60256D3C3E3464588EF853D24F3"))->Selected(true);
		}break;
		case shared::FollowField::EnTypeType::EN_TYPE_TYPE_SELL: {
			GetSubCtrl<COptionUI>(_T("DEF_86ACFC2BB8E140F23C56C120C221B4BC"))->Selected(true);
		}break;
		}
		//!@ 最大阀值
		GetSubCtrl<CEditUI>(_T("DEF_7F97CD1778B9D4BE9EC8E23FEE336B48"))->SetText(sk::Helper::FloatToStr(con.VolumeOverMax, 5).c_str());
		//!@ 最小阀值
		GetSubCtrl<CEditUI>(_T("DEF_5C46AAB7E6E548870319607A28AAD2D0"))->SetText(sk::Helper::FloatToStr(con.VolumeOverMin, 5).c_str());
		//!@ 跟随类型
		switch (con.FollowMode) {
		case shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_FIXED: {
			GetSubCtrl<COptionUI>(_T("DEF_F8958799EBD889CAEA5AAD0EFA34BC94"))->Selected(true);
		}break;
		case shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO: {
			GetSubCtrl<COptionUI>(_T("DEF_DAEFA0E14135834DD76BB2C69B1C77FD"))->Selected(true);
		}break;
		case shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RANGE: {
			GetSubCtrl<COptionUI>(_T("DEF_1C5C48C2A89F06D7D15E2C2EEB91AA43"))->Selected(true);
		}break;
		}
		//!@ 下单比例
		GetSubCtrl<CEditUI>(_T("DEF_F82FECD30755D5E09C234BF051BA8F3D"))->SetText(sk::Helper::FloatToStr(con.VolumeRatio, 2).c_str());
		//!@ 固定手数
		GetSubCtrl<CEditUI>(_T("DEF_8EF6A21FB9FE3B15581F1E45B1D2F58F"))->SetText(sk::Helper::FloatToStr(con.VolumeFixed, 2).c_str());
		//!@ 最小手数
		GetSubCtrl<CEditUI>(_T("DEF_067173C7CFD5F93445FF7114B87CA045"))->SetText(sk::Helper::FloatToStr(con.VolumeMin, 2).c_str());
		//!@ 最大手数
		GetSubCtrl<CEditUI>(_T("DEF_D35797EA888634519D504A4FEC272437"))->SetText(sk::Helper::FloatToStr(con.VolumeMax, 2).c_str());
		//!@ 后缀校正
		GetSubCtrl<CEditUI>(_T("DEF_9D3826902B29C4BF7EF9553D4ADCE2D2"))->SetText(con.SymbolSuffix);
		//!@ 只跟品种
		GetSubCtrl<CEditUI>(_T("DEF_4A4E04C8E1C1B0D6034A66CD53BE7716"))->SetText(con.SymbolAllow);
		//!@ 不跟品种
		GetSubCtrl<CEditUI>(_T("DEF_649419F876DFEF711D1DE21B94FB50DD"))->SetText(con.SymbolReject);
		//!@ 仓位阀值
		GetSubCtrl<CEditUI>(_T("DEF_901ADEC2BB6996595388B780C65E29CA"))->SetText(tostring(con.PositionOverMax).c_str());
		//!@ 时段开关
		switch (con.TimeFrameFollowFlag) {
		case shared::FollowField::EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_ENABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_E1F2BCF5C64DD0CB47084874E1B8A94D"))->Selected(true);
		}break;
		case shared::FollowField::EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_DISABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_630D8ED7CFAF18CD2DF7F4EB7708FD7B"))->Selected(true);
		}break;
		}
		//!@ 时段设置
		GetSubCtrl<CEditUI>(_T("DEF_ED60DB5B2FFCAAFC31AEDD998003BFC4"))->SetText(shared::FollowField::TimeFrameTimestampToStrBegin(con.TimeFrameFollowBegin[0]).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_18AF32058A212DAC49DD0B3EAEA01B98"))->SetText(shared::FollowField::TimeFrameTimestampToStrEnd(con.TimeFrameFollowEnd[0]).c_str());
		//!@ 时段设置
		GetSubCtrl<CEditUI>(_T("DEF_6ACBBEB43583AB333EA7F78CDFC83898"))->SetText(shared::FollowField::TimeFrameTimestampToStrBegin(con.TimeFrameFollowBegin[1]).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_6D79808AAE45F4C6A0576A98A48F87F9"))->SetText(shared::FollowField::TimeFrameTimestampToStrEnd(con.TimeFrameFollowEnd[1]).c_str());
		//!@ 时段设置
		GetSubCtrl<CEditUI>(_T("DEF_B4743DD2747F626BEDFB383B963A9C87"))->SetText(shared::FollowField::TimeFrameTimestampToStrBegin(con.TimeFrameFollowBegin[2]).c_str());
		GetSubCtrl<CEditUI>(_T("DEF_53B6D4A8B02AC82700F95ABCF8ED69B5"))->SetText(shared::FollowField::TimeFrameTimestampToStrEnd(con.TimeFrameFollowEnd[2]).c_str());
		//!@ 盈损开关
		switch (con.PointSLTPFlag) {
		case shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_9A1DC47B5592E47FEB5B1C2EF502ED3A"))->Selected(true);
		}break;
		case shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_DISABLE: {
			GetSubCtrl<COptionUI>(_T("DEF_80A5644F3A576E292723DE5F2DC126F6"))->Selected(true);
		}break;
		}
		//!@ 点差校正
		GetSubCtrl<CEditUI>(_T("DEF_27210D641F5C62325E7B2716BA2C04C5"))->SetText(tostring(con.PointSLTP).c_str());
		//!@ 跟单注释
		GetSubCtrl<CEditUI>(_T("DEF_59E8CCC844803415E9EBA48164F665A9"))->SetText(con.IMTComment);
	}
	std::shared_ptr<shared::FollowField> IFollowConfigureEditUI::Get() {
		auto result = std::make_shared<shared::FollowField>();

		//!@ 跟单开关
		if (GetSubCtrl<COptionUI>(_T("DEF_F2B7998118BCFA412A5F489CCAF444F2"))->IsSelected()) {
			result->FollowFlag = shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_ENABLE;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_9B73723DD4753563489F46CD07B5A0CF"))->IsSelected()) {
			result->FollowFlag = shared::FollowField::EnFollowFlag::EN_TRADE_FLAG_DISABLE;
		}
		//!@ 强跟开关
		if (GetSubCtrl<COptionUI>(_T("DEF_2C29C48A8359E0EC265347A7E89B2F16"))->IsSelected()) {
			result->FollowAddToFlag = shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_ENABLE;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_3AEF3AF319B23570C63DCA46878B4CDE"))->IsSelected()) {
			result->FollowAddToFlag = shared::FollowField::EnFollowAddToFlag::EN_FOLLOWADDTO_FLAG_DISABLE;
		}
		//!@ 信号帐户
		if (GetSubCtrl<CEditUI>(_T("DEF_3F7E9179BC7A289837CAEC89B009C958"))->GetText().GetData()) {
			result->LoginSignal = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_3F7E9179BC7A289837CAEC89B009C958"))->GetText().GetData());
		}
		//!@ 跟单帐户
		if (GetSubCtrl<CEditUI>(_T("DEF_6D9D4D263116926A47CA77ACB833BF1C"))->GetText().GetData()) {
			result->LoginFollow = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_6D9D4D263116926A47CA77ACB833BF1C"))->GetText().GetData());
		}
		//!@ 跟随方向
		if (GetSubCtrl<COptionUI>(_T("DEF_73F8C0D042DAA967DC47ACA426EE377D"))->IsSelected()) {
			result->TypeDirection = shared::FollowField::EnTypeDriection::EN_TYPE_DRIECTION_CONS;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_AF871C850A02A4E34EF9DDAFED991BE9"))->IsSelected()) {
			result->TypeDirection = shared::FollowField::EnTypeDriection::EN_TYPE_DRIECTION_PROS;
		}
		//!@ 多空方向
		if (GetSubCtrl<COptionUI>(_T("DEF_9F3757C4741E95403CC4F7877F7A66C3"))->IsSelected()) {
			result->TypeType = shared::FollowField::EnTypeType::EN_TYPE_TYPE_ALL;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_B705A60256D3C3E3464588EF853D24F3"))->IsSelected()) {
			result->TypeType = shared::FollowField::EnTypeType::EN_TYPE_TYPE_BUY;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_86ACFC2BB8E140F23C56C120C221B4BC"))->IsSelected()) {
			result->TypeType = shared::FollowField::EnTypeType::EN_TYPE_TYPE_SELL;
		}
		//!@ 最大阀值
		if (GetSubCtrl<CEditUI>(_T("DEF_7F97CD1778B9D4BE9EC8E23FEE336B48"))->GetText().GetData()) {
			result->VolumeOverMax = std::stod(GetSubCtrl<CEditUI>(_T("DEF_7F97CD1778B9D4BE9EC8E23FEE336B48"))->GetText().GetData());
		}
		//!@ 最小阀值
		if (GetSubCtrl<CEditUI>(_T("DEF_5C46AAB7E6E548870319607A28AAD2D0"))->GetText().GetData()) {
			result->VolumeOverMin = std::stod(GetSubCtrl<CEditUI>(_T("DEF_5C46AAB7E6E548870319607A28AAD2D0"))->GetText().GetData());
		}
		//!@ 跟随类型
		if (GetSubCtrl<COptionUI>(_T("DEF_F8958799EBD889CAEA5AAD0EFA34BC94"))->IsSelected()) {
			result->FollowMode = shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_FIXED;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_DAEFA0E14135834DD76BB2C69B1C77FD"))->IsSelected()) {
			result->FollowMode = shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RATIO;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_1C5C48C2A89F06D7D15E2C2EEB91AA43"))->IsSelected()) {
			result->FollowMode = shared::FollowField::EnFollowMode::EN_TRADE_MODE_VOLUME_RANGE;
		}
		//!@ 下单比例
		if (GetSubCtrl<CEditUI>(_T("DEF_F82FECD30755D5E09C234BF051BA8F3D"))->GetText().GetData()) {
			result->VolumeRatio = std::stod(GetSubCtrl<CEditUI>(_T("DEF_F82FECD30755D5E09C234BF051BA8F3D"))->GetText().GetData());
		}
		//!@ 固定手数
		if (GetSubCtrl<CEditUI>(_T("DEF_8EF6A21FB9FE3B15581F1E45B1D2F58F"))->GetText().GetData()) {
			result->VolumeFixed = std::stod(GetSubCtrl<CEditUI>(_T("DEF_8EF6A21FB9FE3B15581F1E45B1D2F58F"))->GetText().GetData());
		}
		//!@ 最小手数
		if (GetSubCtrl<CEditUI>(_T("DEF_067173C7CFD5F93445FF7114B87CA045"))->GetText().GetData()) {
			result->VolumeMin = std::stod(GetSubCtrl<CEditUI>(_T("DEF_067173C7CFD5F93445FF7114B87CA045"))->GetText().GetData());
		}
		//!@ 最大手数
		if (GetSubCtrl<CEditUI>(_T("DEF_D35797EA888634519D504A4FEC272437"))->GetText().GetData()) {
			result->VolumeMax = std::stod(GetSubCtrl<CEditUI>(_T("DEF_D35797EA888634519D504A4FEC272437"))->GetText().GetData());
		}
		//!@ 后缀校正
		if (GetSubCtrl<CEditUI>(_T("DEF_9D3826902B29C4BF7EF9553D4ADCE2D2"))->GetText().GetData()) {
			sk::SafeCopyW(result->SymbolSuffix,\
				GetSubCtrl<CEditUI>(_T("DEF_9D3826902B29C4BF7EF9553D4ADCE2D2"))->GetText().GetData(),\
				_countof(result->SymbolSuffix));
		}
		//!@ 只跟品种
		if (GetSubCtrl<CEditUI>(_T("DEF_4A4E04C8E1C1B0D6034A66CD53BE7716"))->GetText().GetData()) {
			sk::SafeCopyW(result->SymbolAllow, \
				GetSubCtrl<CEditUI>(_T("DEF_4A4E04C8E1C1B0D6034A66CD53BE7716"))->GetText().GetData(), \
				_countof(result->SymbolAllow));
		}
		//!@ 不跟品种
		if (GetSubCtrl<CEditUI>(_T("DEF_649419F876DFEF711D1DE21B94FB50DD"))->GetText().GetData()) {
			sk::SafeCopyW(result->SymbolReject, \
				GetSubCtrl<CEditUI>(_T("DEF_649419F876DFEF711D1DE21B94FB50DD"))->GetText().GetData(), \
				_countof(result->SymbolReject));
		}
		//!@ 仓位阀值
		if (GetSubCtrl<CEditUI>(_T("DEF_901ADEC2BB6996595388B780C65E29CA"))->GetText().GetData()) {
			result->PositionOverMax = std::stoll(GetSubCtrl<CEditUI>(_T("DEF_901ADEC2BB6996595388B780C65E29CA"))->GetText().GetData());
		}
		//!@ 时段开关
		if (GetSubCtrl<COptionUI>(_T("DEF_E1F2BCF5C64DD0CB47084874E1B8A94D"))->IsSelected()) {
			result->TimeFrameFollowFlag = shared::FollowField::EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_ENABLE;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_630D8ED7CFAF18CD2DF7F4EB7708FD7B"))->IsSelected()) {
			result->TimeFrameFollowFlag = shared::FollowField::EnTimeFrameFollowFlag::EN_TIMEFRAME_FOLLOW_FLAG_DISABLE;
		}
		//!@ 时段设置
		if (GetSubCtrl<CEditUI>(_T("DEF_ED60DB5B2FFCAAFC31AEDD998003BFC4"))->GetText().GetData()&&
			GetSubCtrl<CEditUI>(_T("DEF_18AF32058A212DAC49DD0B3EAEA01B98"))->GetText().GetData()) {
			result->TimeFrameFollowBegin[0] = shared::FollowField::TimeFrameStrToTimestampBegin(GetSubCtrl<CEditUI>(_T("DEF_ED60DB5B2FFCAAFC31AEDD998003BFC4"))->GetText().GetData());
			result->TimeFrameFollowEnd[0] = shared::FollowField::TimeFrameStrToTimestampEnd(GetSubCtrl<CEditUI>(_T("DEF_18AF32058A212DAC49DD0B3EAEA01B98"))->GetText().GetData());
		}
		//!@ 时段设置
		if (GetSubCtrl<CEditUI>(_T("DEF_6ACBBEB43583AB333EA7F78CDFC83898"))->GetText().GetData() &&
			GetSubCtrl<CEditUI>(_T("DEF_6D79808AAE45F4C6A0576A98A48F87F9"))->GetText().GetData()) {
			result->TimeFrameFollowBegin[1] = shared::FollowField::TimeFrameStrToTimestampBegin(GetSubCtrl<CEditUI>(_T("DEF_6ACBBEB43583AB333EA7F78CDFC83898"))->GetText().GetData());
			result->TimeFrameFollowEnd[1] = shared::FollowField::TimeFrameStrToTimestampEnd(GetSubCtrl<CEditUI>(_T("DEF_6D79808AAE45F4C6A0576A98A48F87F9"))->GetText().GetData());
		}
		//!@ 时段设置
		if (GetSubCtrl<CEditUI>(_T("DEF_B4743DD2747F626BEDFB383B963A9C87"))->GetText().GetData() &&
			GetSubCtrl<CEditUI>(_T("DEF_53B6D4A8B02AC82700F95ABCF8ED69B5"))->GetText().GetData()) {
			result->TimeFrameFollowBegin[2] = shared::FollowField::TimeFrameStrToTimestampBegin(GetSubCtrl<CEditUI>(_T("DEF_B4743DD2747F626BEDFB383B963A9C87"))->GetText().GetData());
			result->TimeFrameFollowEnd[2] = shared::FollowField::TimeFrameStrToTimestampEnd(GetSubCtrl<CEditUI>(_T("DEF_53B6D4A8B02AC82700F95ABCF8ED69B5"))->GetText().GetData());
		}
		//!@ 盈损开关
		if (GetSubCtrl<COptionUI>(_T("DEF_9A1DC47B5592E47FEB5B1C2EF502ED3A"))->IsSelected()) {
			result->PointSLTPFlag = shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_ENABLE;
		}
		else if (GetSubCtrl<COptionUI>(_T("DEF_80A5644F3A576E292723DE5F2DC126F6"))->IsSelected()) {
			result->PointSLTPFlag = shared::FollowField::EnSLTPLimitFlag::EN_SLTP_LIMIT_FLAG_DISABLE;
		}
		//!@ 点差校正
		if (GetSubCtrl<CEditUI>(_T("DEF_27210D641F5C62325E7B2716BA2C04C5"))->GetText().GetData()) {
			result->PointSLTP = std::stoi(GetSubCtrl<CEditUI>(_T("DEF_27210D641F5C62325E7B2716BA2C04C5"))->GetText().GetData());
		}
		//!@ 跟单注释
		if (GetSubCtrl<CEditUI>(_T("DEF_59E8CCC844803415E9EBA48164F665A9"))->GetText().GetData()) {
			sk::SafeCopyW(result->IMTComment, GetSubCtrl<CEditUI>(_T("DEF_59E8CCC844803415E9EBA48164F665A9"))->GetText().GetData(), _countof(result->IMTComment));
		}
		return result;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	void IFollowEditUI::Parse() {
		sk::packet pak = GetCache();
		if (pak.empty()) return;
		auto pFollow = (shared::FollowField*)pak.data();
		if (!pFollow->Verify()) return;

		CDialogBuilder builder;
		auto pFollowEditCtrl = builder.Create<IFollowConfigureEditUI>(_T("UIFollowConfigureEdit.xml"));
		Add(pFollowEditCtrl);
		pFollowEditCtrl->Set(*pFollow);
		pFollowEditCtrl->SetUserData(sk::StringConvert::MBytesToWString(pFollow->GetKey().Get()).c_str());
	}

	sk::packet IFollowEditUI::Packet() {
		auto nLine = GetCount();
		if (nLine <= 0) return "";
		sk::packet pak;
		for (int i = 0; i < nLine; ++i) {
			auto pTargetLine = GetItemAt(i);
			if (!pTargetLine) continue;
			auto pFollowEditNode = (IFollowConfigureEditUI*)pTargetLine;
			auto pFollowData = pFollowEditNode->Get();
			if (pFollowData) {
				pFollowData->TimeUpdate = ::time(0);
				pak.append((char*)pFollowData.get(), shared::LENFOLLOWFIELD);
			}
		}
		return pak;
	}


}///namespace client