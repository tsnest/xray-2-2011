////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_deffer_materials.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render_dx10 {


effect_deffer_materials::effect_deffer_materials( u32 material_index): effect_deffer_base( false, false, false),
m_material_ind (material_index)
{
	m_desc.m_version = 1;
}

void effect_deffer_materials::compile( effect_compiler& compiler, const effect_compilation_options& options)
{
	m_stencil_marking = true;

	compiler.begin_technique( /*material int*/);
		m_stencil_marker = m_material_ind;
		uber_deffer( compiler, "base", "base", true, false, options);
	compiler.end_technique( );
}

void effect_deffer_materials::load( memory::reader& mem_reader)
{
	effect::load( mem_reader);
}

} // namespace render 
} // namespace xray 
