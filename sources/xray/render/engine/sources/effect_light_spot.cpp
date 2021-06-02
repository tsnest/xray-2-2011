////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_light_spot.h>
#include <xray/render/core/effect_compiler.h>


namespace xray {
namespace render_dx10 {

void effect_light_spot::compile( effect_compiler& compiler, const effect_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETER	( options);

	shader_defines_list	defines;
	make_defines( defines);

	bool		blend = TRUE;	//RImplementation.o.fp16_blend;
	D3D_BLEND	dest = blend ? D3D_BLEND_ONE : D3D_BLEND_ZERO;

	//compiler.begin_technique( /*fill*/)
	//	.begin_pass( "dumb", "copy", defines)
	//		.set_depth( FALSE, D3D_COMPARISON_ALWAYS)
	//		.def_sampler( "s_base", options.tex_list[0])
	//	.end_pass()
	//.end_technique();

	compiler.begin_technique( /*unshadowed*/)
		.begin_pass	( "accum_volume", "accum_omni_unshadowed", defines)
			.set_depth			( true, false, D3D_COMPARISON_GREATER)
			.set_stencil		( true, 0x00,0xff,0x00, D3D_COMPARISON_LESS_EQUAL, D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP)
			.set_alpha_blend	( blend, D3D_BLEND_ONE, dest)
			.set_cull_mode		( D3D_CULL_FRONT)

			.set_texture		( "t_position",    r2_rt_p)
			.set_texture		( "t_normal",      r2_rt_n)
			//.set_texture		( "t_accumulator", r2_rt_accum) //used when fp16 blend is not available
			.set_texture		( "t_material",    r2_material)
		.end_pass()
	.end_technique();

	//IBlender::Compile		( C);

	//BOOL	b_HW_smap		= RImplementation.o.HW_smap;
	//BOOL	b_HW_PCF		= RImplementation.o.HW_smap_PCF;
	//BOOL		blend		= RImplementation.o.fp16_blend;
	//D3D_BLEND	dest		= blend?D3D_BLEND_ONE:D3D_BLEND_ZERO;
	//switch ( C.iElement)
	//{
	//case SE_L_FILL:			// fill projective
	//	C.r_Pass			( "null", 			"copy",						false,	FALSE,	FALSE);
	//	C.r_Sampler			( "s_base",			C.L_textures[0]	);
	//	C.r_End				();
	//	break;
	//case SE_L_UNSHADOWED:	// unshadowed
	//	C.r_Pass			( "accum_volume",	"accum_omni_unshadowed",	false,	FALSE,FALSE,blend,D3D_BLEND_ONE,dest);
	//	C.r_Sampler_rtf		( "s_position",		r2_RT_P			);
	//	C.r_Sampler_rtf		( "s_normal",		r2_RT_N			);
	//	C.r_Sampler_clw		( "s_material",		r2_material		);
	//	C.r_Sampler_clf		( "s_lmap",			*C.L_textures[0]);
	//	C.r_Sampler_rtf		( "s_accumulator",	r2_RT_accum		);
	//	C.r_End				();
	//	break;
	//case SE_L_NORMAL:		// normal
	//	C.r_Pass			( "accum_volume",	"accum_omni_normal",		false,	FALSE,FALSE,blend,D3D_BLEND_ONE,dest);
	//	C.r_Sampler_rtf		( "s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		( "s_normal",		r2_RT_N);
	//	C.r_Sampler_clw		( "s_material",		r2_material);
	//	C.r_Sampler			( "s_lmap",			C.L_textures[0]	);
	//	if ( b_HW_smap)		{
	//		if ( b_HW_PCF)	C.r_Sampler_clf		( "s_smap",r2_RT_smap_depth	);
	//		else			C.r_Sampler_rtf		( "s_smap",r2_RT_smap_depth	);
	//	}
	//	else				C.r_Sampler_rtf		( "s_smap",r2_RT_smap_surf	);
	//	jitter				( C);
	//	C.r_Sampler_rtf		( "s_accumulator",	r2_RT_accum		);
	//	C.r_End				();
	//	break;
	//case SE_L_FULLSIZE:		// normal-fullsize
	//	C.r_Pass			( "accum_volume",	"accum_omni_normal",		false,	FALSE,FALSE,blend,D3D_BLEND_ONE,dest);
	//	C.r_Sampler_rtf		( "s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		( "s_normal",		r2_RT_N);
	//	C.r_Sampler_clw		( "s_material",		r2_material);
	//	C.r_Sampler			( "s_lmap",			C.L_textures[0]);
	//	if ( b_HW_smap)		{
	//		if ( b_HW_PCF)	C.r_Sampler_clf		( "s_smap",r2_RT_smap_depth	);
	//		else			C.r_Sampler_rtf		( "s_smap",r2_RT_smap_depth	);
	//	}
	//	else				C.r_Sampler_rtf		( "s_smap",r2_RT_smap_surf	);
	//	jitter				( C);
	//	C.r_Sampler_rtf		( "s_accumulator",	r2_RT_accum		);
	//	C.r_End				();
	//	break;
	//case SE_L_TRANSLUENT:	// shadowed + transluency
	//	C.r_Pass			( "accum_volume",	"accum_omni_transluent",	false,	FALSE,FALSE,blend,D3D_BLEND_ONE,dest);
	//	C.r_Sampler_rtf		( "s_position",		r2_RT_P);
	//	C.r_Sampler_rtf		( "s_normal",		r2_RT_N);
	//	C.r_Sampler_clw		( "s_material",		r2_material);
	//	C.r_Sampler_clf		( "s_lmap",			r2_RT_smap_surf);			// diff here
	//	if ( b_HW_smap)		{
	//		if ( b_HW_PCF)	C.r_Sampler_clf		( "s_smap",r2_RT_smap_depth	);
	//		else			C.r_Sampler_rtf		( "s_smap",r2_RT_smap_depth	);
	//	}
	//	else				C.r_Sampler_rtf		( "s_smap",r2_RT_smap_surf	);
	//	jitter				( C);
	//	C.r_Sampler_rtf		( "s_accumulator",	r2_RT_accum		);
	//	C.r_End				();
	//	break;
	//}
}

} // namespace render
} // namespace xray

