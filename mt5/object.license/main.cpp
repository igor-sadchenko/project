#include "stdafx.h"
#include "shlobj.h"

enum struct EnCurrentModule : UINT
{
	EN_CURRENT_MODULE_PRODUCT_TYPE = 0,
	EN_CURRENT_MODULE_PRODUCT_VERSION_TYPE = 1,
	EN_CURRENT_MODULE_PRODUCT_LICENSE_TYPE = 2,
	EN_CURRENT_MODULE_PRODUCT_CLIENT_LOGO = 3,
};


int main(int argc, char* argv[])
{
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(268);
#endif

	sk::Dump __Dump;

	sk::LicenseParam licenseParam;
	EnCurrentModule CurrentModule = EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_TYPE;

	do {

		sk::stringa tipInfo;
		switch (CurrentModule)
		{
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_TYPE:
		{
			tipInfo = "\n授权产品类型ID:\nSK抛单系统: 4\nSK跟单系统: 2\n";
			tipInfo.append("请输入授权产品类型(并按Enter键继续):");
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_VERSION_TYPE:
		{
			tipInfo = "\n授权产品版本类型:\nBETA: 0\nDEMO: 1\nTRIAL: 2\nOFFICIAL: 3\nFINAL: 4\n";
			tipInfo.append("请输入授权产品版本类型(并按Enter键继续):");
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_LICENSE_TYPE:
		{
			tipInfo = "\n授权产品期限类型:\n一周: 0\n一个月: 1\n三个月: 2\n半年: 3\n一年: 4\n三年: 5\n终身: 6\n";
			tipInfo.append("请输入授权产品期限类型(并按Enter键继续):");
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_CLIENT_LOGO:
		{
			tipInfo = "\n请输入授权目标客户英文LOGO:";
		}break;
		}

		local::Global::Log()->warn(tipInfo);

		sk::stringa _input;
		char c;
		while (std::cin >> std::noskipws >> c)
		{
			if ('\n' == c) break;
			_input.push_back(c);
		}
		std::cin >> std::skipws;

		sk::StringConvert::ToLowerA(_input);

		switch (CurrentModule)
		{
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_TYPE:
		{
			auto typeProduct = atoll(_input.c_str());
			switch (sk::LicenseInfo::EnProductType(typeProduct))
			{
			case sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_SA:
			{
			}break;
			case sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_FOLLOWOBJ_MA:
			{
			}break;
			case sk::LicenseInfo::EnProductType::EN_PRODUCT_TYPE_BRIGDE_PROXY:
			{
			}break;
			default:
			{
				local::Global::Log()->error("不支持的产品类型,请重新输入!");
				continue;
			}break;
			}

			licenseParam.ProductType = sk::LicenseInfo::EnProductType(typeProduct);
			CurrentModule = EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_VERSION_TYPE;

		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_VERSION_TYPE:
		{
			auto typeProductVersion = atoll(_input.c_str());
			switch (sk::LicenseInfo::EnProductVersionType(typeProductVersion))
			{
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_BETA:
			{
			}break;
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_DEMO:
			{
			}break;
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_OFFICIAL:
			{
			}break;
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_TRIAL:
			{
			}break;
			case sk::LicenseInfo::EnProductVersionType::EN_PRODUCT_VERSION_TYPE_FINAL:
			{
			}break;
			default:
			{
				local::Global::Log()->error("不支持的产品版本类型,请重新输入!");
				continue;
			}break;
			}

			licenseParam.ProductVersionType = sk::LicenseInfo::EnProductVersionType(typeProductVersion);
			CurrentModule = EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_LICENSE_TYPE;
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_LICENSE_TYPE:
		{
			auto typeProductLicense = atoll(_input.c_str());
			switch (sk::LicenseInfo::EnProductLicenseType(typeProductLicense))
			{
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_MONTH_1:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_MONTH_3:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_MONTH_6:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_WEEK_1:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_YEAR_1:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_YEAR_3:
			{
			}break;
			case sk::LicenseInfo::EnProductLicenseType::EN_PRODUCT_LICENSE_TYPE_FINAL:
			{
			}break;
			default:
			{
				local::Global::Log()->error("不支持的产品授权(期限)类型,请重新输入!");
				continue;
			}break;
			}
			licenseParam.ProductLicenseType = sk::LicenseInfo::EnProductLicenseType(typeProductLicense);
			CurrentModule = EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_CLIENT_LOGO;
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_PRODUCT_CLIENT_LOGO:
		{
			if (!_input.empty())
			{
				sk::SafeCopyA(licenseParam.ClientLogo, _input.c_str(), _countof(licenseParam.ClientLogo));
			}
			else
			{
				sk::SafeCopyA(licenseParam.ClientLogo, "UNKNOWN", _countof(licenseParam.ClientLogo));
			}

			SKAPIRES result = SKAPIRES::SYSTEM_SUCCESS;
			sk::License licenseObj;
			auto readyFileBuffer = sk::Helper::LoadFile((local::GetGlobal()->GetCurrentProcessDirectory() + sk::LICENSE_FILENAME_READY).c_str());
			if (readyFileBuffer.empty())
			{
				local::Global::Log()->error("授权失败，当前目录未找到预授权文件({})!", sk::LICENSE_FILENAME_READY);
				result = SKAPIRES::SYSTEM_FAILED;
			}
			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				sk::stringa licenseBuffer;
				if (licenseObj.Made("SK", sk::stringa(&readyFileBuffer[0], readyFileBuffer.size()), licenseParam))
				{
					local::Global::Log()->error("授权失败，制作授权文件失败!");
					result = SKAPIRES::SYSTEM_FAILED;
				}
			}
			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				auto enBuffer = sk::encrypt::wemade::Encode6Bit(sk::stringa((char*)&licenseParam, sizeof(licenseParam)));
				if (enBuffer.empty())
				{
					local::Global::Log()->error("授权失败，制作授权文件失败!");
					result = SKAPIRES::SYSTEM_FAILED;
				}
				else
				{
					auto targetFile = local::GetGlobal()->GetCurrentProcessDirectory() + sk::LICENSE_FILENAME;
					std::ofstream ofs(targetFile, std::ios::out | std::ios::binary | std::ios::trunc);
					if (ofs.is_open())
					{
						ofs.write(enBuffer.data(), enBuffer.size());
						ofs.flush();
						ofs.close();
					}
				}
			}

			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				local::Global::Log()->error("\n授权完成!\n授权文件({})\n生成完成 !", local::GetGlobal()->GetCurrentProcessDirectory() + sk::LICENSE_FILENAME);
			}
			else
			{
				local::Global::Log()->error("授权失败，授权文件生成失败 !");
			}
			std::this_thread::sleep_for(std::chrono::seconds(10));
			return 0;
		}break;
		}
	} while (1);

	return 0;
}

