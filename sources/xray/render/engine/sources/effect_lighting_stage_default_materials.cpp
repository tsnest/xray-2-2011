////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_lighting_stage_default_materials.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/facade/light_props.h>
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_lighting_stage_default_materials::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	shader_configuration configuration;
	
	configuration.use_diffuse_texture			 = bool(custom_config["use_tdiffuse"]);
	configuration.use_normal_texture			 = bool(custom_config["use_nmap"]);
	configuration.use_alpha_test				 = bool(custom_config["use_alpha_test"]);
	configuration.use_transparency_texture		 = bool(custom_config["use_ttransparency"]);
	configuration.use_tfresnel_at_0_degree		 = custom_config.value_exists("use_fresnel_at_0_degree_texture") ? bool(custom_config["use_fresnel_at_0_degree_texture"]) : false;

	configuration.use_specular_intensity_texture = bool(custom_config["use_tspecular_inensity"]);
	configuration.use_specular_power_texture	 = bool(custom_config["use_tspecular_power"]);
	
	configuration.is_anisotropic_material		= custom_config.value_exists("is_anisotropic_material") ? 
												(bool)custom_config["is_anisotropic_material"] : false;
	
	//for (u32 light_type_index=0; light_type_index<num_light_types; light_type_index++)
	{
		configuration.light_type				 = 0;//u8(light_type_index);
		
		compile_begin("vertex_base", "forward_lighting", compiler, &configuration, custom_config);	
			float4	solid_color_specular( 0.f, 0.f, 0.f, 0.f);
			bool	solid_color_specular_used = false;
			
			
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
			
			//solid_material_params.y = float(custom_config["constant_diffuse_power"]);
			solid_material_params_used = true;
			
			float   solid_transparency   = 1.0f;
			
			if (configuration.use_transparency_texture)
				compiler.set_texture("t_transparency", pcstr(custom_config["texture_transparency"]));
			//else
				solid_transparency = custom_config.value_exists("constant_transparency") ? float(custom_config["constant_transparency"]) : 1.0f;
			
			compiler.set_constant("solid_transparency",   solid_transparency);	
			
			if( solid_material_params_used)
				compiler.set_constant( "solid_material_params", solid_material_params);
			
			//compiler.set_cull_mode(D3D_CULL_NONE);
			//compiler.set_alpha_to_coverage(true);
		compile_end(compiler);
	}
}

} // namespace render
} // namespace xray