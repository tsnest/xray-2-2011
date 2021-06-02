////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_system_colored.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_system_colored::compile(effect_compiler& c, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER	( config );

	c.begin_technique( /*solid*/)
		.begin_pass	( "color", NULL, "color", shader_configuration(), NULL)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			//.set_cull_mode(D3D11_CULL_NONE)
		.end_pass	()
	.end_technique();

	c.begin_technique( /*stenciled*/)
		.begin_pass	( "color_top", NULL, "color_doted", shader_configuration(), NULL)
			.set_depth	( true, true, D3D_COMPARISON_ALWAYS)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			//.set_cull_mode(D3D11_CULL_NONE)
		.end_pass	()
	.end_technique();

	c.begin_technique( /*voided*/)
		.begin_pass	( "color", NULL, "color", shader_configuration(), NULL)
			.color_write_enable( D3D_COLOR_WRITE_ENABLE_NONE)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			//.set_cull_mode(D3D11_CULL_NONE)
		.end_pass	()
	.end_technique();
	
	c.begin_technique( /*cover*/)
		.begin_pass	( "color_cover", NULL, "color_cover", shader_configuration(), NULL)
			.set_stencil( true, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.set_texture("t_position", r2_rt_p)
			.set_texture("t_random_rotates", "engine/ssao_rotate")
		.end_pass	()
	.end_technique();
}

} // namespace render
} // namespace xray
