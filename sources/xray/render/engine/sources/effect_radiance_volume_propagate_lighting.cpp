////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_radiance_volume_propagate_lighting.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_radiance_volume_propagate_lighting::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETERS(&config, &compiler);
	
	//compiler.begin_technique();
	//	compiler.begin_pass("lpv_propagate_lighting", "lpv_propagate_lighting", "lpv_propagate_lighting", shader_configuration(), NULL);
	//		compiler.set_depth(false, false);
	//	compiler.end_pass();
	//compiler.end_technique();
}

} // namespace render
} // namespace xrayz