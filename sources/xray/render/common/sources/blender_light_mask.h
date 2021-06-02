////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_LIGHT_MASK_H_INCLUDED
#define BLENDER_LIGHT_MASK_H_INCLUDED

namespace xray {
namespace render{

class blender_light_mask: public blender
{
public:
	enum
	{
		tech_mask_spot,
		tech_mask_point,
		tech_mask_direct,
		tech_mask_stencil
	};

public:
	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
}; // class blender_light_mask

} // namespace xray
} // namespace render

#endif // #ifndef BLENDER_LIGHT_MASK_H_INCLUDED