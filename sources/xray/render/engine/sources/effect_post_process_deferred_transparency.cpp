////////////////////////////////////////////////////////////////////////////
//	Created		: 08.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_post_process_deferred_transparency.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_post_process_deferred_transparency::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("post_process0", NULL, "post_process_deferred_transparency", shader_configuration(), NULL);
			compiler.set_texture("t_position", r2_rt_p);				// set here?
			compiler.set_texture("t_normal", r2_rt_n);					// set here?
			compiler.set_depth( true, false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
