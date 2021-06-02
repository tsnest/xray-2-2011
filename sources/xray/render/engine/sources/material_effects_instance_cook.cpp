////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_effects_instance_cook.h"
#include <xray/render/core/effect_cook.h>
#include "material.h"
#include "material_effects.h"
#include <xray/render/core/res_texture.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/destroy_data_helper.h>
#include <xray/render/core/effect_manager.h>
#include "material_manager.h"
#include <xray/configs_binary_config_value.h>
#include <xray/render/facade/render_stage_types.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include <xray/render/core/untyped_buffer.h>

static xray::command_line::key s_sync_effects_creation_key( "sync_effects_creation", "", "", "enable sync effects creation" );

namespace xray {
namespace render {

material_effects_instance_cook::material_effects_instance_cook(): 
	resources::translate_query_cook(
		resources::material_effects_instance_class, 
		reuse_false, 
		use_current_thread_id
	)
{	
}

// TODO: remove this functions!
static void replace_value_data(custom_config_value const& to, custom_config_value from)
{ 
	custom_config_value& w_value	= const_cast<custom_config_value&>(to);
	(u16&)w_value.count				= from.count;
	(u16&)w_value.type				= from.type;
	pcvoid& w_data					= *(pcvoid*)&w_value.data;
	*(pvoid*)&w_data				= *(pvoid*)&from.data;
}

/*
	param = { value = "123", } => param = "123"
*/
static void replace_values(custom_config_value const& in_v)
{
	if ((in_v.type==configs::t_table_indexed || in_v.type==configs::t_table_named))
	{
		if ( in_v.value_exists("value"))
		{
			replace_value_data(in_v, in_v["value"]);
		}
		else
		{
			for (custom_config_value::const_iterator it=in_v.begin(); it!=in_v.end(); ++it)
				replace_values(*it);
		}
	}
}

static void merge_configs(custom_config_value const*& current, 
						  custom_config_value const*& values, 
						  custom_config_value const& base, 
						  custom_config_value const* replace)
{
	if (base.type==xray::configs::t_table_named || base.type==xray::configs::t_table_indexed)
	{
		xray::memory::copy((pvoid)current, sizeof(custom_config_value), (pvoid)&base, sizeof(custom_config_value));
		
		custom_config_value* root_values = const_cast<custom_config_value*>(current);
		//values++;
		
		root_values->count = u16(base.size());
		root_values->data  = (pvoid)values;
		custom_config_value const* data_values = values;
		values+=root_values->count;
		
		if (replace)
		{
			for (custom_config_value::const_iterator it=replace->begin(); it!=replace->end(); ++it)
			{
				if (!base.value_exists(it->id))
				{
					root_values->count++;
					xray::memory::copy((pvoid)values, sizeof(custom_config_value), (pvoid)it, sizeof(custom_config_value));
					values++;
				}
			}
		}
		
		for (custom_config_value::const_iterator it=base.begin(); it!=base.end(); ++it)
		{
			if (replace && replace->value_exists(it->id))
				merge_configs(data_values,values,*it,&(*replace)[it->id]);
			else
				merge_configs(data_values,values,*it,0);
			
			data_values++;
		}
	}
	else
		xray::memory::copy((pvoid)current, sizeof(custom_config_value), (pvoid)&base, sizeof(custom_config_value));
}

static u32 get_num_fields(custom_config_value const& value)
{
	u32 result = 1;

	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
		for (custom_config_value::const_iterator it=value.begin(); it!=value.end(); ++it)
			result += get_num_fields(*it);

	return result;
}

static custom_config_ptr merge_effect_options(custom_config_value const& effect_config, render::effect_options_descriptor const& desc, u32& out_crc)
{
	u32 crc = 0;
	custom_config_ptr C = create_custom_config(desc, crc, false);
	
	u32 num_fields = get_num_fields(effect_config) + get_num_fields(C->root());
	custom_config_value const* values = (custom_config_value*)ALLOCA( sizeof(custom_config_value) * num_fields * 5 );
	custom_config_value const* values_root = values;
	custom_config_value* values_init	   = const_cast<custom_config_value*>(values);
	
	for (u32 i=0; i<num_fields; i++)
		new(values_init++)custom_config_value;
	
	xray::memory::copy((pvoid)values, sizeof(custom_config_value), (pvoid)&effect_config, sizeof(custom_config_value));
	custom_config_value const* values2 = values + 1;
	
	merge_configs(values, values2, effect_config, &C->root());
	
	return create_custom_config(*values_root, out_crc, false);	
}

void material_effects_instance_cook::on_material_ready(resources::queries_result& data, 
													   material_effects_instance_cook_data* cook_data)
{
	if (data[0].is_successful())
	{
		cook_data->material						= data[0].get_unmanaged_resource();
		query_effects							(*data.get_parent_query(), cook_data);
	}
	else
	{
		data.get_parent_query()->finish_query	(result_error, assert_on_fail_false);
		
		if (cook_data->delete_in_cook)
			DELETE								(cook_data);
	}
}

static D3D_CULL_MODE base_cull_mode_to_d3d_cull_mode(enum_cull_mode cull_mode)
{
	switch (cull_mode)
	{
		case cull_mode_none:	return D3D_CULL_NONE;
		case cull_mode_back:	return D3D_CULL_BACK;
		case cull_mode_front:	return D3D_CULL_FRONT;
		default:				NODEFAULT(return D3D_CULL_NONE);
	}
}

void material_effects_instance_cook::query_effects(resources::query_result_for_cook& parent, material_effects_instance_cook_data* cook_data)
{
	enum_vertex_input_type	vertex_type			= cook_data->vertex_input_type;
	material_ptr			material			= static_cast_resource_ptr<material_ptr>(cook_data->material);
	
	u32 crc										= 0;
	custom_config_ptr material_config			= create_custom_config(material->m_config->get_root()["material"], crc, false);
	
	bool is_two_sided							= false;
	bool from_material							= false;
	
	if (cook_data->cull_mode != cull_mode_none && 
		material_config->root().value_exists("g_stage") && material_config->root()["g_stage"].value_exists("effect"))
	{
		if (material_config->root()["g_stage"]["effect"].value_exists("two_sided"))
		{
			is_two_sided						= bool(material_config->root()["g_stage"]["effect"]["two_sided"]["value"]);
			from_material						= true;
		}
	}
	
	byte data[Kb];
	xray::render::effect_options_descriptor		additional_parameters(data, sizeof(data) );
	additional_parameters["vertex_input_type"]	= vertex_type;
	
	if (from_material)
	{
		additional_parameters["cull_mode"]		= base_cull_mode_to_d3d_cull_mode(is_two_sided ? cull_mode_none : cull_mode_back);
	}
	else
	{
		additional_parameters["cull_mode"]		= base_cull_mode_to_d3d_cull_mode(cook_data->cull_mode);
	}
	
	replace_values								(material_config->root());
	
	u32 const num_requests						= num_render_stages;
	custom_config_value const& root_config		= material_config->root();
	
	resources::user_data_variant*	user_data_variants		= (resources::user_data_variant*)MALLOC(sizeof(resources::user_data_variant) * num_requests, "");
	resources::user_data_variant**	user_data_variants_ptrs	= (resources::user_data_variant**)MALLOC(sizeof(resources::user_data_variant*) * num_requests, "");
	resources::creation_request*	requests				= (resources::creation_request*)MALLOC(sizeof(resources::creation_request) * num_requests, "");
	
	for (u32 i = 0; i < num_requests; i++)
	{
		user_data_variants_ptrs[i]							= new(&user_data_variants[i])resources::user_data_variant;
		new(&requests[i])resources::creation_request		("", xray::const_buffer("", 1), resources::render_effect_class);
	}
	
	gather_request_user_data					(user_data_variants, &root_config, &additional_parameters);
	
	if (s_sync_effects_creation_key.is_set())
	{
		resources::query_create_resources_and_wait(
			requests,
			num_requests,
			boost::bind(&material_effects_instance_cook::on_effect_ready, this, _1, cook_data),
			xray::render::g_allocator,
			(resources::user_data_variant const**)user_data_variants_ptrs,
			&parent
		);
	}
	else
	{
		resources::query_create_resources		(
			requests,
			num_requests,
			boost::bind(&material_effects_instance_cook::on_effect_ready, this, _1, cook_data),
			xray::render::g_allocator,
			(resources::user_data_variant const**)user_data_variants_ptrs,
			&parent
		);
	}

	FREE										(user_data_variants);
	FREE										(user_data_variants_ptrs);
	FREE										(requests);
}

void material_effects_instance_cook::translate_query(resources::query_result_for_cook& parent)
{
	ASSERT_CMP									(parent.user_data(), !=, NULL);
	
	material_effects_instance_cook_data*		cook_data = NULL;
	parent.user_data()->try_get					(cook_data);
	
	ASSERT_CMP									(cook_data, !=, NULL);
	
	if (!cook_data->material)
	{
		resources::query_resource				(
			parent.get_requested_path(),
			resources::material_class,
			boost::bind(
				&material_effects_instance_cook::on_material_ready, 
				this, 
				_1, 
				cook_data
			),
			xray::render::g_allocator,
			0,
			&parent
		);
		return;
	}
	else
	{
		query_effects							(parent, cook_data);
	}
}

void material_effects_instance_cook::on_effect_ready(resources::queries_result& data, material_effects_instance_cook_data* cook_data)
{
	resources::query_result_for_cook& parent	= *data.get_parent_query();
	
	material_effects_instance* new_resource		= NEW(material_effects_instance);
	
	new_resource->m_material_name				= static_cast_resource_ptr<material_ptr>(cook_data->material)->get_material_name();
	
	material_effects& mtl_effects				= new_resource->get_material_effects();
	mtl_effects.m_vertex_input_type				= cook_data->vertex_input_type;
	
	for (u32 i = 0; i < num_render_stages; i++)
	{
		mtl_effects.stage_enable[i]				= data[i].is_successful();
		
		if (i == geometry_render_stage)
		{
			xray::configs::binary_config_value const& mtl_config = 
				static_cast_resource_ptr<material_ptr>(cook_data->material)->m_config->get_root()["material"];
			
			if (mtl_config.value_exists("g_stage") && mtl_config["g_stage"].value_exists("is_emissive"))
				mtl_effects.is_emissive			= (bool)mtl_config["g_stage"]["is_emissive"]["value"];
			
			if (mtl_config.value_exists("g_stage") && mtl_config["g_stage"].value_exists("is_cast_shadow"))
				mtl_effects.is_cast_shadow		= (bool)mtl_config["g_stage"]["is_cast_shadow"]["value"];
		}
		
		if (i == lighting_render_stage)
		{
			xray::configs::binary_config_value const& mtl_config = 
				static_cast_resource_ptr<material_ptr>(cook_data->material)->m_config->get_root()["material"];
			
			if (mtl_config.value_exists("lighting") && mtl_config["lighting"].value_exists("is_organic"))
				mtl_effects.is_organic			= (bool)mtl_config["lighting"]["is_organic"]["value"];

			if (mtl_config.value_exists("lighting") && mtl_config["lighting"].value_exists("constant_clear_color"))
				mtl_effects.organic_clear_color	= float4(mtl_config["lighting"]["constant_clear_color"]["value"]);
		}
		
		if (data[i].is_successful())
			mtl_effects.m_effects[i]			= static_cast_resource_ptr<ref_effect>(data[i].get_unmanaged_resource());
			
		mtl_effects.stage_enable[i]				= mtl_effects.stage_enable[i] && mtl_effects.m_effects[i] != NULL;
	}
	
	parent.set_unmanaged_resource				(new_resource, resources::nocache_memory, sizeof(material_effects_instance));
	parent.finish_query							(result_success);
	
	if (cook_data->delete_in_cook)
		DELETE									(cook_data);
}

void material_effects_instance_cook::gather_request_user_data(resources::user_data_variant*		user_data, 
															  custom_config_value const*		root_config,
															  effect_options_descriptor const*	additional_parameters)
{
	bool				has_gstage				= false;
	
	custom_config_ptr	gstage_config;
	u32					gstage_config_crc		= 0;
	
	for (u32 i = 0; i < num_render_stages; i++)
	{
		enum_render_stage_type type				= (enum_render_stage_type)i;
		pcstr stage_name						= stage_type_to_string(type);
		
		if (!(*root_config).value_exists(stage_name))
		{
			user_data[i].set					((effect_compile_data*)0);
		}
		else
		{
			if (i == geometry_render_stage/* || i == forward_render_stage*/)
				has_gstage						= true;
			
			effect_descriptor* descriptor		= effect_manager::ref().get_effect_descriptor_by_name(
				pcstr((*root_config)[stage_name]["effect"]["effect_id"])
			);
			u32 crc;
			custom_config_ptr config			= merge_effect_options((*root_config)[stage_name]["effect"], *additional_parameters, crc);
			effect_compile_data* cook_data		= NEW(effect_compile_data)(descriptor, config, crc);
			user_data[i].set					(cook_data);
			
			if (i == geometry_render_stage/* || i == forward_render_stage*/)
			{
				gstage_config					= config;
				gstage_config_crc				= crc;
			}
		}
	}
	
	if (has_gstage)
	{
		user_data[shadow_render_stage].set		(
			NEW(effect_compile_data)(
				effect_manager::ref().get_effect_descriptor_by_name("shadow_accumulate"), 
				gstage_config,
				gstage_config_crc
			)
		);
	}
}

void material_effects_instance_cook::delete_resource(resources::resource_base * resource)
{
	material_effects_instance* instance = static_cast<material_effects_instance*>(resource);
	DELETE(instance);
}

} // namespace render
} // namespace xray
