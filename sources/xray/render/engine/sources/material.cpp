////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material.h"
#include "material_effects.h"
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/custom_config_value.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_texture.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "effect_gbuffer_nomaterial_materials.h"
#include "depth_accumulate_material_effect.h"
#include <xray/render/core/destroy_data_helper.h>

namespace xray {
namespace render {

static material_effects* s_nomaterial_material_effects[num_vertex_input_types];

bool material::is_nomaterial_material_ready()
{
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		if (!s_nomaterial_material_effects[i]->m_effects[geometry_render_stage].c_ptr())
			return false;
	}
	return true;
}

void material::initialize_nomaterial_material()
{
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		s_nomaterial_material_effects[i]									  = NEW(material_effects);
		s_nomaterial_material_effects[i]->m_vertex_input_type				  = (enum_vertex_input_type)i;
		
		byte data[Kb];
		xray::render::effect_options_descriptor								  additional_parameters(data, sizeof(data) );
		additional_parameters["vertex_input_type"]							  = (enum_vertex_input_type)i;
		additional_parameters["cull_mode"]									  = D3D_CULL_NONE;
		
		effect_manager::ref().create_effect<effect_gbuffer_nomaterial_materials>(
			&s_nomaterial_material_effects[i]->m_effects[geometry_render_stage], 
			additional_parameters
		);
		s_nomaterial_material_effects[i]->stage_enable[geometry_render_stage] = true;
		
		effect_manager::ref().create_effect<render::depth_accumulate_material_effect>(
			&s_nomaterial_material_effects[i]->m_effects[shadow_render_stage], 
			additional_parameters
		);
		s_nomaterial_material_effects[i]->stage_enable[shadow_render_stage] = true;
	}
}

material_effects& material::nomaterial_material(enum_vertex_input_type vi)
{
//	if (!s_nomaterial_material_effects[vi])
//		material::initialize_nomaterial_material();
	
	ASSERT_CMP(s_nomaterial_material_effects[vi], !=, 0);
	return *s_nomaterial_material_effects[vi];
}

void material::finalize_nomaterial_material()
{
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		DELETE(s_nomaterial_material_effects[i]);
		s_nomaterial_material_effects[i] = 0;
	}
}

} // namespace render
} // namespace xray


