////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_downsample_reflective_shadow_map.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_downsample_reflective_shadow_map::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER(config);
	
	compiler.begin_technique();
		compiler.begin_pass("lpv_downsample_reflective_shadow_map", NULL, "lpv_downsample_reflective_shadow_map", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_lpv_rsm_albedo_source", r2_rt_lpv_rsm_albedo_source);
			compiler.set_texture("t_lpv_rsm_normal_source", r2_rt_lpv_rsm_normal_source);
			compiler.set_texture("t_lpv_rsm_position_source", r2_rt_lpv_rsm_position_source);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray