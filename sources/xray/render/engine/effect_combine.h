////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_COMBINE_H_INCLUDED
#define BLENDER_COMBINE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render_dx10 {


class effect_combine: public effect
{
public:
	effect_combine();
	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
}; // class effect_combine

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_COMBINE_H_INCLUDED