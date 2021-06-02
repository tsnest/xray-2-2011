////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_clouds.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_clouds::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER(config);
	
	compiler.begin_technique();
		compiler.begin_pass("clouds", NULL, "clouds", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray