////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_distortion_materials.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_post_process_distortion_materials::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	float distortion_scale = float(custom_config["constant_distortion_scale"]);
	
	shader_configuration shader_config;

	compile_begin("post_process", "post_process_distortion", compiler, &shader_config, custom_config);
		compiler.set_texture("t_base", pcstr(custom_config["texture_base"]));
		compiler.set_constant("distortion_scale", distortion_scale);
		compiler.set_depth(true, false);
		//compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
		compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
	compile_end(compiler);
}

} // namespace render 
} // namespace xray 
