#include "stdafx.h"

MTPluginParam ExtPluginDefaults[] =
{
	{ MTPluginParam::TYPE_STRING,  PLUGIN_PARAM_KEY_PATH_OBJ   ,L"C:\\c.object.setup\\plugin.feeder\\"},
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: {
		feeder::global_hInstance = hModule;
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
	info.version = feeder::MT_PLUGIN_FEEDER_VERSION;
	info.version_api = MTServerAPIVersion;
	sk::SafeCopyW(info.name, L"FeederObj(SA)", _countof(info.name));
	sk::SafeCopyW(info.copyright, L"XSLC®", _countof(info.copyright));
	sk::SafeCopyW(info.description, L"Feeder Plugin For MetaTrader 5", _countof(info.description));

	memcpy(info.defaults, ExtPluginDefaults, sizeof(ExtPluginDefaults));
	info.defaults_total = _countof(ExtPluginDefaults);
	return MT_RET_OK;
}

MTAPIENTRY MTAPIRES MTServerCreate(UINT apiversion, IMTServerPlugin** plugin)
{
	if (!plugin) { return MT_RET_ERR_PARAMS; }
	if (nullptr != feeder::FeederGet()) { return MT_RET_ERR_PARAMS; }
	feeder::global_Feeder = new feeder::Feeder();
	if (!feeder::FeederGet()) { return MT_RET_ERR_MEM; }
	*plugin = feeder::FeederGet();
	return MT_RET_OK;
}

