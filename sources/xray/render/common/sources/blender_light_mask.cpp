////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_light_mask.h"

namespace xray {
namespace render{

void blender_light_mask::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETER	( options );
	shader_defines_list	defines;
	make_defines(defines);

	compiler.begin_technique(/*SE_MASK_SPOT*/)
		.begin_pass("accum_mask", "dumb", defines)
			.set_z_test(FALSE, D3DCMP_GREATEREQUAL)
			.def_sampler_rtf("s_position",    r2_rt_p)
		.end_pass()
	.end_technique();

	compiler.begin_technique(/*SE_MASK_POINT*/)
		.begin_pass("accum_mask", "dumb", defines)
			.set_z_test(FALSE, D3DCMP_GREATEREQUAL)
			//.set_z_test(FALSE, D3DCMP_GREATEREQUAL)
			.def_sampler_rtf("s_position",    r2_rt_p)
		.end_pass()
	.end_technique();

	compiler.begin_technique(/*SE_MASK_DIRECT*/)
		.begin_pass("stub_notransform_t", "accum_sun_mask", defines)
			.set_z_test(FALSE)
			.set_alpha_blend(TRUE, D3DBLEND_ZERO, D3DBLEND_ONE)
			.set_alpha_test(TRUE,1)
			.def_sampler_rtf("s_normal",      r2_rt_n)
		.end_pass()
	.end_technique();

	compiler.begin_technique(/*SE_MASK_STENCIL*/)
		.begin_pass("stub_notransform_t", "dumb", defines)
		.end_pass()
	.end_technique();
	//switch (C.iElement) 
	//{
	//case SE_MASK_SPOT:		// spot or omni-part
	//	C.r_Pass			("accum_mask",		"dumb",				false,	TRUE,FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_End				();
	//	break;
	//case SE_MASK_POINT:		// point
	//	C.r_Pass			("accum_mask",		"dumb",				false,	TRUE,FALSE);
	//	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	//	C.r_End				();
	//	break;
	//case SE_MASK_DIRECT:	// stencil mask for directional light
	//	C.r_Pass			("null",			"accum_sun_mask",	false,	FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,1);
	//	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	//	C.r_End				();
	//	break;
	//case SE_MASK_ACCUM_VOL:	// copy accumulator (temp -> real), volumetric (usually after blend)
	//	C.r_Pass			("accum_volume",	"copy_p",			false,	FALSE,FALSE);
	//	C.r_Sampler_rtf		("s_base",			r2_RT_accum_temp	);
	//	C.r_End				();
	//	break;
	//case SE_MASK_ACCUM_2D:	// copy accumulator (temp -> real), 2D (usually after sun-blend)
	//	C.r_Pass			("null",			"copy",				false,	FALSE,FALSE);
	//	C.r_Sampler_rtf		("s_base",			r2_RT_accum_temp	);
	//	C.r_End				();
	//	break;
	//case SE_MASK_ALBEDO:	// copy accumulator, 2D (for accum->color, albedo_wo)
	//	C.r_Pass			("null",			"copy",				false,	FALSE,FALSE);
	//	C.r_Sampler_rtf		("s_base",			r2_RT_accum			);
	//	C.r_End				();
	//	break;
	//}
}

} // namespace render
} // namespace xray

