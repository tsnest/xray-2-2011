////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_fix_irradiance_texture.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_fix_irradiance_texture::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("blur_irradiance_texture", NULL, "fix_irradiance_texture", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_texture("t_skin_scattering_temp", r2_rt_skin_scattering_temp);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
