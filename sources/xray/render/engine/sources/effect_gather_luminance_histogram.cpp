////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_gather_luminance_histogram.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_gather_luminance_histogram::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("gather_luminance", NULL, "gather_luminance_in_range", shader_configuration(), NULL);
			compiler.set_depth( false, false);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique();
		compiler.begin_pass("gather_luminance", NULL, "gather_luminance_count", shader_configuration(), NULL);
			compiler.set_depth( false, false);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique();
		compiler.begin_pass("gather_luminance2", NULL, "gather_luminance_histogram", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_texture("t_frame_luminance", r2_rt_frame_luminance);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
