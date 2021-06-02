////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_deffer.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render_dx10 {


effect_deffer::effect_deffer(): effect_deffer_base(false, false, false)
{
	m_desc.m_version = 1;
}

void effect_deffer::compile(effect_compiler& compiler, const effect_compilation_options& options)
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
			//.set_depth(TRUE)
			//.set_alpha_blend(FALSE)
			.set_stencil( true, 0x01, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_texture( "t_base", options.tex_list[0].c_str())
		.end_pass()
	.end_technique();

}

void effect_deffer::load(memory::reader& mem_reader)
{
	effect::load(mem_reader);
}

} // namespace render 
} // namespace xray 
