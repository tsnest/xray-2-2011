////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_show_overdraw.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {

void effect_editor_show_overdraw::compile(effect_compiler& compiler, custom_config_value const&)
{
/*
	NewStencilBufferValue = (StencilBufferValue & ~StencilWriteMask) | 
							(StencilWriteMask & StencilOp(StencilBufferValue))
	
	D3D10_COMPARISON_NEVER			if(false)
	D3D10_COMPARISON_LESS			if((ref & mask) < (stencil & mask))
	D3D10_COMPARISON_LESS_EQUAL		if((ref & mask) <= (stencil & mask))
	D3D10_COMPARISON_GREATER		if((ref & mask) > (stencil & mask))
	D3D10_COMPARISON_GREATER_EQUAL	if((ref & mask) >= (stencil & mask))
	D3D10_COMPARISON_EQUAL			if((ref & mask) == (stencil & mask))
	D3D10_COMPARISON_NOT_EQUAL		if((ref & mask) != (stencil & mask))
	D3D10_COMPARISON_ALWAYS			if(true)
*/
	math::float4 overdraw_colors[effect_editor_show_overdraw::num_overdraw_layers];
	
	math::float4 min_color		= math::float4(0.0f, 0.25f, 0.0f, 1.0f);
	math::float4 middle_color	= math::float4(1.0f, 0.0f, 0.0f, 1.0f);
	math::float4 max_color		= math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	for (u32 layer_index = 0; layer_index < effect_editor_show_overdraw::num_overdraw_layers; layer_index++)
	{
		float const alpha = float(layer_index + 1.0f) / float(effect_editor_show_overdraw::num_overdraw_layers);
		
		if (alpha <= 0.5f)
		{
			float const local_alpha = (alpha - 0.0f) * 0.5f;
			overdraw_colors[layer_index] = min_color * (1.0f - alpha) + middle_color * alpha;
		}
		else
		{
			float const local_alpha = (alpha - 0.5f) * 0.5f;
			overdraw_colors[layer_index] = middle_color * (1.0f - alpha) + max_color * alpha;
		}
		
	}
	
	for (u32 layer_index = 0; layer_index < effect_editor_show_overdraw::num_overdraw_layers; layer_index++)
	{
		compiler.begin_technique();
			compiler.begin_pass("post_process0", NULL, "editor_show_overdraw", shader_configuration(), NULL);
				compiler.set_depth(false, false);
				compiler.set_alpha_blend(false);
				
				if (layer_index + 1 == effect_editor_show_overdraw::num_overdraw_layers)
					compiler.set_stencil(true, layer_index + 1, 0xff, 0xff, D3D_COMPARISON_LESS_EQUAL);
				else
					compiler.set_stencil(true, layer_index + 1, 0xff, 0xff, D3D_COMPARISON_EQUAL);
				
				compiler.set_constant("show_overdraw_color", overdraw_colors[layer_index]);
				
			compiler.end_pass();
		compiler.end_technique();
	}
}

} // namespace render
} // namespace xray