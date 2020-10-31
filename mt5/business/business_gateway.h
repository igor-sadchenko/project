#if !defined(AFX_SKSTU_BUSINESS_GATEWAY_H__FE134_88B8BCEC_C3E6C3EEE7E3CBC_4F04__HEAD__)
#define AFX_SKSTU_BUSINESS_GATEWAY_H__FE134_88B8BCEC_C3E6C3EEE7E3CBC_4F04__HEAD__

#include "mt5api/Include/MT5APIGateway.h"
#include "mt5api/Include/MT5APITools.h"
#include "interface.h"

namespace shared {
	enum struct EnGatewayStatus : LONG
	{
		STATUS_API_SHUTDOWN = 0,
		STATUS_API_STOPPED = 1,                     // no connection
		STATUS_API_STARTING = 2,
		STATUS_API_CONFIGURED = 3,                     // Gateway settings received
		STATUS_API_STARTED = 4,                     // started
		STATUS_API_SYNCHRONIZED = 5,
		STATUS_API_FIRST = STATUS_API_SHUTDOWN,
		STATUS_API_LAST = STATUS_API_SYNCHRONIZED,
	};
}///namespace shared

/// /*新生联创（上海）**/
/// /*2019年10月27日 10:21:44|641**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_BUSINESS_GATEWAY_H__FE134_88B8BCEC_C3E6C3EEE7E3CBC_4F04__TAIL__