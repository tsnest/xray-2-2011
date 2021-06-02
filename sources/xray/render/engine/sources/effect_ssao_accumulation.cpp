////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_ssao_accumulation.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_ssao_accumulation::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETER(custom_config);
	
 	compiler.begin_technique();
 		compiler.begin_pass("post_process0", NULL, "ssao_accumulation", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_alpha_blend(false);
 			compiler.set_texture("t_random_rotates", "engine/ssao_rotate");
			compiler.set_texture("t_position", r2_rt_p);
			compiler.set_texture("t_normal", r2_rt_n);
			compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
 		compiler.end_pass();
 	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
