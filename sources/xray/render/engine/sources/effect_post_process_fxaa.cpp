////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_fxaa.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_post_process_fxaa::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("post_process_fxaa", NULL, "post_process_fxaa", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_frame_color", r2_rt_generic1);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
