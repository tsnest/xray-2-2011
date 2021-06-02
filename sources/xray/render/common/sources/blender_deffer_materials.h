////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_MATERIALS_H_INCLUDED
#define BLENDER_DEFFER_MATERIALS_H_INCLUDED

#include "blender_deffer_base.h"


namespace xray {
namespace render {


class blender_deffer_materials: public blender_deffer_base
{

public:
	blender_deffer_materials( u32 material_index);

	virtual void compile( blender_compiler& compiler, const blender_compilation_options& options);
	virtual	void load	( memory::reader& mem_reader);

private:
	u32 m_material_ind;

}; // class blender_deffer_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_DEFFER_MATERIALS_H_INCLUDED