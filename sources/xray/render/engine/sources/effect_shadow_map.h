////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_SHADOW_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_SHADOW_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_shadow_map: public effect_descriptor
{
public:
	enum
	{
		tech_mask_spot,
		tech_mask_point,
		tech_mask_direct,
		tech_mask_direct_single_material,
		tech_mask_stencil
	};

public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_light_mask

} // namespace xray
} // namespace render

#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_SHADOW_H_INCLUDED