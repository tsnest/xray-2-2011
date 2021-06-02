#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_UI_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"ui"
#include <xray/extensions.h>

//#include "ui_base.h"
//#include <xray/ui/api.h>
//#include <xray/ui/ui.h>

using namespace xray;
//using namespace xray::ui;

#endif // #ifndef PCH_H_INCLUDED