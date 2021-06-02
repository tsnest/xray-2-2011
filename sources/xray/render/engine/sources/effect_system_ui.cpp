////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_system_ui.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_system_ui::compile( effect_compiler& c, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER	( config );

	c.begin_technique( /*ui_font*/)
		.begin_pass	( "stub_notransform_t", NULL, "ui_font", shader_configuration(), NULL)
			.set_depth( false, false)
			.set_stencil( false, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.set_cull_mode( D3D_CULL_NONE)
			.set_texture( "t_base", pcstr(config["ui_texture0"]))
		.end_pass	()
	.end_technique();

	c.begin_technique( /*ui*/)
		.begin_pass	( "stub_notransform_t", NULL, "ui", shader_configuration(), NULL)
			.set_depth( false, false)
			.set_stencil( false, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.set_cull_mode( D3D_CULL_NONE)
			.set_texture( "t_base", pcstr(config["ui_texture1"]))
		.end_pass	()
	.end_technique();

	c.begin_technique( /*ui_fill*/)
		.begin_pass	( "stub_notransform_t", NULL, "ui_fill", shader_configuration(), NULL)
			.set_depth( false, false)
			.set_stencil( false, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_alpha_blend( true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA)
			.set_cull_mode( D3D_CULL_NONE)
		.end_pass	()
	.end_technique();

	c.begin_technique( /*ui_line_strip*/)
		.begin_pass	( "stub_notransform_t", NULL, "ui_fill", shader_configuration(), NULL)
			.set_depth( false, false)
			.set_stencil( false, 0x20, 0x00, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
			.set_cull_mode( D3D_CULL_NONE)
		.end_pass	()
	.end_technique();
}

} // namespace render
} // namespace xray
