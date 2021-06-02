////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_antialiasing.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_post_process_antialiasing::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("post_process0", NULL, "post_process_antialiasing", shader_configuration(), NULL);
			compiler.set_texture("t_position", r2_rt_p);				// set here?
			compiler.set_texture("t_normal", r2_rt_n);					// set here?
			compiler.set_texture("t_decals_diffuse", r2_rt_decals_diffuse);
			compiler.set_texture("t_decals_normal", r2_rt_decals_normal);
			compiler.set_depth(true, false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
