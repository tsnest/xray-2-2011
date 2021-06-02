////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_FLAT_H_INCLUDED
#define BLENDER_DEFFER_FLAT_H_INCLUDED

#include <xray/render/engine/effect_deffer_base.h>

namespace xray {
namespace render_dx10 {


class effect_deffer: public effect_deffer_base
{
public:
	effect_deffer();

	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:

}; // class effect_deffer_flat

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_DEFFER_FLAT_H_INCLUDED