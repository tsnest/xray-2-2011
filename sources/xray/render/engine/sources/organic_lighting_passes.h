////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_ORGANIC_LIGHTING_PASSES_H_INCLUDED
#define XRAY_RENDER_ENGINE_ORGANIC_LIGHTING_PASSES_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

enum enum_organic_lighting_pass
{
	organic_lighting_position_pass = 0,
	organic_lighting_lighting_pass,
	organic_lighting_blurring_pass,
	organic_lighting_combine_pass,
}; // enum enum_organic_lighting_pass

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_ORGANIC_LIGHTING_PASSES_H_INCLUDED

