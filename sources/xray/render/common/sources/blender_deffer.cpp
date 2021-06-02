////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_deffer.h"

namespace xray {
namespace render {


blender_deffer::blender_deffer(): blender_deffer_base(false, false, false)
{
	m_desc.m_version = 1;
}

void blender_deffer::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	compiler.begin_technique(/*SE_R2_NORMAL_HQ*/);
	uber_deffer(compiler, "base", "base", true, false, options);
	compiler.end_technique();
	
	compiler.begin_technique(/*SE_R2_NORMAL_LQ*/);
	uber_deffer(compiler, "base", "base", false, false, options);
	compiler.end_technique();
	
	shader_defines_list	defines;
	make_defines(defines);

	compiler.begin_technique(/*SE_R2_SHADOW*/);
	//if (RImplementation.o.HW_smap)
	//	compiler.begin_pass("shadow_direct_base","dumb");
	//else
		compiler.begin_pass("shadow_direct_base", "null", defines)
			//.set_z_test(TRUE)
			//.set_alpha_blend(FALSE)
			.set_stencil(TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP)
			.def_sampler("s_base", options.tex_list[0])
		.end_pass()
	.end_technique();
}

void blender_deffer::load(memory::reader& mem_reader)
{
	blender::load(mem_reader);
}

} // namespace render 
} // namespace xray 
