////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_light_mask.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"
#include <xray/render/core/effect_constant_storage.h>
#include "geometry_type.h"

namespace xray {
namespace render {

void effect_light_mask::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER	( config);
	
	 // Draw back faces.
	compiler.begin_technique(); // tech_mask_local_light
		compiler.begin_pass	("accum_mask", NULL, "dumb", shader_configuration(), NULL);
 			compiler.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL);
 			//compiler.set_stencil		( true, 0x81, 0x81, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE);
			//compiler.set_stencil		( true, terrain_geometry_type, 0xff, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR);
			compiler.set_stencil		( true, all_geometry_type, 0xff, 0xff, D3D_COMPARISON_LESS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT);
			//compiler.set_stencil		( true, speedtree_geometry_type, 0xff, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT);
			compiler.set_cull_mode		( D3D_CULL_FRONT);
			compiler.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE);
		compiler.end_pass	();
	compiler.end_technique();
	
	static const float temp_alpha_ref = 1.f;
	compiler.begin_technique(); // tech_mask_direct
		compiler.begin_pass	( "stub_notransform_t", NULL, "accum_sun_mask", shader_configuration(), NULL);
			compiler.set_depth			( false, false);
			//compiler.set_stencil		( true, 0x80, 0x80, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP);
			compiler.set_stencil		( false);//, all_geometry_type, 0xff, 0xff, D3D_COMPARISON_LESS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT);
			compiler.set_cull_mode		( D3D_CULL_NONE);
			compiler.color_write_enable	( D3D_COLOR_WRITE_ENABLE_NONE);
			compiler.set_alpha_blend	( TRUE, D3D_BLEND_ZERO, D3D_BLEND_ONE);
			compiler.set_constant		( "alpha_ref",	temp_alpha_ref);
			compiler.set_texture		( "t_normal",   r2_rt_n);
			compiler.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse);
			compiler.set_texture		( "t_decals_normal", r2_rt_decals_normal);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique(); // tech_mask_direct_single_material
		compiler.begin_pass	( "stub_notransform_t", NULL, "accum_sun_mask", shader_configuration(), NULL);
			compiler.set_depth			( false, false);
			compiler.set_stencil		( true, 0x80, 0x80, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP);
			compiler.set_cull_mode		( D3D_CULL_NONE);
			compiler.color_write_enable	( D3D_COLOR_WRITE_ENABLE_NONE);
			compiler.set_alpha_blend	( TRUE, D3D_BLEND_ZERO, D3D_BLEND_ONE);
			compiler.set_constant		( "alpha_ref",	temp_alpha_ref);
			compiler.set_texture		( "t_normal",	r2_rt_n);
			compiler.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse);
			compiler.set_texture		( "t_decals_normal", r2_rt_decals_normal);
		compiler.end_pass();
	compiler.end_technique();
	
#if 0
	compiler.begin_technique( /*SE_MASK_SPOT*/)
		.begin_pass	( "accum_mask", "dumb")
 			.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL)
 			.set_stencil		( true, 0x82, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE) 
			.set_cull_mode		( D3D_CULL_FRONT)
			.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE)
 			.set_texture		( "t_position",    r2_rt_p)
		.end_pass	()
	.end_technique();
	
	compiler.begin_technique( /*SE_MASK_POINT*/)
		.begin_pass	( "accum_mask", "dumb")
			.set_depth			( false, false, D3D_COMPARISON_LESS_EQUAL)
			.set_stencil		( true, 0x80, 0x81, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP)
			.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE)
			.set_cull_mode		( D3D_CULL_FRONT)
			.set_texture		( "t_position",    r2_rt_p)
		.end_pass	()

		.begin_pass	( "accum_mask", "dumb")
			.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL)
			.set_stencil		( true, 0x81, 0x81, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR, D3D_STENCIL_OP_KEEP) 
			.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE)
			.set_cull_mode		( D3D_CULL_FRONT)
			.set_texture		( "t_position",    r2_rt_p)
		.end_pass	()

	.end_technique();

	static const float temp_alpha_ref = 1.f;
	compiler.begin_technique( /*SE_MASK_DIRECT*/)
		.begin_pass	( "stub_notransform_t", "accum_sun_mask")
			.set_depth		( false, false)
			.set_stencil	( true, 0x80, 0x80, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP)
			.set_cull_mode	( D3D_CULL_NONE)
			.color_write_enable	( D3D_COLOR_WRITE_ENABLE_NONE)
			.set_alpha_blend	( TRUE, D3D_BLEND_ZERO, D3D_BLEND_ONE)
			.set_constant		( "alpha_ref",	temp_alpha_ref)
			.set_texture		( "t_normal",   r2_rt_n)
			.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse)
			.set_texture		( "t_decals_normal", r2_rt_decals_normal)
		.end_pass()
	.end_technique();

	compiler.begin_technique( /*SE_MASK_DIRECT_SINGLE_MATERIAL*/)
		.begin_pass	( "stub_notransform_t", "accum_sun_mask")
		.set_depth		( false, false)
		.set_stencil	( true, 0x80, 0x80, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP)
		.set_cull_mode	( D3D_CULL_NONE)
		.color_write_enable	( D3D_COLOR_WRITE_ENABLE_NONE)
		.set_alpha_blend	( TRUE, D3D_BLEND_ZERO, D3D_BLEND_ONE)
		.set_constant		( "alpha_ref",	temp_alpha_ref)
		.set_texture		( "t_normal",	r2_rt_n)
		.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse)
		.set_texture		( "t_decals_normal", r2_rt_decals_normal)
		.end_pass()
		.end_technique();

	compiler.begin_technique( /*SE_MASK_STENCIL*/)
		.begin_pass		( "stub_notransform_t", "dumb")
		.set_cull_mode		( D3D_CULL_NONE)
		.color_write_enable	( D3D_COLOR_WRITE_ENABLE_NONE)
		.set_stencil		( true, 0x00, 0x00, 0xFE, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_ZERO, D3D_STENCIL_OP_ZERO, D3D_STENCIL_OP_ZERO)
		.end_pass		()
	.end_technique	();
#endif // #if 0
}

} // namespace render
} // namespace xray