////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_LIGHT_DIRECT_H_INCLUDED
#define BLENDER_LIGHT_DIRECT_H_INCLUDED

namespace xray {
namespace render {


class blender_light_direct: public blender
{
public:
	enum
	{
		tech_sun_near,
		tech_sun_far
	};

public:
	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
}; // class blender_light_direct

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_LIGHT_DIRECT_H_INCLUDED