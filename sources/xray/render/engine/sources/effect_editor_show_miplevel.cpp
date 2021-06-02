////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_show_miplevel.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_editor_show_miplevel::compile(effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
	compile_begin("vertex_base", "editor_texture_mip_level", compiler, &configuration, config);
		compiler.set_depth(true, true);
		compiler.set_cull_mode(D3D_CULL_NONE);
		compiler.set_texture("t_albedo_color", r2_rt_albedo);
	compile_end(compiler);
}

} // namespace render
} // namespace xray