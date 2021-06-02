////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_terrain.h>
#include <xray/render/core/effect_compiler.h>


namespace xray {
namespace render_dx10 {

effect_terrain::effect_terrain(): effect_deffer_base( true, true, true)
{
	m_blend = true;
	m_desc.m_version = 1;
	m_not_an_tree = false;
}

void effect_terrain::compile( effect_compiler& compiler, const effect_compilation_options& options)
{
		XRAY_UNREFERENCED_PARAMETER	( options);

		shader_defines_list	defines;
		make_defines( defines);

		compiler.begin_technique( /*LEVEL*/);
		compiler.begin_pass			( "terrain", "terrain", defines)
				.set_depth			( true, true)
				.set_stencil		( true, 0x01, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
				.set_alpha_blend	( false)
				.set_texture		( "t_layer0", options.tex_list[0])
				.set_texture		( "t_layer1", options.tex_list[1])
				.set_texture		( "t_layer2", options.tex_list[2])
			.end_pass()
		.end_technique();

		return;
}

void effect_terrain::load( memory::reader& mem_reader)
{
	effect::load( mem_reader);

	if ( 1==m_desc.m_version)
	{
		xrP_BOOL prop_bool;
		xrPREAD_PROP( mem_reader, xrPID_BOOL, prop_bool);
		m_blend = prop_bool.value != 0;

		xrPREAD_PROP( mem_reader, xrPID_BOOL, prop_bool);
		m_not_an_tree = prop_bool.value != 0;
	}
}

} // namespace render
} // namespace xray
