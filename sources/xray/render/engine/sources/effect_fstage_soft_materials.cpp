////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_fstage_soft_materials.h"
#include <xray/render/core/effect_compiler.h>
#include "blend_mode.h"
#include "shared_names.h"
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_fstage_soft_materials::compile( effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	configuration.use_emissive				= bool(config["use_temissive"]) ? 2 : 1;
	configuration.use_transparency_texture  = bool(config["use_ttransparency"]);
	
	compile_begin("vertex_base", "forward_soft", compiler, &configuration, config);
		float4	solid_color_specular (0.f, 0.f, 0.f, 0.f);
		float   solid_transparency   = 1.0f;
		
		if( configuration.use_emissive==2)
		{
			compiler.set_texture("t_base", pcstr(config["texture_emissive"]));
		}
		solid_color_specular		= float4(float4(config["constant_emissive"]).xyz(), float(config["constant_emissive_multiplier"]));
		
		if (configuration.use_transparency_texture)
		{
			compiler.set_texture("t_transparency", pcstr(config["texture_transparency"]));
		}
		//else
		{
			solid_transparency = config.value_exists("constant_transparency") ? float(config["constant_transparency"]) : 1.0f;
		}
		
		compiler.set_texture("t_position", r2_rt_p);
		
		compiler.set_constant("solid_transparency",   solid_transparency);
		compiler.set_constant("solid_color_specular", solid_color_specular);
		
		if (config.value_exists("constant_soft_distance"))
			compiler.set_constant("soft_distance", float(config["constant_soft_distance"]));
		
		// TODO: fix it
		compiler.set_cull_mode(D3D_CULL_NONE);
	compile_end(compiler);
}

} // namespace render
} // namespace xray