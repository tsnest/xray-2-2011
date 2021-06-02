////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_deffer_aref.h"

namespace xray {
namespace render {


blender_deffer_aref::blender_deffer_aref(bool is_lmapped): blender_deffer_base(false, false, false)
{
	m_aref_val = 200;
	m_blend = FALSE;
	m_desc.m_version = 1;
	m_is_lmapped = is_lmapped;
}

void blender_deffer_aref::compile_blended(blender_compiler& compiler, const blender_compilation_options& options)
{
	shader_defines_list	defines;
	make_defines(defines);

	if (m_is_lmapped)
	{
		compiler.begin_pass("lmapE", "lmapE", defines)
			//.set_z_test(TRUE)
			.set_alpha_blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA)
			.set_alpha_test(TRUE, m_aref_val)
			.def_sampler("s_base", options.tex_list[0])
			.def_sampler("s_lmap", options.tex_list[1])
			.def_sampler_clf("s_hemi", options.tex_list[2])
			.def_sampler("s_env", r2_t_envs0, false, D3DTADDRESS_CLAMP)
		.end_pass();
		//C.r_Pass			("lmapE","lmapE",TRUE,TRUE,FALSE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE, oAREF.value);
		//C.r_Sampler			("s_base",	C.L_textures[0]	);
		//C.r_Sampler			("s_lmap",	C.L_textures[1]	);
		//C.r_Sampler_clf		("s_hemi",	*C.L_textures[2]);
		//C.r_Sampler			("s_env",	r2_T_envs0,		false,D3DTADDRESS_CLAMP);
		//C.r_End				();
	}
	else
	{
		compiler.begin_pass("vert", "vert", defines)
			//.set_z_test(TRUE)
			.set_alpha_blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA)
			.set_alpha_test(TRUE, m_aref_val)
			.def_sampler("s_base", options.tex_list[0])
		.end_pass();
		//C.r_Pass			("vert", "vert", TRUE,TRUE,FALSE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE, oAREF.value);
		//C.r_Sampler			("s_base",	C.L_textures[0]	);
		//C.r_End				();
	}
}

void blender_deffer_aref::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	if (m_blend)
	{
		compiler.begin_technique(/*SE_R2_NORMAL_HQ*/);
		compile_blended(compiler, options);
		compiler.end_technique();

		compiler.begin_technique(/*SE_R2_NORMAL_LQ*/);
		compile_blended(compiler, options);
		compiler.end_technique();
	}
	else
	{
		shader_defines_list	defines;
		make_defines(defines);

		//C.SetParams				(1,false);	//.

		// codepath is the same, only the shaders differ
		// ***only pixel shaders differ***
		compiler.begin_technique(/*SE_R2_NORMAL_HQ*/);
		uber_deffer(compiler, "base", "base", true, true, options);
		compiler.end_technique();

		compiler.begin_technique(/*SE_R2_NORMAL_LQ*/);
		uber_deffer(compiler, "base", "base", false, true, options);
		compiler.end_technique();

		compiler.begin_technique(/*SE_R2_SHADOW*/);
		//if (RImplementation.o.HW_smap)
		//	compiler.begin_pass("shadow_direct_base_aref","shadow_direct_base_aref");
		//else
		compiler.begin_pass("shadow_direct_base_aref", "shadow_direct_base_aref", defines)
				.set_z_test(TRUE)
				.set_alpha_blend(FALSE)
				.set_alpha_test(TRUE, 220)
				.def_sampler("s_base", options.tex_list[0])
			.end_pass()
		.end_technique();
	}
}

void blender_deffer_aref::load(memory::reader& mem_reader)
{
	blender::load(mem_reader);

	if (1==m_desc.m_version)
	{
		xrP_Integer	aref_val;
		xrPREAD_PROP(mem_reader, xrPID_INTEGER, aref_val);
		m_aref_val = aref_val.value;
		xrP_BOOL blend;
		xrPREAD_PROP(mem_reader, xrPID_BOOL, blend);
		m_blend = blend.value != 0;
	}
}

} // namespace render 
} // namespace xray 
