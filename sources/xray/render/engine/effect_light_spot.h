////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_LIGHT_SPOT_H_INCLUDED
#define BLENDER_LIGHT_SPOT_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>
namespace xray {
namespace render_dx10 {

class effect_light_spot: public effect
{
public:
	enum
	{
		//tech_fill,
		tech_unshadowed
	};
private:
	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);

}; // class effect_light_spot

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_LIGHT_SPOT_H_INCLUDED