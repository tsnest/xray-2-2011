////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_distortion_stage_default_materials.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_distortion_stage_default_materials::compile( effect_compiler& compiler, custom_config_value const& config)
{
	xray::math::float3 distortion_scale = xray::math::float3(config["distortion_scale"]);
	shader_configuration shader_config;
	
	compile_begin("vertex_base", "distortion_base", compiler, &shader_config, config);
		compiler.set_stencil( true, 0x80, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP);
		compiler.set_texture("t_base", pcstr(config["texture_distortion"]));
		compiler.set_constant("distortion_scale", distortion_scale);
		compiler.set_depth( true, false);
		compiler.set_cull_mode(D3D_CULL_NONE);
		//compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
		compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
	compile_end(compiler);
}

} // namespace render 
} // namespace xray 
