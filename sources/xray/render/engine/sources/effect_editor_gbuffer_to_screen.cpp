////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_gbuffer_to_screen.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_editor_gbuffer_to_screen::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER(config);
	
		compiler.begin_technique();
		compiler.begin_pass("post_process0", NULL, "gbuffer_to_screen", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_alpha_blend(false);
			compiler.set_texture("t_position", r2_rt_p);
			compiler.set_texture("t_normal", r2_rt_n);
			compiler.set_texture("t_diffuse", r2_rt_albedo);
			compiler.set_texture("t_emissive",	r2_rt_emissive);
			compiler.set_texture("t_ssao_accumulator",	r2_rt_ssao_accumulator);
			compiler.set_texture("t_accumulator_dif", r2_rt_accum_diffuse);
			compiler.set_texture("t_accumulator_spec", r2_rt_accum_specular);
			compiler.set_texture("t_frame_color0", r2_rt_generic0);
			compiler.set_texture("t_frame_color1", r2_rt_generic1);
			compiler.set_texture("t_distortion", r2_rt_distortion);
			compiler.set_texture("t_frame_luminance", r2_rt_frame_luminance);
			compiler.set_texture("t_frame_luminance_histogram", r2_rt_frame_luminance_histogram);
			compiler.set_texture("t_decals_diffuse", r2_rt_decals_diffuse);
			compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray
