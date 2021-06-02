////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_LIGHT_DIRECT_H_INCLUDED
#define BLENDER_LIGHT_DIRECT_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render_dx10 {


class effect_light_direct: public effect
{
public:
	enum
	{
		tech_sun_near,
		tech_sun_far
	};

public:
	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
}; // class effect_light_direct

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_LIGHT_DIRECT_H_INCLUDED