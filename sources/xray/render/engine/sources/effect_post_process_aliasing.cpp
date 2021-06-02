////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_post_process_aliasing.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

using namespace xray::math;

void effect_post_process_aliasing::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("post_process", "post_process_aliasing");
			compiler.set_texture("t_scene_color", r2_rt_generic0);		// set here?
			compiler.set_texture("t_position", r2_rt_p);				// set here?
			compiler.set_texture("t_normal", r2_rt_n);					// set here?
			compiler.set_depth( true, false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
