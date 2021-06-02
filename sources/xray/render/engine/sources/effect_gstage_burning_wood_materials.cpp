////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_gstage_burning_wood_materials.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_gstage_burning_wood_materials::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{

	shader_configuration configuration;
	
	configuration.use_diffuse_texture		= bool(custom_config["use_tdiffuse"]);
	configuration.use_normal_texture		= bool(custom_config["use_nmap"]);
	configuration.use_parallax				= bool(custom_config["use_parallax"]);
	configuration.use_alpha_test			= bool(custom_config["use_alpha_test"]);
	configuration.use_tfresnel_at_0_degree	= custom_config.value_exists("use_fresnel_at_0_degree_texture") ? bool(custom_config["use_fresnel_at_0_degree_texture"]) : false;
	configuration.use_detail_normal_texture	= custom_config.value_exists("use_detail_nmap") ? bool(custom_config["use_detail_nmap"]) : false;
	
	configuration.use_detail_texture		= custom_config.value_exists("use_tditail_diffuse") ? 
		bool(custom_config["use_tditail_diffuse"]) : false;

	configuration.use_reflection			= bool(custom_config["use_reflection"]) ? 1 : 0;
	
	if (custom_config.value_exists("use_variation_mask"))
		configuration.use_variation_mask	= bool(custom_config["use_variation_mask"]);
	
	if ( bool(custom_config["use_emissive"]))
	{
		configuration.use_emissive			= bool(custom_config["use_emissive_map"]) ? 2 : 1;
	}
	
	if ( bool(custom_config["use_reflection"]))
	{
		configuration.use_reflection		= 1;
		
		if (bool(custom_config["use_reflection_diffuse"]))
			configuration.use_reflection	= 2;
	}
	
	if (custom_config.value_exists("wind_motion"))
	{
		configuration.wind_motion = s32(custom_config["wind_motion"]);
	}
	
	configuration.use_specular_intensity_texture		= (bool)custom_config["use_tspecular_inensity"];
	configuration.use_specular_power_texture			= (bool)custom_config["use_tspecular_power"];
	configuration.use_transluceny_texture				= (bool)custom_config["use_ttranslucency"];
	
	configuration.is_anisotropic_material				= custom_config.value_exists("is_anisotropic_material") ? 
														  (bool)custom_config["is_anisotropic_material"] : false;
	
	for (u32 pass_index = 0; pass_index < 2; pass_index++)
	{
		if (pass_index == 0)
			compile_begin("vertex_base", "geometry_pre_pass", compiler, &configuration, custom_config);
		else
			compile_begin("vertex_base", "geometry_material_pass", compiler, &configuration, custom_config);
			
			if (pass_index == 0)
				compiler.set_stencil( true, 0x80, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP);
			else
				compiler.set_stencil( false );
			
			if (pass_index == 1)
				compiler.set_depth(true, false);
			
			float alpha_ref = 0.25f;
			
			if (configuration.use_alpha_test && custom_config.value_exists("alpha_ref"))
				alpha_ref = float(custom_config["alpha_ref"]);
			
			compiler.set_constant( "alpha_ref_parameter", alpha_ref );
			
			float const fresnel_at_0_degree				= custom_config.value_exists("fresnel_at_0_degree") ? custom_config["fresnel_at_0_degree"] : 0.04f;
			
			float const alpha_model_parameter			= 0.0f;//custom_config.value_exists("alpha_model_parameter") ? custom_config["alpha_model_parameter"] : 0.0f;
			compiler.set_constant						( "alpha_model_parameter", alpha_model_parameter );
			
			float const min_roughness					= custom_config.value_exists("min_roughness") ? custom_config["min_roughness"] : 0.0f;
			compiler.set_constant						( "min_roughness", min_roughness );
			
			float const max_roughness					= custom_config.value_exists("max_roughness") ? custom_config["max_roughness"] : 1.0f;
			compiler.set_constant						( "max_roughness", max_roughness );

			if (custom_config.value_exists("use_fresnel_material_type") && 
				bool(custom_config["use_fresnel_material_type"]) &&
				custom_config.value_exists("fresnel_material_type"))
			{
				s32 fresnel_material_type				= s32(custom_config["fresnel_material_type"]);
				switch (fresnel_material_type)
				{
				case 0: // Metal
					compiler.set_constant				( "fresnel_at_0_degree", 0.99f );
					break;
				case 1: // Wood
					compiler.set_constant				( "fresnel_at_0_degree", 0.1f );
					break;
				case 2: // Velvet
					compiler.set_constant				( "fresnel_at_0_degree", 0.04f );
					break;
				};
			}
			else
			{
				compiler.set_constant					( "fresnel_at_0_degree", fresnel_at_0_degree );
			}
			
			if (pass_index == 1)
			{
				compiler.set_texture					("t_accumulator_dif", r2_rt_accum_diffuse);
				compiler.set_texture					("t_accumulator_spec", r2_rt_accum_specular);
				compiler.set_texture					("t_ssao_accumulator",	r2_rt_ssao_accumulator);
				compiler.set_texture					("t_decals_diffuse", r2_rt_decals_diffuse);
			}
			
			float4	solid_color_specular( 0.f, 0.f, 0.f, 0.f);
			bool	solid_color_specular_used = false;
			
			if( configuration.use_diffuse_texture)
				compiler.set_texture("t_base", pcstr(custom_config["texture_diffuse"]));
			
			if (configuration.use_detail_texture)
				compiler.set_texture("t_detail", pcstr(custom_config["texture_ditail_diffuse"]));

			if (custom_config.value_exists("texture_whirl"))
				compiler.set_texture("t_whirl", pcstr(custom_config["texture_whirl"]));
			
			solid_color_specular		= float4(custom_config["constant_diffuse"]);
			solid_color_specular.w		= 0;
			solid_color_specular_used	= true;
			
			if (configuration.use_tfresnel_at_0_degree)
				compiler.set_texture("t_fresnel_at_0_degree", pcstr(custom_config["texture_fresnel_at_0_degree"]));
			
			if (configuration.use_reflection)
				compiler.set_texture("t_cubemap", pcstr(custom_config["texture_cubemap"]));
			
			if (pass_index == 0)
			{
				if( configuration.use_normal_texture)
					compiler.set_texture("t_normal", pcstr(custom_config["texture_normal"]));
			}
			else
			{
				compiler.set_texture("t_normal", r2_rt_n);
				compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
			}
			
			if (configuration.use_detail_normal_texture)
			{
				float4 detail_normal_parameters(1.0f, 1.0f, 1.0f, 1.0f);
				
				if (custom_config.value_exists("texture_detail_normal"))
					compiler.set_texture("t_detail_normal", pcstr(custom_config["texture_detail_normal"]));
				
				if (custom_config.value_exists("scale_detail_normal"))
				{
					float value = custom_config["scale_detail_normal"];
					detail_normal_parameters.x = value;
					detail_normal_parameters.y = value;
				}
				if (custom_config.value_exists("tile_detail_normal"))
				{
					float value = custom_config["tile_detail_normal"];
					detail_normal_parameters.z = value;
					detail_normal_parameters.w = value;
				}
				
				compiler.set_constant("detail_normal_parameters", detail_normal_parameters);
			}
			if( configuration.use_parallax)
			{
				if (custom_config.value_exists("constant_parallax_scale"))
					compiler.set_constant( "constant_parallax_scale", float(custom_config["constant_parallax_scale"]));
				
				compiler.set_texture("t_height_map", pcstr(custom_config["texture_bump"]));
			}
			
			float4	solid_emission_color( 0.f, 0.f, 0.f, 0.f);
			if ( configuration.use_emissive!=0)
			{
				float multiplier			= float(custom_config["constant_emissive_multiplier"]);
				solid_emission_color		= float4( custom_config["constant_emissive"]);
				solid_emission_color.xyz()	*= multiplier;
				compiler.set_constant( "solid_emission_color", solid_emission_color);
			}
			if ( configuration.use_emissive==2)
			{
				compiler.set_texture("t_emission", pcstr(custom_config["texture_emissive"]));
			}
			
			if( configuration.use_specular_intensity_texture)
				compiler.set_texture("t_specular_intensity", pcstr(custom_config["texture_specular_intensity"]));
			
			if (custom_config.value_exists("constant_specular_color") && custom_config.value_exists("constant_specular_color_multiplier"))
				compiler.set_constant( "specular_color_parameter", float4(custom_config["constant_specular_color"]).xyz() * float3(custom_config["constant_specular_color_multiplier"]));
						
			if( solid_color_specular_used)
				compiler.set_constant( "solid_color_specular", solid_color_specular);

			float4	solid_material_params( 0.f, 0.f, 0.f, 0.f);
			bool	solid_material_params_used = false;

			float4 roughness_uv_parameters(0, 0, min_roughness, max_roughness);
			
			if (pass_index == 0)
			{
				if( configuration.use_specular_power_texture)
					compiler.set_texture("t_roughness", pcstr(custom_config["texture_specular_power"]));
				else
				{
					if (custom_config.value_exists("constant_specular_power"))
						roughness_uv_parameters.x = float(custom_config["constant_specular_power"]);
					
					if (configuration.is_anisotropic_material)
					{
						if (custom_config.value_exists("constant_roughness_v_direction"))
							roughness_uv_parameters.y = float(custom_config["constant_roughness_v_direction"]);					
					}
					else
					{
						roughness_uv_parameters.y = roughness_uv_parameters.x;
					}
				}
				compiler.set_constant("roughness_uv_parameters", roughness_uv_parameters);
			}

			//solid_material_params.y = float(custom_config["constant_diffuse_power"]);
			solid_material_params_used = true;
			
			if( configuration.use_transluceny_texture)
				compiler.set_texture("t_translucency", pcstr(custom_config["texture_translucency"]));
			//else
			//{
				solid_material_params.z = float(custom_config["constant_translucency"]);
				solid_material_params_used = true;
			//}
			
			if (configuration.use_variation_mask)
			{
				compiler.set_constant( 
					"packed_variation_mask_parameters", 
					float4(
						custom_config.value_exists("constant_variation_position_devider") ? float(custom_config["constant_variation_position_devider"]) : 1.0f,
						custom_config.value_exists("constant_variation_rotate") ? float(custom_config["constant_variation_rotate"]) : 1.0f,
						custom_config.value_exists("constant_variation_scale") ? float(custom_config["constant_variation_scale"]) : 1.0f,
						custom_config.value_exists("constant_variation_multiply") ? float(custom_config["constant_variation_multiply"]) : 1.0f
					)
				);
				
				if (custom_config.value_exists("constant_variation_color"))
					compiler.set_constant( "variation_color", float4(custom_config["constant_variation_color"]));
				
				if (custom_config.value_exists("texture_variation_mask"))
					compiler.set_texture ( "t_variation_mask", pcstr(custom_config["texture_variation_mask"]));
			}
			
			if( solid_material_params_used)
				compiler.set_constant( "solid_material_params", solid_material_params);
		compile_end(compiler);
	}
	// fill rsm
	{
		shader_configuration local_configuration;
		local_configuration.use_diffuse_texture		= bool(custom_config["use_tdiffuse"]);
		
		compile_begin("vertex_base", "fill_reflective_shadow_map", compiler, &local_configuration, custom_config);
		
		if( local_configuration.use_diffuse_texture)
			compiler.set_texture("t_base", pcstr(custom_config["texture_diffuse"]));
		
		if (custom_config.value_exists("constant_diffuse"))
			compiler.set_constant("diffuse_color_parameter", float4(custom_config["constant_diffuse"]).xyz());
		else
			compiler.set_constant("diffuse_color_parameter", float3(1.0f, 1.0f, 1.0f));
		
		compile_end(compiler);
	}
}

} // namespace render 
} // namespace xray 
