////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_terrain.h"

namespace xray {
namespace render{

blender_terrain::blender_terrain(): blender_deffer_base(true, true, true)
{
	m_blend = true;
	m_desc.m_version = 1;
	m_not_an_tree = false;
}

void blender_terrain::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*LEVEL*/);
		compiler.begin_pass			("terrain", "terrain", defines)
				.set_z_test			(TRUE)
				.set_stencil		(TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP)
				.set_alpha_blend	(FALSE)
				.def_sampler		("s_layer0", options.tex_list[0])
				.def_sampler		("s_layer1", options.tex_list[1])
				.def_sampler		("s_layer2", options.tex_list[2])
			.end_pass()
		.end_technique();

		return;
}

void blender_terrain::load(memory::reader& mem_reader)
{
	blender::load(mem_reader);

	if (1==m_desc.m_version)
	{
		xrP_BOOL prop_bool;
		xrPREAD_PROP(mem_reader, xrPID_BOOL, prop_bool);
		m_blend = prop_bool.value != 0;

		xrPREAD_PROP(mem_reader, xrPID_BOOL, prop_bool);
		m_not_an_tree = prop_bool.value != 0;
	}
}

} // namespace render
} // namespace xray
