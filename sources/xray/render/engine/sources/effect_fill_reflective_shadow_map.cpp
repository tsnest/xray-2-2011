////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_fill_reflective_shadow_map.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_fill_reflective_shadow_map::compile(effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	compile_begin("vertex_base", "fill_reflective_shadow_map", compiler, &configuration, config);
	compile_end(compiler);
}

} // namespace render
} // namespace xray