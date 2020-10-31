#include "stdafx.h"
#include "exports.h"

namespace follow {

	__skapi_ int __stdcall api_object_init(const sk::intptr& obj_api_ptr)
	{
		follow::static_global_Follow = new follow::Follow();
		return 0;
	}

	__skapi_ int __stdcall api_object_uninit()
	{
		if (follow::static_global_Follow) 
		{
			SK_DELETE_PTR(follow::static_global_Follow);
		}
		return 0;
	}

	__skapi_ int __stdcall api_follow_plugin_open() 
	{
		follow::static_global_Follow->Start(nullptr);
		return 0;
	}

	__skapi_ int __stdcall api_follow_plugin_close() 
	{
		follow::static_global_Follow->Stop();
		return 0;
	}

}///namespace follow