////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_copy_image.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_copy_image::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique(); // copy_rewrite
		compiler.begin_pass("copy_image", NULL, "copy_image", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_alpha_blend(false);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique(); // copy_additive
		compiler.begin_pass("copy_image", NULL, "copy_image_simple", shader_configuration(), NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_fill_mode(D3D_FILL_SOLID);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
