////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_debug_editor_wireframe.h"
#include <xray/render/core/effect_compiler.h>
#include "blend_mode.h"
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_debug_editor_wireframe::compile( effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
	compile_begin("vertex_base", "forward_base", compiler, &configuration, config);
		compiler.set_depth( true, false);
		compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
		
		s32				draw_mode = s32(config["draw_mode"]);
		math::float4	draw_color = math::float4(config["draw_color"]);

		if (draw_mode==0)
		{
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		}
		else if (draw_mode==1)
		{
			// TODO: points
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		}
		else
		{
			compiler.set_fill_mode(D3D_FILL_SOLID);
		}
		
		compiler.set_constant("solid_transparency",   draw_color.w);
		compiler.set_constant("solid_color_specular", draw_color);
		
	compile_end(compiler);
}

} // namespace render 
} // namespace xray 
