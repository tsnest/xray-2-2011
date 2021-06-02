////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_apply_distortion.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_apply_distortion::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("apply_distortion", NULL, "apply_distortion", shader_configuration(), NULL);
			compiler.set_texture("t_base", r2_rt_generic1);
			compiler.set_texture("t_distortion", r2_rt_distortion);
			compiler.set_depth( true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
