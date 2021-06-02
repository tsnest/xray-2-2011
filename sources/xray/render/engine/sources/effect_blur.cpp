////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_blur.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_blur::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique(); // horizontally
		compiler.begin_pass("blur", NULL, "blur_horizontally", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_alpha_blend(false, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique(); // vertically
		compiler.begin_pass("blur", NULL, "blur_vertically", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_alpha_blend(false, D3D_BLEND_SRC_ALPHA, D3D_BLEND_INV_SRC_ALPHA);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
