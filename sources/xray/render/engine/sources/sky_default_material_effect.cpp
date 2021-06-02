////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sky_default_material_effect.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>
#include "shared_names.h"

namespace xray {
namespace render {

void sky_default_material_effect::compile( effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	compile_begin("vertex_base", "forward_empty", compiler, &configuration, config);
	compile_end(compiler);
	
	compiler.begin_technique();
		compiler.begin_pass("sky2", NULL, "sky2", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_stencil(true, 0x00, 0xff, 0x00, D3D_COMPARISON_EQUAL);
			compiler.set_texture("t_skybox", pcstr(config["sky_texture"]));
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
