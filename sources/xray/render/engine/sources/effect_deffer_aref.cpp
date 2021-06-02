////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_deffer_aref.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render_dx10 {


effect_deffer_aref::effect_deffer_aref( bool is_lmapped): 
effect_deffer_base( false, false, false)
{
	m_blend = FALSE;
	m_desc.m_version = 1;
	m_is_lmapped = is_lmapped;
}

void effect_deffer_aref::compile_blended( effect_compiler& compiler, const effect_compilation_options& options)
{
	shader_defines_list	defines;
	make_defines( defines);

	if ( m_is_lmapped)
	{
		compiler.begin_pass( "lmapE", "lmapE", defines)
			//.set_depth( TRUE)
			.set_alpha_blend	( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.bind_constant		( "alpha_ref", &m_aref_val)
			.set_texture		( "t_base", options.tex_list[0])
			.def_sampler		( "s_lmap")
			.set_texture		( "t_lmap", options.tex_list[1])
			.def_sampler		( "s_hemi", D3D_TEXTURE_ADDRESS_CLAMP, D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT)
			.set_texture		( "t_hemi", options.tex_list[2])
			.def_sampler		( "s_env", D3D_TEXTURE_ADDRESS_CLAMP)
			.set_texture		( "t_env", r2_t_envs0)
		.end_pass();
		//C.r_Pass			( "lmapE","lmapE",TRUE,TRUE,FALSE,TRUE,D3D_BLEND_SRC_ALPHA,	D3D_BLEND_INV_SRC_ALPHA,	TRUE, oAREF.value);
		//C.r_Sampler			( "s_base",	C.L_textures[0]	);
		//C.r_Sampler			( "s_lmap",	C.L_textures[1]	);
		//C.r_Sampler_clf		( "s_hemi",	*C.L_textures[2]);
		//C.r_Sampler			( "s_env",	r2_T_envs0,		false,D3DTADDRESS_CLAMP);
		//C.r_End				();
	}
	else
	{
		compiler.begin_pass( "vert", "vert", defines)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.bind_constant	( "alpha_ref", &m_aref_val)
			.set_texture	( "t_base", options.tex_list[0])
		.end_pass();
		//C.r_Pass			( "vert", "vert", TRUE,TRUE,FALSE,TRUE,D3D_BLEND_SRC_ALPHA,	D3D_BLEND_INV_SRC_ALPHA,	TRUE, oAREF.value);
		//C.r_Sampler			( "s_base",	C.L_textures[0]	);
		//C.r_End				();
	}
}

void effect_deffer_aref::compile( effect_compiler& compiler, const effect_compilation_options& options)
{
	if ( m_blend)
	{
		compiler.begin_technique( /*SE_R2_NORMAL_HQ*/);
		compile_blended( compiler, options);
		compiler.end_technique();

		compiler.begin_technique( /*SE_R2_NORMAL_LQ*/);
		compile_blended( compiler, options);
		compiler.end_technique();
	}
	else
	{
		shader_defines_list	defines;
		make_defines( defines);

		//C.SetParams				( 1,false);	//.

		// codepath is the same, only the shaders differ
		// ***only pixel shaders differ***
		compiler.begin_technique( /*SE_R2_NORMAL_HQ*/);
		uber_deffer( compiler, "base", "base", true, true, options);
		compiler.end_technique();

		compiler.begin_technique( /*SE_R2_NORMAL_LQ*/);
		uber_deffer( compiler, "base", "base", false, true, options);
		compiler.end_technique();

		compiler.begin_technique( /*SE_R2_SHADOW*/);
		//if ( RImplementation.o.HW_smap)
		//	compiler.begin_pass( "shadow_direct_base_aref","shadow_direct_base_aref");
		//else
		compiler.begin_pass		( "shadow_direct_base_aref", "shadow_direct_base_aref", defines)
				.set_depth		( true, true)
				.set_alpha_blend( FALSE)
				.bind_constant	( "alpha_ref", &m_aref_val220)
				.set_texture	( "t_base", options.tex_list[0])
			.end_pass()
		.end_technique();
	}
}

void effect_deffer_aref::load( memory::reader& mem_reader)
{
	effect::load( mem_reader);

	if ( 1==m_desc.m_version)
	{
		xrP_Integer	aref_val;
		xrPREAD_PROP( mem_reader, xrPID_INTEGER, aref_val);
		//m_aref_val = aref_val.value;
		xrP_BOOL blend;
		xrPREAD_PROP( mem_reader, xrPID_BOOL, blend);
		m_blend = blend.value != 0;
	}
}

} // namespace render 
} // namespace xray 
