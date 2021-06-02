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

#define XRAY_LOG_MODULE_INITIATOR	"render_dx9"
#include <xray/extensions.h>

#define XRAY_RENDER_BUILDING
#include <xray/render/api.h>

#include <xray/render/dx9/render_include.h>

#include <xray/render/base/memory.h>


using xray::math::float2;
using xray::math::float3;
using xray::math::color;
using xray::math::aabb;
using xray::math::int2;
using xray::math::uint2;

using namespace xray;

using xray::render::vector;
using xray::render::map;
//using xray::render::fixed_vector;

#include <xray/render/common/sources/d3d_defs.h>
#include <xray/render/dx9/dx9_include.h>

//#include "visual.h"
#include "render_queue.h"
//#include "model_manager.h"

#endif // #ifndef PCH_H_INCLUDED