////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_light_propagation_volumes.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_light_propagation_volumes::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER(config);
	
	// inject_lighting_stage
	compiler.begin_technique();
		compiler.begin_pass("lpv_inject_lighting", "lpv_inject_lighting", "lpv_inject_lighting", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			// TODO D3D_BLEND_OP_MAX
			//compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX);
			compiler.set_texture("t_lpv_rsm_albedo", r2_rt_lpv_rsm_albedo);
			compiler.set_texture("t_lpv_rsm_normal", r2_rt_lpv_rsm_normal);
			compiler.set_texture("t_lpv_rsm_position", r2_rt_lpv_rsm_position);
		compiler.end_pass();
	compiler.end_technique();
	
	// inject_occluders_light_view_stage
	compiler.begin_technique();
		compiler.begin_pass("lpv_inject_occluders", "lpv_inject_occluders", "lpv_inject_occluders", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			// TODO D3D_BLEND_OP_MAX
			//compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			//compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			//compiler.set_texture("t_lpv_rsm_albedo", r2_rt_lpv_rsm_albedo);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX);
			compiler.set_texture("t_lpv_rsm_normal", r2_rt_lpv_rsm_normal);
			compiler.set_texture("t_lpv_rsm_position", r2_rt_lpv_rsm_position);
		compiler.end_pass();
	compiler.end_technique();
	
	// inject_occluders_camera_view_stage
	compiler.begin_technique();
		compiler.begin_pass("lpv_inject_occluders_camera_view", "lpv_inject_occluders_camera_view", "lpv_inject_occluders_camera_view", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			// TODO D3D_BLEND_OP_MAX
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_MAX);
			compiler.set_texture("t_gbuffer_position_downsampled", r2_rt_gbuffer_position_downsampled);
			compiler.set_texture("t_gbuffer_normal_downsampled", r2_rt_gbuffer_normal_downsampled);
		compiler.end_pass();
	compiler.end_technique();
	
	// propagate_lighting_stage
	compiler.begin_technique();
		compiler.begin_pass("lpv_propagate_lighting", "lpv_propagate_lighting", "lpv_propagate_lighting", shader_configuration(), NULL);
			compiler.set_depth(false, false);
		compiler.end_pass();
	compiler.end_technique();
	
	// accumulate propagation stage
	compiler.begin_technique();
		compiler.begin_pass("lpv_accumulate_propagation", "lpv_accumulate_propagation", "lpv_accumulate_propagation", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray