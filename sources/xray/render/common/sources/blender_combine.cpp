////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_combine.h"

namespace xray {
namespace render {


void jitter(blender_compiler& compiler)
{
	compiler.def_sampler("jitter0",	JITTER(0), D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT)
			.def_sampler("jitter1",	JITTER(1), D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT)
			.def_sampler("jitter2",	JITTER(2), D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT)
			.def_sampler("jitter3",	JITTER(3), D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
}

void blender_combine::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETER	( options );

	shader_defines_list	defines;
	make_defines(defines);

	compiler.begin_technique(/*SE_R2_SHADOW*/)
		.begin_pass("combine_1", "combine_1", defines)
			.set_z_test(FALSE)
			.set_alpha_blend(TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA)
			.def_sampler_rtf("s_position",    r2_rt_p)
			.def_sampler_rtf("s_normal",      r2_rt_n)
			.def_sampler_rtf("s_diffuse",     r2_rt_albedo)
			.def_sampler_rtf("s_accumulator", r2_rt_accum)
			//.def_sampler_rtf("s_depth",   r2_rt_depth)
			//.def_sampler_rtf("s_tonemap", r2_rt_luminance_cur)
			//.def_sampler("s_mat_params",  r2_mat_parms, false, D3DTADDRESS_WRAP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR)
			.def_sampler_clw("s_material",	r2_material)
			//.def_sampler_clw("s_occ",     r2_rt_ssao_temp)
			//.def_sampler_clf("env_s0",    r2_t_envs0)
			//.def_sampler_clf("env_s1",    r2_t_envs1)
			.def_sampler_clf("env_s0",		"sky/sky_9_cube#small")
			.def_sampler_clf("env_s1",		"sky/sky_19_cube#small");
			//.def_sampler_clf("sky_s0",    r2_t_sky0)
			//.def_sampler_clf("sky_s1",    r2_t_sky1);
			jitter(compiler);
		compiler.end_pass()
	.end_technique();

	//IBlender::Compile		(C);

	//switch (C.iElement)
	//{
	//case 0:	// combine
	//	C.r_Pass			("combine_1",		"combine_1",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);	//. MRT-blend?
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P				);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N				);
	//	C.r_Sampler_rtf		("s_diffuse",		r2_RT_albedo		);
	//	C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum			);
	//	C.r_Sampler_rtf		("s_depth",			r2_RT_depth			);
	//	C.r_Sampler_rtf		("s_tonemap",		r2_RT_luminance_cur	);
	//	C.r_Sampler	("s_mat_params", r2_mat_parms, false, D3DTADDRESS_WRAP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
	//	C.r_Sampler_clw		("s_material",		r2_material			);
	//	C.r_Sampler_clw		("s_occ",			r2_RT_ssao_temp		);
	//	C.r_Sampler_clf		("env_s0",			r2_T_envs0			);
	//	C.r_Sampler_clf		("env_s1",			r2_T_envs1			);
	//	C.r_Sampler_clf		("sky_s0",			r2_T_sky0			);
	//	C.r_Sampler_clf		("sky_s1",			r2_T_sky1			);
	//	jitter(C);
	//	C.r_End				();
	//	break;
	//case 1:	// aa-edge-detection + AA :)
	//	C.r_Pass			("null",			"combine_2_AA",		FALSE,	FALSE,	FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	//	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	//	C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
	//	C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
	//	C.r_End				();
	//	break;
	//case 2:	// non-AA
	//	C.r_Pass			("null",			"combine_2_NAA",	FALSE,	FALSE,	FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	//	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	//	C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
	//	C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
	//	C.r_End				();
	//	break;
	//case 3:	// aa-edge-detection + AA :) + DISTORTION
	//	C.r_Pass			("null",			"combine_2_AA_D",	FALSE,	FALSE,	FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	//	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	//	C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
	//	C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
	//	C.r_End				();
	//	break;
	//case 4:	// non-AA + DISTORTION
	//	C.r_Pass			("null",			"combine_2_NAA_D",	FALSE,	FALSE,	FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	//	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	//	C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
	//	C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
	//	C.r_End				();
	//	break;
	//case 5:	// post-processing
	//	break;
	//}
}

} // namespace render 
} // namespace xray 
