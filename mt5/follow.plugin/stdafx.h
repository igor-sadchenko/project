#if !defined(AFX_SKSTU_STDAFX_H__2C2F5_6C77CD82_852DE38FE14A5C3_8DCB__HEAD__)
#define AFX_SKSTU_STDAFX_H__2C2F5_6C77CD82_852DE38FE14A5C3_8DCB__HEAD__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "common/common.h"
#include "common/spdlog.hpp"
#include "common/timer.hpp"
#include "common/license.hpp"

#include "gpl/rapidxml/rapidxml.hpp"
#include "gpl/rapidxml/rapidxml_iterators.hpp"
#include "gpl/rapidxml/rapidxml_print.hpp"
#include "gpl/rapidxml/rapidxml_utils.hpp"

#include "shared/shared.hpp"
#include "shared/shared.network/dllexports.hpp"

#include "../business/business_server_plugin.h"
#include "../business/tradefollowfix.hpp"
#include "../business/mmapfile.hpp"
#include "../business/dealer_dispatch.hpp"
#include "../business/disklog.hpp"


#include "Define.h"
#include "Setup.h"

#include "RouteObj.h"
//! Mate Quotes 5 Follow Objcet
#include "FollowObj.h"

#include "Shared.h"
#include "ServiceObj.h"
#include "Server.h"
#include "Follow.h"

#define ENABLE_FOLLOW_PROCESS_TYPE_ONDEALPERFORM 0
#define ENABLE_FOLLOW_DEBUG_LOG 1

/// /*新生联创（上海）**/
/// /*2019年07月29日 8:46:09|579**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_STDAFX_H__2C2F5_6C77CD82_852DE38FE14A5C3_8DCB__TAIL__