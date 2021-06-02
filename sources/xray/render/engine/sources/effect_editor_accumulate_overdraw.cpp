////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_accumulate_overdraw.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_editor_accumulate_overdraw::compile(effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
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
	
	compile_begin("vertex_base", "editor_accumulate_overdraw", compiler, &configuration, config);
		compiler.set_depth(true, true);
		compiler.set_stencil(true, 0xff, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INCR, D3D_STENCIL_OP_KEEP);
		compiler.set_cull_mode(D3D_CULL_BACK);
		//compiler.color_write_enable(D3D_COLOR_WRITE_ENABLE_NONE);
	compile_end(compiler);
}

} // namespace render
} // namespace xray