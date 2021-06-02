////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_editor_wireframe_accumulation.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/effect_constant_storage.h>

namespace xray {
namespace render {


void effect_editor_wireframe_accumulation::compile(effect_compiler& compiler, custom_config_value const& config)
{
	shader_configuration configuration;
	
	u32 vertex_input_type = 0;
	
	float3 wireframe_colors[u32(num_vertex_input_types)];
	
	for (u32 i = 0; i < u32(num_vertex_input_types); i++)
	{
		wireframe_colors[i] = float3(0.0f, 0.5f, 0.5f);
	}
	
 	wireframe_colors[skeletal_mesh_vertex_input_type] = float3(0.0f, 0.25f, 0.75f);
 	
	wireframe_colors[particle_vertex_input_type] = 
 	wireframe_colors[particle_subuv_vertex_input_type] = 
 	wireframe_colors[particle_beamtrail_vertex_input_type] = float3(1.0f, 0.25f, 0.25f);
	
 	wireframe_colors[speedtree_branch_vertex_input_type] =  
	wireframe_colors[speedtree_frond_vertex_input_type] = 
	wireframe_colors[speedtree_leafmesh_vertex_input_type] = 
	wireframe_colors[speedtree_leafcard_vertex_input_type] =  
	wireframe_colors[speedtree_billboard_vertex_input_type] = float3(0.25f, 0.75f, 0.25f);
	
	wireframe_colors[grassmesh_vertex_input_type] = 
	wireframe_colors[grassbillboard_vertex_input_type] = float3(0.25f, 0.5f, 0.25f);
	
	wireframe_colors[terrain_vertex_input_type] = float3(0.333f, 0.333f, 0.333f);
	
	wireframe_colors[wires_vertex_input_type] = float3(0.7f, 0.5f, 0.1f);
	
	if (config.value_exists("vertex_input_type"))
		vertex_input_type = u32((enum_vertex_input_type)config["vertex_input_type"]);
	
	compile_begin("vertex_base", "editor_wireframe_accumulation", compiler, &configuration, config);
		compiler.set_depth(true, true);
		compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.set_constant("wireframe_color", wireframe_colors[vertex_input_type]);
	compile_end(compiler);
	
	compile_begin("vertex_base", "editor_wireframe_accumulation", compiler, &configuration, config);
		compiler.set_depth(true, true);
		compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.set_cull_mode(D3D_CULL_NONE);
		compiler.set_constant("wireframe_color", wireframe_colors[vertex_input_type]);
	compile_end(compiler);
}

} // namespace render
} // namespace xray