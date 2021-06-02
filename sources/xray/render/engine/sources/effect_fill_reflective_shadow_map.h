////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_FILL_REFLECTIVE_SHADOW_MAP_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_FILL_REFLECTIVE_SHADOW_MAP_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_fill_reflective_shadow_map: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_fill_reflective_shadow_map

} // namespace xray
} // namespace render

#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_FILL_REFLECTIVE_SHADOW_MAP_H_INCLUDED