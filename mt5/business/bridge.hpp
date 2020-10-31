#if !defined(AFX_SK_H__684A73A7_AD71_444A_BD17_41774914666D__HEAD__)
#define AFX_SK_H__684A73A7_AD71_444A_BD17_41774914666D__HEAD__


#define BRIDGE_ENABLE_DISKLOG 1

#define PLUGIN_PARAM_KEY_PATH_OBJ						L"PathObj"
#define PLUGIN_PARAM_KEY_ROUTE_RULE_NAME		L"RouteRule Name"
#define PLUGIN_PARAM_KEY_DEALER_MANAGER_LOGIN				L"Dealer Manager Login"
#define PLUGIN_PARAM_KEY_DEALER_GATEWAY_NAME			L"Dealer Gateway Name"

#define MTRelease(obj) if(obj) {obj->Release(); obj=nullptr;}

namespace bridge {
}///namespace bridge


typedef unsigned int MTRequestID;
typedef unsigned long long MTOrderID;
typedef unsigned long long MTVolume;
typedef unsigned long long MTPositionID;
typedef double MTPrice;
typedef double LPPrice;
typedef double LPVolume;






///!@ 新生联创®（上海）
///!@ Sun Jun 7 08:47:18 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__684A73A7_AD71_444A_BD17_41774914666D__HEAD__
