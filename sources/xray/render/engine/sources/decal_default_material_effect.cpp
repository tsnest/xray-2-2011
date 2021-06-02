////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "decal_default_material_effect.h"
#include <xray/render/core/effect_compiler.h>
#include "blend_mode.h"
#include <xray/render/core/effect_constant_storage.h>
#include "shared_names.h"
#include "geometry_type.h"

namespace xray {
namespace render {

void decal_default_material_effect::compile( effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
	configuration.use_diffuse_texture	= bool(config["use_tdiffuse"]);
	configuration.use_normal_texture	= m_is_forward ? false : bool(config["use_nmap"]);
	configuration.use_reflection		= config.value_exists("use_reflection") ? (bool(config["use_reflection"]) ? 1 : 0) : false;
	configuration.decal_material		= true;
	configuration.decal_type			= m_is_forward ? 1 : 0;
	
	for (u32 pass_index = 0; pass_index < 2; pass_index++)
	{
		compile_begin("vertex_base", "decal_base", compiler, &configuration, config);
			
			if (pass_index == 0)
			{
				compiler.set_depth		(true, false, D3D_COMPARISON_LESS_EQUAL);
				compiler.set_stencil	( true, 0xff, check_all_geometry_type, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT, D3D_STENCIL_OP_INVERT);
				compiler.set_cull_mode	(D3D_CULL_BACK);
			}	
			else
			{
				compiler.set_depth		(false, false, D3D_COMPARISON_LESS_EQUAL);
				compiler.set_stencil	( true, 0xff, check_all_geometry_type, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT, D3D_STENCIL_OP_KEEP);
				compiler.set_cull_mode	(D3D_CULL_FRONT);
			}
			
			if (!m_is_forward)
				compiler.set_alpha_blend(true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
			
			if (configuration.use_diffuse_texture)
				compiler.set_texture	("t_base", pcstr(config["texture_diffuse"]));
			
			if (config.value_exists("constant_diffuse"))
				compiler.set_constant	("constant_diffuse", math::float4(config["constant_diffuse"]));
			
			if (config.value_exists("normal_multiplier"))
				compiler.set_constant	("constant_normal_multiplier", math::float3(config["normal_multiplier"]));
			
			if (configuration.use_normal_texture)
				compiler.set_texture	("t_normal_map", pcstr(config["texture_normal"]));
			
			compiler.set_texture		("t_position", r2_rt_p);
			compiler.set_texture		("t_normal", r2_rt_n);
			
			if (config.value_exists("blend_with_geometry_normals"))
				compiler.set_constant			(
					"blend_with_geometry_normals",
					config.value_exists("blend_with_geometry_normals") ?
						(bool(config["blend_with_geometry_normals"]) ? 1.0f : -1.0f):
						-1.0f
				);
			
			if (configuration.use_reflection)
				compiler.set_texture("t_cubemap", pcstr(config["texture_cubemap"]));
			
		compile_end(compiler);
	}
}

} // namespace render 
} // namespace xray 
