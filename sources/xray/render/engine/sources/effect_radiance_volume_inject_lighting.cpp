////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_radiance_volume_inject_lighting.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_radiance_volume_inject_lighting::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETERS(&compiler, &config);
	
// 	compiler.begin_technique();
// 		compiler.begin_pass("lpv_inject_lighting", "lpv_inject_lighting", "lpv_inject_lighting", shader_configuration(), NULL);
// 			compiler.set_depth(false, false);
// 			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
// 			compiler.set_texture("t_lpv_rsm_albedo", r2_rt_lpv_rsm_albedo);
// 			compiler.set_texture("t_lpv_rsm_normal", r2_rt_lpv_rsm_normal);
// 			compiler.set_texture("t_lpv_rsm_position", r2_rt_lpv_rsm_position);
// 		compiler.end_pass();
// 	compiler.end_technique();
}

} // namespace render
} // namespace xray