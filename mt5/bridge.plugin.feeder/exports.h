#if !defined(AFX_SKSTU_EXPORTS_H__9F16D_6ADC584F_8B3F8C5E1BDFF8F_120C__HEAD__)
#define AFX_SKSTU_EXPORTS_H__9F16D_6ADC584F_8B3F8C5E1BDFF8F_120C__HEAD__

#if defined(MTPLUGIN_EXPORTS)
#define __skapi_ extern "C" __declspec(dllexport)
#else
#define __skapi_ extern "C" __declspec(dllimport)
#endif

namespace feeder {

	__skapi_ int __stdcall api_object_init(const sk::intptr& obj_api_ptr);
	__skapi_ int __stdcall api_object_uninit();

	__skapi_ int __stdcall api_feeder_plugin_open();
	__skapi_ int __stdcall api_feeder_plugin_close();

}///namespace feeder



/// /*新生联创（上海）**/
/// /*2019年09月11日 3:4:18|143**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_EXPORTS_H__9F16D_6ADC584F_8B3F8C5E1BDFF8F_120C__TAIL__