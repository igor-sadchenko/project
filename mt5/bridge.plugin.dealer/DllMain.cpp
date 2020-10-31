#include "stdafx.h"

MTPluginParam ExtPluginDefaults[] =
{
	{ MTPluginParam::TYPE_STRING,  PLUGIN_PARAM_KEY_PATH_OBJ   ,L"C:\\sk.project.setup\\plugin.dealer.obj\\"},
	{ MTPluginParam::TYPE_STRING,  PLUGIN_PARAM_KEY_ROUTE_RULE_NAME  ,L"RouteRuleSK"},
	{ MTPluginParam::TYPE_STRING,  PLUGIN_PARAM_KEY_DEALER_MANAGER_LOGIN  ,L"9999"},
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: {
		dealer::global_hInstance = hModule;
	}break;
	case DLL_THREAD_ATTACH: {
	}break;
	case DLL_THREAD_DETACH: {
	}break;
	case DLL_PROCESS_DETACH: {
	}break;
	}
	return TRUE;
}

MTAPIENTRY MTAPIRES MTServerAbout(MTPluginInfo& info)
{
	info.version = dealer::MT_PLUGIN_DEALER_VERSION;
	info.version_api = MTServerAPIVersion;
	sk::SafeCopyW(info.name, L"DealerObj(SA)", _countof(info.name));
	sk::SafeCopyW(info.copyright, L"XSLC®", _countof(info.copyright));
	sk::SafeCopyW(info.description, L"Dealer Plugin For MetaTrader 5", _countof(info.description));

	memcpy(info.defaults, ExtPluginDefaults, sizeof(ExtPluginDefaults));
	info.defaults_total = _countof(ExtPluginDefaults);
	return MT_RET_OK;
}

MTAPIENTRY MTAPIRES MTServerCreate(UINT apiversion, IMTServerPlugin** plugin)
{
	if (!plugin) { return MT_RET_ERR_PARAMS; }
	if (nullptr != dealer::DealerGet()) { return MT_RET_ERR_PARAMS; }
	dealer::global_Dealer = new dealer::Dealer();
	if (!dealer::DealerGet()) { return MT_RET_ERR_MEM; }
	*plugin = dealer::DealerGet();
	return MT_RET_OK;
}

