#if !defined(AFX_SKSTU_INPUTCHECK_H__58C80_C83C0818_2B17FA5C8F80C41_8A19__HEAD__)
#define AFX_SKSTU_INPUTCHECK_H__58C80_C83C0818_2B17FA5C8F80C41_8A19__HEAD__


namespace shared {

	class InputCheck final {
	public:
		InputCheck() {}
		~InputCheck() = delete;
	public:


#if 0
		static sk::timestamp TimeStampToTimeFrameTimestamp(const sk::timestamp& srcTime/*时差修复后的值*/) {
			sk::stringa strTimeCalc = sk::time::Time::TimestampToTimeStrNoDate(srcTime, false);
			if (strTimeCalc == "00:00:00") {
				return 90060;
			}
			std::vector<sk::stringa> vrParse;
			sk::Helper::OneUnknownParseA(strTimeCalc, ':', vrParse);
			if (vrParse.size() == 3) {
				return strtoll(vrParse.at(0).c_str(), nullptr, 10) * 3600 + strtoll(vrParse.at(1).c_str(), nullptr, 10) * 60 + strtoll(vrParse.at(2).c_str(), nullptr, 10);
			}
			return 0;
		}
#endif


		//! 与本系统沟通交流的约定、限定、

		//! 通用编辑框输入长度限定
		static bool EditInputLimit(__inout sk::string& _src, __in const size_t& limit_length) {
			if (!_src.empty()) {
				if (_src.length() > limit_length) {
					_src[limit_length - 1] = 0;
					return false;
				}
			}
			return true;
		}

