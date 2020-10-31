#include "stdafx.h"
#include "exports.h"

namespace dealer {

	__skapi_ int __stdcall api_object_init(const sk::intptr& obj_api_ptr)
	{
		if (nullptr == global_Dealer)
		{
			global_Dealer = new Dealer();
			return 0;
		}
		return -1;
	}
	__skapi_ int __stdcall api_object_uninit()
	{
		SK_DELETE_PTR(global_Dealer);
		return 0;
	}

	__skapi_ int __stdcall api_dealer_plugin_open()
	{
		DealerGet()->Start(nullptr);
		return 0;
	}
	__skapi_ int __stdcall api_dealer_plugin_close()
	{
		DealerGet()->Stop();
		return 0;
	}

}///namespace gateway