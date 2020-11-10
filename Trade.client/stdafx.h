#if !defined(AFX_SKSTU_1604538404_H__B6471_8F13EEEE_2C666A3974EF602_73EF__HEAD__)
#define AFX_SKSTU_1604538404_H__B6471_8F13EEEE_2C666A3974EF602_73EF__HEAD__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "common/common.h"
#include "common/spdlog.hpp"
#include "common/dump.hpp"
#include "common/timer.hpp"

#define DRIVER_GLUT_GL2
#include "common/imgui.hpp"
#if !defined(DRIVER_GLUT_GL2)
#include "common/glut.hpp"
#endif

#include "common/duilib.hpp"

#include "shared/shared.hpp"
#include "shared/shared.network/dllexports.hpp"
#include "shared/shared.resource/dllexports.h"
#include "common/window.h"

#if ENABLE_CEF
#include "common/libcef.hpp"
#include "CefHelper.h"
#endif


#include "ChildDraw.h"
#include "ChildWeb.h"
#include "ChildWork.h"
#include "Frame.h"
#include "Window.h"
#include "Global.h"


/// /*新生联创（上海）**/
/// /*2020年11月5日 9:6:48|493**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1604538404_H__B6471_8F13EEEE_2C666A3974EF602_73EF__TAIL__