		//! SymbolSuffix 输入限定方法
		static bool SymbolsSuffixFix(__inout sk::string& _src, __in const size_t& limit_length) {
			if (_src.empty()) {
				return true;
			}

			if (_src[0] != '.') {
				_src[_src.length() - 1] = 0;
				return false;
			}

			if (!_src.empty()) {
				if (_src.find(_T("..")) != sk::string::npos) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}

			if (std::count(_src.begin(), _src.end(), '.') > 1) {
				_src[_src.length() - 1] = 0;
				return false;
			}

			if (_src.length() > limit_length) {
				_src[limit_length - 1] = 0;
				return false;
			}

			if (!_src.empty()) {
				if (!sk::Helper::IsDestStrCh(_T(".abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}

			return true;
		}

		//! Symbols 群的输入限定方法
		static bool SymbolsFix(__inout sk::string& _src) {
			if (_src.empty()) {
				return true;
			}
			if (_src.length() > ARRAYSIZE(shared::FollowField::SymbolAllow)) {
				_src[ARRAYSIZE(shared::FollowField::SymbolAllow) - 1] = 0;
				return false;
			}
			return true;
			if (_src.length() > 1) {
				if (_src[0] == '*' || _src[0] == '.') {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}


			if (!_src.empty()) {
				if (_src[0] == ',') {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}

			if (!_src.empty()) {
				auto find = _src.find('*');
				if (find != sk::string::npos) {
					if (_src[0] != '*') {
						_src[_src.length() - 1] = 0;
						return false;
					}
				}
			}

			if (!_src.empty()) {
				if (_src.find(_T("**")) != sk::string::npos || _src.find(_T("..")) != sk::string::npos || _src.find(_T(",,")) != sk::string::npos) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}

			if (!_src.empty()) {
				if (!sk::Helper::IsDestStrCh(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.*0123456789"), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			return true;
		}
		//! 小数的输入限定方法
		static bool DecimalsFix(__inout sk::string& _src, __in const double& _max_value, __in const int& _decimal_places_count_max, bool NeedNegative = true) {
			if (_src.empty()) {
				return true;
			}
			if (NeedNegative) {
				if (!sk::Helper::IsDestStrCh(_T("1234567890."), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
				if (!_src.empty()) {
					auto find = _src.find('-');
					if (find != sk::string::npos) {
						if (_src[0] != '-') {
							_src[_src.length() - 1] = 0;
							return false;
						}
					}
				}
				if (std::count(_src.begin(), _src.end(), '-') > 1) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			else {
				if (!sk::Helper::IsDestStrCh(_T("1234567890."), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			if (std::count(_src.begin(), _src.end(), '.') > 1) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			auto find = _src.find('.');
			if (find != sk::string::npos) {
				if ((_src.length() - find - 1) > _decimal_places_count_max) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			if (!(abs(_max_value) == 0)) {
				if (abs(_tcstod(_src.c_str(), nullptr)) > _max_value) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			return true;
		}
		//! MT帐户组
		static bool LoginsFix(__inout sk::string& _src) {
			if (_src.empty()) {
				return true;
			}
			if (!sk::Helper::IsDestStrCh(_T("\\1234567890"), _src[_src.length() - 1])) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			return true;
		}
		//! 整数的输入限定方法
		static bool IntegerlsFix(__inout sk::string& _src, __in const double& _max_value, __in const int& _decimal_places_count_max, bool NeedNegative = true) {
			if (_src.empty()) {
				return true;
			}
			if (NeedNegative) {
				if (!sk::Helper::IsDestStrCh(_T("-1234567890"), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
				if (!_src.empty()) {
					auto find = _src.find('-');
					if (find != sk::string::npos) {
						if (_src[0] != '-') {
							_src[_src.length() - 1] = 0;
							return false;
						}
					}
				}
				if (std::count(_src.begin(), _src.end(), '-') > 1) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			else {
				if (!sk::Helper::IsDestStrCh(_T("1234567890-"), _src[_src.length() - 1])) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			if (std::count(_src.begin(), _src.end(), '.') > 1) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			auto find = _src.find('.');
			if (find != sk::string::npos) {
				if ((_src.length() - find - 1) > _decimal_places_count_max) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			if (!(abs(_max_value) == 0)) {
				if (abs(_tcstod(_src.c_str(), nullptr)) > _max_value) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			return true;
		}
		//! 时间段的输入限定方法
		static bool TimeFrameFix(__inout sk::string& _src) {
			if (_src.empty()) {
				return true;
			}
			if (!sk::Helper::IsDestStrCh(_T("1234567890"), _src[0])) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			if (_src.length() == 2) {//! hour<24
				if (_src[1] != ':') {
					if (_tcstol(_src.c_str(), nullptr, 10) > 23) {
						_src[_src.length() - 1] = 0;
						return false;
					}
				}
			}
			if (_src.find(_T("::")) != sk::string::npos) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			if (!sk::Helper::IsDestStrCh(_T("1234567890:"), _src[_src.length() - 1])) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			if (std::count(_src.begin(), _src.end(), ':') > 2) {
				_src[_src.length() - 1] = 0;
				return false;
			}
			if (_src.length() > _tcslen(_T("0:0:0")) && _src.length() <= _tcslen(_T("00:00:00"))) {
				if (std::count(_src.begin(), _src.end(), ':') != 2) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}
			if (std::count(_src.begin(), _src.end(), ':') >= 1) {
				std::vector<sk::string> vrParse;
				sk::Helper::OneUnknownParse(_src, ':', vrParse);

				if (_tcstol(vrParse[vrParse.size() - 1].c_str(), nullptr, 10) > 59) {
					_src[_src.length() - 1] = 0;
					return false;
				}
			}

			return true;
		}
		//! 检测系统约定的时间段字符串是否合理
		static bool VerifyTimeFrameStr(const sk::string& srcTime) {
			if (srcTime.find(_T("::")) != sk::string::npos) {
				return false;
			}
			if (std::count(srcTime.begin(), srcTime.end(), ':') != 2) {
				return false;
			}
			std::vector<sk::string> vrParse;
			sk::Helper::OneUnknownParse(srcTime, ':', vrParse);
			if (vrParse.size() != 3) {
				return false;
			}
			auto hour = _tcstol(vrParse.at(0).c_str(), nullptr, 10);
			auto min = _tcstol(vrParse.at(1).c_str(), nullptr, 10);
			auto sec = _tcstol(vrParse.at(2).c_str(), nullptr, 10);
			if (hour > 23) {
				return false;
			}
			if (min > 59) {
				return false;
			}
			if (sec > 59) {
				return false;
			}
			return true;
		}


	};
}///namespace shared




/// /*新生联创（上海）**/
/// /*2020年01月12日 18:18:49|651**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_INPUTCHECK_H__58C80_C83C0818_2B17FA5C8F80C41_8A19__TAIL__