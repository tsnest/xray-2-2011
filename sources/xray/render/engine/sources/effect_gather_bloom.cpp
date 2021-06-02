////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_gather_bloom.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_gather_bloom::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("gather_bloom", NULL, "gather_bloom", shader_configuration(), NULL);
			//compiler.set_texture("t_frame_color", r2_rt_generic0);
			compiler.set_texture("t_position", r2_rt_p);
			compiler.set_texture("t_frame_luminance", r2_rt_frame_luminance);
			compiler.set_depth( true, false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
