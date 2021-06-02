////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"render"

#include <xray/extensions.h>

#define XRAY_RENDER_CORE_BUILDING

#include <xray/render/core/render_include.h>
#include <xray/render/core/memory.h>
#include <xray/render/core/macros_extensions.h>

#endif // #ifndef PCH_H_INCLUDED