////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_FLAT_H_INCLUDED
#	define BLENDER_DEFFER_FLAT_H_INCLUDED

#	include "blender_deffer_base.h"

namespace xray {
namespace render {


class blender_deffer: public blender_deffer_base
{
public:
	blender_deffer();

	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:

}; // class blender_deffer_flat

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_DEFFER_FLAT_H_INCLUDED