#include "stdafx.h"
#include "exports.h"

namespace feeder {

	__skapi_ int __stdcall api_object_init(const sk::intptr& obj_api_ptr)
	{
		if (nullptr == global_Feeder)
		{
			global_Feeder = new Feeder();
			return 0;
		}
		return -1;
	}
	__skapi_ int __stdcall api_object_uninit()
	{
		SK_DELETE_PTR(global_Feeder);
		return 0;
	}

	__skapi_ int __stdcall api_feeder_plugin_open()
	{
		FeederGet()->Start(nullptr);
		return 0;
	}
	__skapi_ int __stdcall api_feeder_plugin_close()
	{
		FeederGet()->Stop();
		return 0;
	}

}///namespace gateway