////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "depth_accumulate_material_effect.h"
#include <xray/render/core/effect_compiler.h>

void xray::render::depth_accumulate_material_effect::compile	( effect_compiler& compiler, custom_config_value const& config )
{
	shader_configuration				configuration;
	configuration.use_alpha_test		= config.value_exists("use_alpha_test") ? bool(config["use_alpha_test"]) : false;
	
	compile_begin("vertex_base", "depth_accumulate", compiler, &configuration, config);	
		
		if (configuration.use_alpha_test)
			compiler.set_texture("t_base", pcstr(config["texture_diffuse"]));
		
		compiler.set_depth			(true, true);
		compiler.color_write_enable (D3D_COLOR_WRITE_ENABLE_NONE);
		compiler.set_cull_mode		(D3D_CULL_NONE);
		
	compile_end(compiler);
}