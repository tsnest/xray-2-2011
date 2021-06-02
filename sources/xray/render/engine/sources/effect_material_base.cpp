////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_material_base.h"
#include "blend_mode.h"
#include <xray/render/core/shader_include_getter.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/shader_configuration.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

class material_shader_include_getter: public shader_include_getter
{
public:
	material_shader_include_getter(u32 vertex_type): m_vertex_type(vertex_type) {}
	
	virtual pcstr get_shader_include(pcstr include_file_name) const 
	{ 
		if (strings::compare(include_file_name, "vertex_input.h")==0)
		{
			switch (m_vertex_type)
			{
				case null_vertex_input_type:				return "null_mesh_vertex_input.h";
				case static_mesh_vertex_input_type:			return "static_mesh_vertex_input.h";
				case skeletal_mesh_vertex_input_type:		return "skeletal_mesh_vertex_input.h";
				case particle_vertex_input_type:			return "particle_vertex_input.h";
				case particle_subuv_vertex_input_type:		return "particle_subuv_vertex_input.h";
				case particle_beamtrail_vertex_input_type:	return "particle_beamtrail_vertex_input.h";
				case decal_vertex_input_type:				return "decal_vertex_input.h";
				case lensflares_vertex_input_type:			return "lensflares_vertex_input.h";
				case terrain_vertex_input_type:				return "terrain_vertex_input.h";
				case grassmesh_vertex_input_type:			return "grass_vertex_input.h";
				case grassbillboard_vertex_input_type:		return "grass_vertex_input.h";
				case speedtree_branch_vertex_input_type:	return "speedtree_branch_vertex_input.h";
				case speedtree_frond_vertex_input_type:		return "speedtree_frond_vertex_input.h";
				case speedtree_leafmesh_vertex_input_type:	return "speedtree_leafmesh_vertex_input.h";
				case speedtree_leafcard_vertex_input_type:	return "speedtree_leafcard_vertex_input.h";
				case speedtree_billboard_vertex_input_type:	return "speedtree_billboard_vertex_input.h";
			}
		}
		return include_file_name;
	}

private:
	u32 m_vertex_type;
};

void effect_material_base::compile_begin(pcstr vertex_shader_name, pcstr pixel_shader_name, effect_compiler& compiler, shader_configuration* shader_config, custom_config_value const& config)
{
	compile_begin(vertex_shader_name, 0, pixel_shader_name, compiler, shader_config, config);
}

void effect_material_base::compile_begin(pcstr vertex_shader_name, pcstr geometry_shader_name, pcstr pixel_shader_name, effect_compiler& compiler, shader_configuration* shader_config, custom_config_value const& config)
{
	if (config.value_exists("vertex_input_type"))
		shader_config->vertex_input_type = u32((enum_vertex_input_type)config["vertex_input_type"]);
	
	D3D_CULL_MODE cull_mode = D3D_CULL_BACK;
	if (config.value_exists("cull_mode"))
		cull_mode = (D3D_CULL_MODE)config["cull_mode"];
	
	shader_config->use_subuv		= shader_config->vertex_input_type == particle_subuv_vertex_input_type;
	
	//material_shader_include_getter  include_getter( vertex_input_type );
	
	s32 blend_mode = -1;
	
	if (config.value_exists("blend_mode"))
		blend_mode = s32(config["blend_mode"]);
	
	compiler.begin_technique();
	compiler.begin_pass(vertex_shader_name, geometry_shader_name, pixel_shader_name, *shader_config, 0);//&include_getter);
	
	switch (blend_mode)
	{
		case blend_mode_opaque:
			compiler.set_depth( true, true);
			compiler.set_alpha_blend(false);
			break;
		case blend_mode_translucent:
			compiler.set_depth( true, false);
			compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
			break;
		case blend_mode_additive:
			compiler.set_depth( true, false);
			compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_ONE);
			break;
		case blend_mode_modulate:
			compiler.set_depth( true, false);
			compiler.set_alpha_blend(true,D3D_BLEND_ZERO,D3D_BLEND_SRC_COLOR);
			break;
		case blend_mode_subtractive:
			compiler.set_depth( true, false);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE,D3D_BLEND_OP_REV_SUBTRACT);
			break;
	}
	//	int bool_type = static_type::get_type_id<bool>();
	
	//if (config.value_exists("two_sided") /*&& config["two_sided"].type==bool_type*/ && bool(config["two_sided"]))
	//	compiler.set_cull_mode(D3D_CULL_NONE);
	//else
		compiler.set_cull_mode(cull_mode);
}

void effect_material_base::compile_end(effect_compiler& compiler)
{
	compiler.end_pass();
	compiler.end_technique();	
}

} // namespace render
} // namespace xray
