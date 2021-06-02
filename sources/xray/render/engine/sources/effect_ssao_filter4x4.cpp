////////////////////////////////////////////////////////////////////////////
//	Created		: 11.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_ssao_filter4x4.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_ssao_filter4x4::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETER(custom_config);
	
 	compiler.begin_technique();
 		compiler.begin_pass("post_process0", NULL, "ssao_filter4x4", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_alpha_blend(false);
//			compiler.set_texture("t_ssao_accumulator", r2_rt_ssao_accumulator);
			compiler.set_texture("t_position", r2_rt_p);
 		compiler.end_pass();
 	compiler.end_technique();

 	compiler.begin_technique();
 		compiler.begin_pass("post_process0", NULL, "ssao_filter4x4_1", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_alpha_blend(false);
//			compiler.set_texture("t_ssao_accumulator", r2_rt_ssao_accumulator);
			compiler.set_texture("t_position", r2_rt_p);
 		compiler.end_pass();
 	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
