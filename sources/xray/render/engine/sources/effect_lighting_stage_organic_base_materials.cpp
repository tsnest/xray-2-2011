////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_lighting_stage_organic_base_materials.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/facade/light_props.h>
#include <xray/render/core/effect_constant_storage.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_lighting_stage_organic_base_materials::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	{
		shader_configuration configuration;
		compile_begin("vertex_base", "skin_position_pass", compiler, &configuration, custom_config);
		compile_end(compiler);
	}
	//for (u32 light_type_index=0; light_type_index<num_light_types; light_type_index++)
	{
		shader_configuration configuration;
		
		configuration.use_diffuse_texture			 = bool(custom_config["use_tdiffuse"]);
		configuration.use_normal_texture			 = bool(custom_config["use_nmap"]);
		configuration.use_alpha_test				 = custom_config.value_exists("use_alpha_test") ? bool(custom_config["use_alpha_test"]) : false;
		configuration.use_tfresnel_at_0_degree		 = custom_config.value_exists("use_fresnel_at_0_degree_texture") ? bool(custom_config["use_fresnel_at_0_degree_texture"]) : false;
		configuration.use_specular_intensity_texture = custom_config.value_exists("use_tspecular_inensity") ? bool(custom_config["use_tspecular_inensity"]) : false;
		configuration.use_specular_power_texture	 = custom_config.value_exists("use_tspecular_power") ? bool(custom_config["use_tspecular_power"]) : false;
		
		configuration.is_anisotropic_material		= custom_config.value_exists("is_anisotropic_material") ? 
													(bool)custom_config["is_anisotropic_material"] : false;
		
		configuration.light_type					= 0;//u8(light_type_index);
		configuration.shadowed_light				= true;

		compile_begin("unwrap_mesh", "organic_forward_lighting", compiler, &configuration, custom_config);	
			float4	solid_color_specular( 0.f, 0.f, 0.f, 0.f);
			bool	solid_color_specular_used = false;
			
			compiler.set_alpha_blend(false);//true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			
			float const fresnel_at_0_degree				= custom_config.value_exists("fresnel_at_0_degree") ? custom_config["fresnel_at_0_degree"] : 0.04f;
			compiler.set_constant						( "fresnel_at_0_degree", fresnel_at_0_degree );
			
			float const alpha_model_parameter			= 0.0f;//custom_config.value_exists("alpha_model_parameter") ? custom_config["alpha_model_parameter"] : 0.0f;
			compiler.set_constant						( "alpha_model_parameter", alpha_model_parameter );
			
			float const min_roughness					= custom_config.value_exists("min_roughness") ? custom_config["min_roughness"] : 0.0f;
			compiler.set_constant						( "min_roughness", min_roughness );
			
			float const max_roughness					= custom_config.value_exists("max_roughness") ? custom_config["max_roughness"] : 1.0f;
			compiler.set_constant						( "max_roughness", max_roughness );
			
			if (configuration.use_tfresnel_at_0_degree)
				compiler.set_texture("t_fresnel_at_0_degree", pcstr(custom_config["texture_fresnel_at_0_degree"]));
			
			if( configuration.use_diffuse_texture)
				compiler.set_texture("t_base", pcstr(custom_config["texture_diffuse"]));
			//else
			{
				solid_color_specular		= float4(custom_config["constant_diffuse"]);
				solid_color_specular.w		= 0;
				solid_color_specular_used	= true;
			}
			
			if( configuration.use_normal_texture)
				compiler.set_texture("t_normal", pcstr(custom_config["texture_normal"]));
			
			if( configuration.use_specular_intensity_texture)
				compiler.set_texture("t_specular_intensity", pcstr(custom_config["texture_specular_intensity"]));
			
			if (custom_config.value_exists("constant_specular_color") && custom_config.value_exists("constant_specular_color_multiplier"))
				compiler.set_constant( "specular_color_parameter", float4(custom_config["constant_specular_color"]).xyz() * float3(custom_config["constant_specular_color_multiplier"]));
			
			if( solid_color_specular_used)
				compiler.set_constant( "solid_color_specular", solid_color_specular);
			
			float4	solid_material_params( 0.f, 0.f, 0.f, 0.f);
			bool	solid_material_params_used = false;
			
			float4 roughness_uv_parameters(0, 0, min_roughness, max_roughness);
			
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
						
			float   solid_transparency   = 1.0f;
			compiler.set_constant("solid_transparency",   solid_transparency);	
			
			if( solid_material_params_used)
				compiler.set_constant( "solid_material_params", solid_material_params);
			
		compile_end(compiler);
	}
	
	{
		shader_configuration configuration;
		configuration.use_organic_scattering_depth_texture		= custom_config.value_exists("use_scattering_depth_texture") ? bool(custom_config["use_scattering_depth_texture"]) : false;		
		
		compiler.begin_technique();
			compiler.begin_pass("blur_irradiance_texture", NULL, "blur_organic_irradiance_texture", configuration, NULL);
				compiler.set_depth( false, false);
				compiler.set_cull_mode(D3D_CULL_NONE);
				compiler.set_fill_mode(D3D_FILL_SOLID);
				compiler.set_alpha_blend(false, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA);
				compiler.set_texture("t_skin_scattering", r2_rt_skin_scattering);
				
				//float4 scattering_depth_parameters = float4(0.1f, 0.5f, 0.0f, 0.0f);
				
				float4 scattering_component_blurring_weights_and_color_multiplier = float4(1.2f, 0.3f, 0.1f, 1.0f);
				//
				//if (custom_config.value_exists("scattering_depth") && custom_config.value_exists("scattering_min_depth"))
				//	scattering_depth_parameters = float4(
				//		float(custom_config["scattering_min_depth"]),
				//		float(custom_config["scattering_depth"]),
				//		0.0f,
				//		0.0f
				//	);
				
				if (custom_config.value_exists("scattering_component_blurring_weights"))
					scattering_component_blurring_weights_and_color_multiplier.xyz() = float3(custom_config["scattering_component_blurring_weights"]);
				
				if (custom_config.value_exists("scattering_color_multiplier"))
					scattering_component_blurring_weights_and_color_multiplier.w = float(custom_config["scattering_color_multiplier"]);
				
				if (configuration.use_organic_scattering_depth_texture && custom_config.value_exists("texture_scattering_depth"))
					compiler.set_texture("t_scattering_depth", pcstr(custom_config["texture_scattering_depth"]));
				
				//compiler.set_constant("scattering_depth_parameters", scattering_depth_parameters);
				compiler.set_constant("scattering_component_blurring_weights_and_color_multiplier", scattering_component_blurring_weights_and_color_multiplier);
				
			compiler.end_pass();
		compiler.end_technique();
	}
	
	{
		shader_configuration configuration;
		configuration.use_diffuse_texture = custom_config.value_exists("use_tdiffuse") ? bool(custom_config["use_tdiffuse"]) : false;
		configuration.use_normal_texture  = custom_config.value_exists("use_nmap") ? bool(custom_config["use_nmap"]) : false;
		configuration.use_specular_intensity_texture = custom_config.value_exists("use_tspecular_inensity") ? bool(custom_config["use_tspecular_inensity"]) : false;
		
		// TODO
		configuration.use_ao_texture							= custom_config.value_exists("use_ao_texture") ? bool(custom_config["use_ao_texture"]) : false;
		configuration.use_organic_scattering_amount_mask		= custom_config.value_exists("use_scattering_amount_mask") ? bool(custom_config["use_scattering_amount_mask"]) : false;
		configuration.use_organic_back_illumination_texture		= custom_config.value_exists("use_back_illumination_texture") ? bool(custom_config["use_back_illumination_texture"]) : false;
		configuration.use_organic_subdermal_texture				= custom_config.value_exists("use_subdermal_texture") ? bool(custom_config["use_subdermal_texture"]) : false;
		
		compile_begin("vertex_base", "organic_combine", compiler, &configuration, custom_config);
			
			//compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			//compiler.set_depth( true, false);
			//compiler.set_alpha_blend(false);
			
			compiler.set_depth( true, false);
			compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_ONE);
			
			float4	solid_color_specular (0.f, 0.f, 0.f, 0.f);
			
			if (configuration.use_diffuse_texture)
				compiler.set_texture("t_base", pcstr(custom_config["texture_diffuse"]));
			
			solid_color_specular = float4(custom_config["constant_diffuse"]);
			
			if( configuration.use_normal_texture)
				compiler.set_texture("t_normal", pcstr(custom_config["texture_normal"]));
			
			if (configuration.use_organic_scattering_amount_mask && custom_config.value_exists("texture_scattering_amount"))
				compiler.set_texture("t_sss_amount", pcstr(custom_config["texture_scattering_amount"]));
			
			if (configuration.use_organic_back_illumination_texture && custom_config.value_exists("texture_back_illumination"))
				compiler.set_texture("t_back_color", pcstr(custom_config["texture_back_illumination"]));
			
			if (configuration.use_organic_subdermal_texture && custom_config.value_exists("texture_subdermal"))
				compiler.set_texture("t_subdermal", pcstr(custom_config["texture_subdermal"]));

			if (configuration.use_ao_texture && custom_config.value_exists("texture_ao"))
				compiler.set_texture("t_ao", pcstr(custom_config["texture_ao"]));
			
			if( configuration.use_specular_intensity_texture)
				compiler.set_texture("t_specular_intensity", pcstr(custom_config["texture_specular_intensity"]));
			
			if (custom_config.value_exists("constant_specular_color") && custom_config.value_exists("constant_specular_color_multiplier"))
				compiler.set_constant( "specular_color_parameter", float4(custom_config["constant_specular_color"]).xyz() * float3(custom_config["constant_specular_color_multiplier"]));
			
			compiler.set_constant( "solid_color_specular", solid_color_specular);

			compiler.set_texture("t_skin_scattering", r2_rt_skin_scattering);
			compiler.set_texture("t_skin_scattering_blurred_0", r2_rt_skin_scattering_blurred_0);
			compiler.set_texture("t_skin_scattering_blurred_1", r2_rt_skin_scattering_blurred_1);
			compiler.set_texture("t_skin_scattering_blurred_2", r2_rt_skin_scattering_blurred_2);
			compiler.set_texture("t_skin_scattering_blurred_3", r2_rt_skin_scattering_blurred_3);
			compiler.set_texture("t_skin_scattering_blurred_4", r2_rt_skin_scattering_blurred_4);
			
		compile_end(compiler);
	}

}

} // namespace render
} // namespace xray