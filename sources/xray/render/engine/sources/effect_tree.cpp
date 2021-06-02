////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_tree.h>
#include <xray/render/core/effect_compiler.h>


namespace xray {
namespace render_dx10 {

effect_tree::effect_tree(): effect_deffer_base( true, true, true)
{
	m_blend = true;
	m_desc.m_version = 1;
	m_not_an_tree = false;
}

void effect_tree::compile( effect_compiler& compiler, const effect_compilation_options& options)
{
		XRAY_UNREFERENCED_PARAMETER	( options);

		shader_defines_list	defines;
		make_defines( defines);

		compiler.begin_technique( /*LEVEL*/);
		compiler.begin_pass( "test", "test", defines)
				.set_depth( true, true)
				.set_stencil( true, 0x01, 0xFF, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
					.set_alpha_blend( false)
				.set_texture( "t_base", options.tex_list[0])
			.end_pass()
		.end_technique();

		return;
#if 0	
	pcstr	tvs				= "tree";
	pcstr	tvs_s			= "shadow_direct_tree";

	if ( m_not_an_tree)
	{
		tvs="tree_s"; tvs_s="shadow_direct_tree_s";
	}

	//shader_defines_list	defines;
	//make_defines( defines);

	//C.SetParams				( 1,false);	//.

	// codepath is the same, only the shaders differ
	// ***only pixel shaders differ***
	compiler.begin_technique( /*SE_R2_NORMAL_HQ*/);
	uber_deffer( compiler, tvs, "base", true, m_blend, options);
	compiler.end_technique();

	compiler.begin_technique( /*SE_R2_NORMAL_LQ*/);
	uber_deffer( compiler, tvs, "base", false, m_blend, options);
	compiler.end_technique();

	compiler.begin_technique( /*SE_R2_SHADOW*/)
		.begin_pass( tvs_s, "shadow_direct_base_aref", defines);
			if ( m_blend) compiler.set_alpha_blend( TRUE, D3D_BLEND_ZERO, D3D_BLEND_ONE)
								 .set_alpha_test( TRUE, 200);
			compiler.def_sampler( "s_base", options.tex_list[0])
		.end_pass()
	.end_technique();

//	IBlender::Compile	( C);
//
//	//*************** codepath is the same, only shaders differ
//	LPCSTR	tvs				= "tree";
//	LPCSTR	tvs_s			= "shadow_direct_tree";
//	if ( oNotAnTree.value)	{ tvs="tree_s"; tvs_s="shadow_direct_tree_s"; }
//	switch ( C.iElement)
//	{
//	case SE_R2_NORMAL_HQ:	// deffer
//		uber_deffer			( C,true,tvs,"base",oBlend.value);
//		break;
//	case SE_R2_NORMAL_LQ:	// deffer
//		uber_deffer			( C,false,tvs,"base",oBlend.value);
//		break;
//	case SE_R2_SHADOW:		// smap-spot
////	TODO: DX10: Use dumb shader for shadowmap since shadows are drawn using hardware PCF
//		if ( oBlend.value)	C.r_Pass	( tvs_s,"shadow_direct_base_aref",	FALSE,TRUE,TRUE,TRUE,D3D_BLEND_ZERO,D3D_BLEND_ONE,TRUE,200);
//		else				C.r_Pass	( tvs_s,"shadow_direct_base",		FALSE);
//		C.r_Sampler			( "s_base",	C.L_textures[0]);
//		C.r_End				();
//		break;
//	}
#endif // #if 0
}

void effect_tree::load( memory::reader& mem_reader)
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
