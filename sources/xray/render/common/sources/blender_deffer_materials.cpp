////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2010
//	Author		: Armen Abroyan
//	Copyright ( C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_deffer_materials.h"

namespace xray {
namespace render {


blender_deffer_materials::blender_deffer_materials( u32 material_index): blender_deffer_base( false, false, false),
m_material_ind (material_index)
{
	m_desc.m_version = 1;
}

void blender_deffer_materials::compile( blender_compiler& compiler, const blender_compilation_options& options)
{
	m_stencil_marking = true;

	compiler.begin_technique( /*material int*/);
		m_stencil_marker = m_material_ind;
		uber_deffer( compiler, "base", "base", true, false, options);
	compiler.end_technique( );
}

void blender_deffer_materials::load( memory::reader& mem_reader)
{
	blender::load( mem_reader);
}

} // namespace render 
} // namespace xray 
