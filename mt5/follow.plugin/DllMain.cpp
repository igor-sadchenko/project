#include "stdafx.h"

MTPluginParam ExtPluginDefaults[] =
{
	{ MTPluginParam::TYPE_STRING,  L"PathObj"   ,L"C:\\c.object.setup\\plugin.follow\\"},
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) 
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: {
		follow::global_hInstance = hModule;
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
	info.version = follow::MT_PLUGIN_FOLLOW_VERSION;
	info.version_api = MTServerAPIVersion;
	sk::SafeCopyW(info.name, L"FollowObj(SA)", _countof(info.name));
	sk::SafeCopyW(info.copyright, L"XSLC®", _countof(info.copyright));
	sk::SafeCopyW(info.description, L"Follow Plugin For MetaTrader 5", _countof(info.description));
	memcpy(info.defaults, ExtPluginDefaults, sizeof(ExtPluginDefaults));
	info.defaults_total = _countof(ExtPluginDefaults);
	return MT_RET_OK;
}

MTAPIENTRY MTAPIRES MTServerCreate(UINT apiversion, IMTServerPlugin** plugin)
{
	if (!plugin) { return MT_RET_ERR_PARAMS; }
	if (follow::static_global_Follow) { return MT_RET_ERR_PARAMS; }
	follow::static_global_Follow = new follow::Follow();
	if (((*plugin) = follow::static_global_Follow) == NULL) { return MT_RET_ERR_MEM; }
	return MT_RET_OK;
}

