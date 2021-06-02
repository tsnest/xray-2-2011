////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_DOWNSAMPLE_REFLECTIVE_SHADOW_MAP_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_DOWNSAMPLE_REFLECTIVE_SHADOW_MAP_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_downsample_reflective_shadow_map: public effect_descriptor
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_downsample_reflective_shadow_map

} // namespace xray
} // namespace render

#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_DOWNSAMPLE_REFLECTIVE_SHADOW_MAP_H_INCLUDED