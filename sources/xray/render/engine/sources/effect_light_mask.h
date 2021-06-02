////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_LIGHT_MASK_H_INCLUDED
#define BLENDER_LIGHT_MASK_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_light_mask: public effect_descriptor
{
public:
	enum
	{
		tech_mask_local_light,
		tech_mask_direct,
		tech_mask_direct_single_material,
	};

public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_light_mask

} // namespace xray
} // namespace render

#endif // #ifndef BLENDER_LIGHT_MASK_H_INCLUDED