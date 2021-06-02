////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_texture_density.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_editor_texture_density::compile(effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
	compile_begin("vertex_base", "editor_texture_density", compiler, &configuration, config);
		compiler.set_depth(true, true);
		compiler.set_cull_mode(D3D_CULL_NONE);
	compile_end(compiler);
}

} // namespace render
} // namespace xray