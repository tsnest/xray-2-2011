////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_gather_sun_light_scattering_zone.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_gather_sun_light_scattering_zone::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("gather_bloom", NULL, "gather_sun_light_scattering_zone", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			//compiler.set_stencil(true, 0x80, 0x80, 0x00, D3D_COMPARISON_LESS_EQUAL);
			compiler.set_texture("t_position", r2_rt_p);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray