////////////////////////////////////////////////////////////////////////////
//	Created		: 27.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_fstage_volume_cone_base_materials.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_fstage_volume_cone_base_materials::compile( effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	configuration.use_diffuse_texture	    = bool(config["use_tdiffuse"]);
	configuration.use_transparency_texture  = bool(config["use_ttransparency"]);

	compile_begin("vertex_base", "forward_volume_cone_base", compiler, &configuration, config);
		
		float   solid_transparency   = 1.0f;
		
		compiler.set_cull_mode(D3D_CULL_NONE);
		
		float4 volume_color = float4(config["constant_volume_color"]);
		float multiplier	= float (config["constant_volume_color_multiplier"]);
		
		compiler.set_constant("mode_direction_and_uv_tile", float4(float3(config["move_direction"]), float(config["uv_tile"])));
		
		compiler.set_constant("volume_color", float4(volume_color.xyz()*multiplier, volume_color.w));
		compiler.set_texture("t_position", r2_rt_p);
		
		if( configuration.use_diffuse_texture)
			compiler.set_texture("t_diffuse", pcstr(config["texture_diffuse"]));
		
		compiler.set_texture("t_spot_falloff", "fx/spot_falloff");
		compiler.set_texture("t_sphere_falloff", "fx/sphere_falloff");

		if (configuration.use_transparency_texture)
			compiler.set_texture("t_transparency", pcstr(config["texture_transparency"]));
		//else
			solid_transparency = config.value_exists("constant_transparency") ? float(config["constant_transparency"]) : 1.0f;
		
		compiler.set_constant("attenuation_scale", float(config["attenuation_scale"]));
		
		compiler.set_constant("solid_transparency",   solid_transparency);

	compile_end(compiler);
}

} // namespace render
} // namespace xray