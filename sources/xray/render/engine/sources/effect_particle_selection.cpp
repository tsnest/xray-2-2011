////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_particle_selection.h"
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/facade/vertex_input_type.h>

namespace xray {
namespace render {

void effect_particle_selection::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	enum_vertex_input_type vertex_types[] = {particle_vertex_input_type, 
											 particle_subuv_vertex_input_type, 
											 particle_beamtrail_vertex_input_type};
	
	for (u32 index = 0; index < 3; index++)
	{
		shader_configuration configuration;
		configuration.vertex_input_type = vertex_types[index];
		
		compile_begin("vertex_base", "particle_selected", compiler, &configuration, custom_config);
			compiler.set_depth( true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
			compiler.set_fill_mode(D3D_FILL_WIREFRAME);
		compile_end(compiler);
	}

#if 0
	shader_configuration configuration;
	//configuration.use_subuv				= false;
	
	compiler.begin_technique(/*billboard*/);
		compiler.begin_pass("particle_billboard", "particle_selected", configuration);
			compiler.set_depth( true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			//compiler.set_fill_mode(D3D_FILL_WIREFRAME);
			//compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
		compiler.end_pass();
	compiler.end_technique();
	
	configuration.use_subuv				= true;
	compiler.begin_technique(/*billboard_subuv*/);
		compiler.begin_pass("particle_billboard", "particle_selected", configuration);
			compiler.set_depth( true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			//compiler.set_fill_mode(D3D_FILL_WIREFRAME);
			//compiler.set_alpha_blend(true,D3D_BLEND_SRC_ALPHA,D3D_BLEND_INV_SRC_ALPHA);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
		compiler.end_pass();
	compiler.end_technique();
	
	compiler.begin_technique(/*beamtrail*/);
		compiler.begin_pass("particle_beamtrail", "particle_selected", configuration);
			compiler.set_depth( true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
			//compiler.set_fill_mode(D3D_FILL_WIREFRAME);
			compiler.set_alpha_blend(true,D3D_BLEND_ONE,D3D_BLEND_ONE);
		compiler.end_pass();
	compiler.end_technique();
#endif
}

} // namespace render 
} // namespace xray 
