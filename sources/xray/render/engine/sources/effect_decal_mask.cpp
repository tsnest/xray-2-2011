////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_decal_mask.h"
#include <xray/render/core/effect_compiler.h>
#include "geometry_type.h"

namespace xray {
namespace render {

void effect_decal_mask::compile(effect_compiler& compiler, custom_config_value const&)
{
	 // Draw back faces.
	compiler.begin_technique();
		compiler.begin_pass	("accum_mask", NULL, "dumb", shader_configuration(), NULL); // specific_type
 			compiler.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL);
			compiler.set_stencil		( true, all_geometry_type, 0xff, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT);
			compiler.set_cull_mode		( D3D_CULL_FRONT);
			compiler.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE);
		compiler.end_pass	();
	compiler.end_technique();
	
	 // Draw back faces.
	compiler.begin_technique();
		compiler.begin_pass	("accum_mask", NULL, "dumb", shader_configuration(), NULL); // all_types
 			compiler.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL);
			compiler.set_stencil		( true, all_geometry_type, 0xff, 0xff, D3D_COMPARISON_LESS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT);
			compiler.set_cull_mode		( D3D_CULL_FRONT);
			compiler.color_write_enable ( D3D_COLOR_WRITE_ENABLE_NONE);
		compiler.end_pass	();
	compiler.end_technique();
}

} // namespace render
} // namespace xray