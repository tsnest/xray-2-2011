////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_terrain_debug_materials.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_post_process_terrain_debug_materials::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	compiler.begin_technique();
		compiler.begin_pass("copy_image", NULL, "terrain_debug", shader_configuration(), NULL);
			compiler.set_depth( false, false, D3D_COMPARISON_LESS_EQUAL);
			compiler.set_stencil( true, 0x80,0x80,0x00, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_texture("t_position", r2_rt_p);
			compiler.set_texture("t_normal", r2_rt_n);
			compiler.set_texture("t_decals_diffuse", r2_rt_decals_diffuse);
			compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
			compiler.set_texture("t_gradient", pcstr(custom_config["texture_gradient"]));
			compiler.set_constant("deepening_color_and_range", float4(float4(custom_config["constant_deepening_color"]).xyz(), float(custom_config["constant_deepening_range"])));
			compiler.set_constant("deepening_parameters", float4(
				float(custom_config["constant_deepening_clip_dist"]),
				float(custom_config["constant_deepening_scale"]),
				float(custom_config["constant_deepening_power"]),
				0));
			compiler.set_alpha_blend(false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray