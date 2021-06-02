////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_system_line.h"
#include "xray/render/core/effect_compiler.h"

namespace xray {
namespace render {

void effect_system_line::compile(effect_compiler& c, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER	( config );

	c.begin_technique( /*z_enabled*/)
		.begin_pass	( "system_line", NULL, "system_line", shader_configuration(), NULL)
			.set_depth( true, true)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
		.end_pass	()
	.end_technique();

	c.begin_technique( /*z_disabled*/)
		.begin_pass	( "system_line_top", NULL, "system_line", shader_configuration(), NULL)
			.set_depth( true, true, D3D_COMPARISON_ALWAYS)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
		.end_pass	()
	.end_technique();
}

} // namespace render
} // namespace xray
