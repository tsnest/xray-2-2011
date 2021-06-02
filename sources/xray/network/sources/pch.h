////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_NETWORK_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_USE_STDIOSTREAM
#define XRAY_NO_SIGNALS_H

#include "stlport_config.h"

#include <xray/os_preinclude.h>
#undef NONET

#define XRAY_LOG_MODULE_INITIATOR	"network"
#include <xray/extensions.h>

#include "network_memory.h"

#if !XRAY_PLATFORM_PS3
#	define _WIN32_WINNT 0x0501
#	include "boost_api.h"
#endif // #if !XRAY_PLATFORM_PS3

#endif // #ifndef PCH_H_INCLUDED