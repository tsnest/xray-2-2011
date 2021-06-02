////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_speedtree_selection.h"
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_speedtree_selection::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	shader_configuration configuration;
	
	configuration.vertex_input_type	= u32(speedtree_branch_vertex_input_type);

	compiler.begin_technique(/*branch*/);
		compiler.begin_pass("vertex_base", NULL, "speedtree_selection", configuration, NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.end_pass();
	compiler.end_technique();

	configuration.vertex_input_type	= u32(speedtree_frond_vertex_input_type);
	
	compiler.begin_technique(/*frond*/);
		compiler.begin_pass("vertex_base", NULL, "speedtree_selection", configuration, NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.end_pass();
	compiler.end_technique();

	configuration.vertex_input_type	= u32(speedtree_leafmesh_vertex_input_type);
	
	compiler.begin_technique(/*leafmesh*/);
		compiler.begin_pass("vertex_base", NULL, "speedtree_selection", configuration, NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.end_pass();
	compiler.end_technique();

	configuration.vertex_input_type	= u32(speedtree_leafcard_vertex_input_type);
	
	compiler.begin_technique(/*leafcard*/);
		compiler.begin_pass("vertex_base", NULL, "speedtree_selection", configuration, NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.end_pass();
	compiler.end_technique();

	configuration.vertex_input_type	= u32(speedtree_billboard_vertex_input_type);
	
	compiler.begin_technique(/*billboard*/);
		compiler.begin_pass("vertex_base", NULL, "speedtree_selection", configuration, NULL);
			compiler.set_depth( false, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
