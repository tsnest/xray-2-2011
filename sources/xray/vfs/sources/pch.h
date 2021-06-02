////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_VFS_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"vfs"
#include <xray/extensions.h>

#endif // #ifndef PCH_H_INCLUDED