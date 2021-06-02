////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_mlaa.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_post_process_mlaa::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("post_process_mlaa", NULL, "mlaa_color_edge_detection", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_frame_color", r2_rt_generic1);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique();
		compiler.begin_pass("post_process_mlaa", NULL, "mlaa_blending_weight_calculation", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_edges", r2_rt_mlaa_edges);
			compiler.set_texture("t_area", "engine/mlaa_area_map_129");
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique();
		compiler.begin_pass("post_process_mlaa", NULL, "mlaa_neighborhood_blending", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_edges", r2_rt_mlaa_edges);
			compiler.set_texture("t_blend", r2_rt_mlaa_blended_weights);
			compiler.set_texture("t_frame_color", r2_rt_generic1);
			compiler.set_texture("t_area", "engine/mlaa_area_map_129");
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
