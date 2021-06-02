////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_shadow_map.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_shadow_map::compile(effect_compiler& compiler, custom_config_value const& )
{
	compiler.begin_technique( )
			.begin_pass	( "dumb", NULL, "dumb", shader_configuration(), NULL)
				.set_depth		( true, true)
				.color_write_enable( D3D_COLOR_WRITE_ENABLE_NONE)
			.end_pass	( )
		.end_technique();
}

} // namespace render
} // namespace xray