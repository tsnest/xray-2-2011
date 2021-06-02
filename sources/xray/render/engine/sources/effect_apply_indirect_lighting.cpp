////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_apply_indirect_lighting.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"
#include "geometry_type.h"

namespace xray {
namespace render {

void effect_apply_indirect_lighting::compile(effect_compiler& compiler, custom_config_value const& config)
{
	XRAY_UNREFERENCED_PARAMETER(config);
	
	compiler.begin_technique();
		compiler.begin_pass("apply_indirect_lighting", NULL, "apply_indirect_lighting", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_texture("t_normal", r2_rt_n);
			compiler.set_texture("t_position", r2_rt_p);
			compiler.set_texture("t_ssao_accumulator",	r2_rt_ssao_accumulator);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique();
		compiler.begin_pass("apply_indirect_lighting", NULL, "apply_indirect_lighting_add", shader_configuration(), NULL);
			compiler.set_depth(false, false);
			compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			compiler.set_texture("t_indirect",	r2_rt_indirect_lighting);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render
} // namespace xray