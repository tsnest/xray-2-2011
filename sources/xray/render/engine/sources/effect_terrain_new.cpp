////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_terrain_new.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>
#include "shared_names.h"
#include "geometry_type.h"

namespace xray {
namespace render {

effect_terrain_NEW::effect_terrain_NEW()//: effect_deffer_base( true, true, true)
{
}

void effect_terrain_NEW::compile( effect_compiler& compiler, custom_config_value const& config)
{
		XRAY_UNREFERENCED_PARAMETER	( config);

		static float4 l_material = float4(0.0f, 0.0f, 0.0f, 1.f);

		shader_configuration configuration;
		configuration.use_normal_texture = true;

		for (u32 pass_index = 0; pass_index < 3; pass_index++)
		{
			compiler.begin_technique( /*LEVEL*/);
				
				if (pass_index == 0)
					compiler.begin_pass			( "terrain", NULL, "terrain_pre_pass", configuration, NULL);
				else if (pass_index == 1)
					compiler.begin_pass			( "terrain", NULL, "terrain_material_pass", configuration, NULL);
				else 
					compiler.begin_pass			( "terrain", NULL, "terrain_shadow_pass", configuration, NULL);
					//.set_fill_mode		( D3D_FILL_WIREFRAME)
					
					if (pass_index == 0)
					{
						//compiler.set_stencil	( true, 0x6e, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP);
						compiler.set_stencil	( true, all_geometry_type, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP);
						compiler.set_depth		( true, true);
					}
					else if (pass_index == 1)
					{
						compiler.set_stencil	( false );
						compiler.set_depth		( true, false );
					}
					else
					{
						compiler.set_stencil	( false );
						compiler.set_depth		( true, true );
					}
					
					compiler.set_alpha_blend	( FALSE);
					compiler.set_texture		( "t_array_diffuse",&*(ref_texture)config["texture_tile_diffuse"]);
					compiler.set_texture		( "t_array_nmap",	&*(ref_texture)config["texture_tile_nmap"]);
					compiler.set_texture		( "t_array_spec",	&*(ref_texture)config["texture_tile_spec"]);
					

					if (pass_index == 1)
					{
						compiler.set_texture	("t_accumulator_dif", r2_rt_accum_diffuse);
						compiler.set_texture	("t_accumulator_spec", r2_rt_accum_specular);
						compiler.set_texture	("t_ssao_accumulator",	r2_rt_ssao_accumulator);
						compiler.set_texture	("t_decals_diffuse", r2_rt_decals_diffuse);
					}

	//				.set_texture		( "t_array_nmap",	"editor/editor_dummy_nmap")
					compiler.set_texture		( "t_height",		&*(ref_texture)config["height_texture"]);
					compiler.set_texture		( "t_color",		&*(ref_texture)config["color_texture"]);
					compiler.set_texture		( "t_params",		&*(ref_texture)config["params_texture"]);
					compiler.set_texture		( "t_tex_ids",		&*(ref_texture)config["tex_ids_texture"]);
					compiler.set_texture		( "t_tc_shift",		&*(ref_texture)config["tc_shift_texture"]);
					compiler.set_constant		( "terrain_size",	float4( 64.f, 64.f, 1.f/64, 1.f/64)	);
					compiler.set_texture		( "t_diffuse_detail", "terrain/earth_cracked");
					//compiler.set_texture		( "t_normal_detail", "terrain/earth_cracked_nmap");
					if (pass_index == 1)
					{
						compiler.set_texture("t_normal", r2_rt_n);
						compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
					}
					if (pass_index == 1)
					{
//						compiler.set_texture	("t_radiance_r", r2_rt_lpv_radiance_r);
//						compiler.set_texture	("t_radiance_g", r2_rt_lpv_radiance_g);
//						compiler.set_texture	("t_radiance_b", r2_rt_lpv_radiance_b);
					}
					//.def_sampler		( "s_tile", D3D_TEXTURE_ADDRESS_WRAP, D3D_FILTER_ANISOTROPIC)
						
					compiler.begin_sampler( "s_array")
						.set_address_mode	( D3D_TEXTURE_ADDRESS_WRAP, D3D_TEXTURE_ADDRESS_WRAP, D3D_TEXTURE_ADDRESS_WRAP)
						.set_filter			( D3D_FILTER_ANISOTROPIC)
						.set_max_anisotropy	( 16);
					compiler.end_sampler();
				compiler.end_pass	();
			compiler.end_technique();
		}

	return;
}
// 
// void effect_terrain_NEW::load( memory::reader& mem_reader)
// {
// 	effect_descriptor::load( mem_reader);
// 
// }

} // namespace render
} // namespace xray
