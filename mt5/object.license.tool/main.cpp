#include "stdafx.h"

enum struct EnCurrentModule : UINT
{
	EN_CURRENT_MODULE_LICENSE_READY_MADE = 0,
	EN_CURRENT_MODULE_LICENSE_SETUP = 1,
	EN_CURRENT_MODULE_LICENSE_UNSETUP = 2,
};

int main(int argc, char* argv[])
{

	sk::MachineHardwareInformation MachineLocal;
	if (sk::Helper::MachineHardwareInformationGet(MachineLocal))
	{
		for (int i = 0; i < 3; ++i)
		{
			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("{}", "Unknown hardware (MAC),The program cannot continue!");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		return 0;
	}



	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	}

#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(268);
#endif

	sk::Dump __Dump;

	EnCurrentModule CurrentModule = EnCurrentModule::EN_CURRENT_MODULE_LICENSE_READY_MADE;

	do {

		sk::stringa tipInfo;
		tipInfo = "\n制作预授权文件: 0\n安装授权文件: 1\n卸载全部产品授权文件: 2\n";
		tipInfo.append("请输入操作类型(并按Enter键继续):");

		local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn(tipInfo);

		sk::stringa _input;
		char c;
		while (std::cin >> std::noskipws >> c)
		{
			if ('\n' == c) break;
			_input.push_back(c);
		}
		std::cin >> std::skipws;
		sk::StringConvert::ToLowerA(_input);
		CurrentModule = EnCurrentModule(atoll(_input.c_str()));

		switch (CurrentModule)
		{
		case EnCurrentModule::EN_CURRENT_MODULE_LICENSE_READY_MADE:
		{
			if (!MachineLocal.Verify())
			{
				local::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("Unable to obtain local system hardware information.");
				local::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("The system cannot generate an license file!");
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return -1;
			}

			auto LicenseReadyData = sk::encrypt::wemade::Encode6Bit(sk::packet((char*)&MachineLocal,sk::LENMACHINEHARDWAREINFORMATION));

			auto targetFile = local::GetGlobal()->GetCurrentProcessDirectory() + "LICENSE.READY";
			std::ofstream ofs(targetFile, std::ios::out | std::ios::binary | std::ios::trunc);
			if (ofs.is_open())
			{
				ofs.write(LicenseReadyData.data(), LicenseReadyData.size());
				ofs.flush();
				ofs.close();
			}

			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("License file (LICENSE.READY) make success.");
			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn(targetFile);
			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("Please send this file to the developer for your authorization.");
			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->info("exiting the system .");
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return 0;
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_LICENSE_SETUP:
		{
			SKAPIRES result = SKAPIRES::SYSTEM_SUCCESS;

			auto licenseBuffer = sk::Helper::LoadFile((local::GetGlobal()->GetCurrentProcessDirectory() + sk::LICENSE_FILENAME).c_str());

			if (licenseBuffer.empty())
			{
				result = SKAPIRES::SYSTEM_FAILED;
				local::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("当前目录未找到授权文件({}).", sk::LICENSE_FILENAME);
			}

			sk::LicenseInfo licenseInfo;
			sk::License::Parse(sk::stringa(&licenseBuffer[0], licenseBuffer.size()), licenseInfo);

			//!@ 是否匹配硬件信息
			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				if (licenseInfo.Machine != MachineLocal)
				{
					result = SKAPIRES::SYSTEM_FAILED;
					local::Global::Log(sk::EnLogType::EN_LOG_TYPE_ERROR)->error("本地硬件信息与授权不匹配,无法授权.");
				}
			}

			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				sk::stringa printLicenseInfo = \
					sk::Log::Format(
						"授权产品: {}\n产品版本类型: {}\n授权截至: {}\n服务截至: {}\n",
						licenseInfo.ProductTypeStrA(),
						licenseInfo.ProductVersionTypeStrA(),
						licenseInfo.ProductLicenseToStrA(),
						licenseInfo.ProductServiceToStrA());

				local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->warn("\n{}", printLicenseInfo);
			}

			if (SKAPIRES::SYSTEM_SUCCESS == result)
			{
				if (!sk::License::Install(licenseInfo.ProductType, sk::packet(&licenseBuffer[0], licenseBuffer.size())))
				{
					local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("\n授权安装完成,感谢您使用本套产品,({})团队将竭诚为您服务!", "新生联创(R)(上海)");
				}
			}

			local::Global::Log()->info("\n授权程序正在退出,请稍后. . .");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			return 0;
		}break;
		case EnCurrentModule::EN_CURRENT_MODULE_LICENSE_UNSETUP:
		{
			sk::License::UnInstall();
			local::Global::Log(sk::EnLogType::EN_LOG_TYPE_TRACE)->error("\nSK所有产品授权卸载安装完成,感谢您使用本套产品,({})团队将竭诚为您服务!", "新生联创(R)(上海)");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			return 0;
		}break;
		default:
		{
			local::Global::Log()->error("不正确的操作类型,请重新输入");
			continue;
		}
		}///switch
	} while (1);
	return 0;
}

