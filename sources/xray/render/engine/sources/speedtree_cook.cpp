////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "speedtree_cook.h"
#include "speedtree.h"
#include "speedtree_forest.h"
#include "speedtree_instance_impl.h"
#include "material.h"
#include "speedtree_tree_component_billboard.h"
#include <xray/render/facade/model.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include "material_effects.h"
#include <xray/type_variant.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include "material_manager.h"
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

using namespace resources;

speedtree_cook::speedtree_cook(): 
	super(speedtree_class, reuse_true, use_current_thread_id)
{}

void speedtree_cook::translate_query(resources::query_result_for_cook&	parent)
{
 	fs_new::virtual_path_string		model_path;
	model_path.assignf				("resources/speedtree/%s.srt", parent.get_requested_path());

	fs_new::virtual_path_string		config_path;
	config_path.assignf				("resources/speedtree/%s.options", parent.get_requested_path());
	
	resources::request	requests[]={
		{model_path.c_str(), resources::raw_data_class},
		{config_path.c_str(), resources::binary_config_class}
	};
	speedtree_data* creation_data = NEW(speedtree_data)();
	creation_data->m_parent_query	= &parent;
	creation_data->m_failed			= false;

 	resources::query_resources(
		requests,
 		boost::bind	(&speedtree_cook::on_speedtree_raw_data_loaded, this, _1, creation_data ),
 		g_allocator,
		0,
		& parent
	);
}

void speedtree_cook::on_speedtree_raw_data_loaded(resources::queries_result& data, speedtree_data* creation_data)
{
 	if(!data[0].is_successful())
	{
		creation_data->m_parent_query->finish_query	( result_error );
		DELETE										( creation_data );
		return;
	}
	
	resources::pinned_ptr_const<u8> raw_data_ptr(data[0].get_managed_resource());
	
	speedtree_tree* st_tree			= NEW(speedtree_tree)(raw_data_ptr.c_ptr(), raw_data_ptr.size());
	
	creation_data->m_model			= st_tree;
	
 	if (!data[1].is_successful())
	{
		query_materail_effects		(creation_data);
		return;
	}
	
	configs::binary_config_ptr model_config = static_cast_resource_ptr<configs::binary_config_ptr>(data[1].get_unmanaged_resource());
	configs::binary_config_value const& root = model_config->get_root();
	
	resources::request materials_request[num_speedtree_components]= {
		{strings::length(root["branch"]["material"]) ? pcstr(pcstr(root["branch"]["material"])) : "nomaterial", resources::material_class},
		{strings::length(root["frond"]["material"]) ? pcstr(pcstr(root["frond"]["material"])) : "nomaterial", resources::material_class},
		{strings::length(root["leafmesh"]["material"]) ? pcstr(pcstr(root["leafmesh"]["material"])) : "nomaterial", resources::material_class},
		{strings::length(root["leafcard"]["material"]) ? pcstr(pcstr(root["leafcard"]["material"])) : "nomaterial", resources::material_class},
		{strings::length(root["billboard"]["material"]) ? pcstr(pcstr(root["billboard"]["material"])) : "nomaterial", resources::material_class},
	};
	resources::query_resources(
		materials_request,
 		boost::bind	(&speedtree_cook::on_model_materials_loaded, this, _1, creation_data ),
 		g_allocator,
		0,
		creation_data->m_parent_query
	);
}

void  speedtree_cook::on_model_materials_loaded( resources::queries_result& data, speedtree_data* d )
{
	bool has_valid_material			= false;
	for(int i=0; i<num_speedtree_components; ++i)
	{
		if(data[i].is_successful())
		{
			d->m_materials[i]		= static_cast_resource_ptr<material_ptr>(data[i].get_unmanaged_resource());
			d->m_material_path[i]	= data[i].get_requested_path();
			has_valid_material		= true;
		}
	}
	
	if (has_valid_material)
		query_materail_effects		(d);
	else
		finish_model_creation		(d);
}

void speedtree_cook::query_materail_effects( speedtree_data* cook_data )
{
	if(cook_data->m_failed)
	{
		cook_data->m_parent_query->finish_query			( result_error );
		DELETE											( cook_data );
		return;
	}
	
	enum_vertex_input_type speedtree_vertex_types[num_speedtree_components] = 
	{
		speedtree_branch_vertex_input_type,
		speedtree_frond_vertex_input_type,
		speedtree_leafmesh_vertex_input_type,
		speedtree_leafcard_vertex_input_type,
		speedtree_billboard_vertex_input_type
	};
	resources::request				requests[num_speedtree_components];
	resources::user_data_variant	user_data[num_speedtree_components];
	resources::user_data_variant*	user_data_ptrs[num_speedtree_components];
	
	for (u32 i = 0; i < num_speedtree_components; i++)
	{
		user_data[i].set			(NEW(material_effects_instance_cook_data)(
			speedtree_vertex_types[i], 
			static_cast_resource_ptr<resources::unmanaged_resource_ptr>(cook_data->m_materials[i]),
			true, 
			cull_mode_none)
		);
		user_data_ptrs[i]			= &user_data[i];
		requests[i].id				= material_effects_instance_class;
		requests[i].path			= cook_data->m_materials[i] ? cook_data->m_material_path[i].c_str() : "nomaterial";
	}
	resources::query_resources		(
		requests,
		boost::bind(&speedtree_cook::on_material_effects_created, this, _1, cook_data),
		g_allocator,
		(resources::user_data_variant const**)&user_data_ptrs[0],
		cook_data->m_parent_query);
}

void speedtree_cook::on_material_effects_created( resources::queries_result& data, speedtree_data* d )
{
	for(int i = 0; i < 5; i++)
		if(data[i].is_successful())
			d->m_material_effects[i] = static_cast_resource_ptr<material_effects_instance_ptr>(data[i].get_unmanaged_resource());
	
	finish_model_creation			(d);
}

void  speedtree_cook::finish_model_creation( speedtree_data* d )
{
	if(d->m_failed)
	{
		d->m_parent_query->finish_query			( result_error );
	}else
	{
		speedtree_tree* st_tree = static_cast_checked<speedtree_tree*>(d->m_model.c_ptr());
		if( st_tree->m_branch_component )
			if(d->m_material_effects[0] && d->m_materials[0])
				st_tree->m_branch_component->set_material_effects(d->m_material_effects[0], d->m_materials[0]->get_material_name());
			else
				st_tree->m_branch_component->set_default_material();
	
		if( st_tree->m_frond_component )
			if(d->m_material_effects[1] && d->m_materials[1])
				st_tree->m_frond_component->set_material_effects(d->m_material_effects[1], d->m_materials[1]->get_material_name());
			else
				st_tree->m_frond_component->set_default_material();

		if( st_tree->m_leafmesh_component )
			if(d->m_material_effects[2] && d->m_materials[2])
				st_tree->m_leafmesh_component->set_material_effects(d->m_material_effects[2], d->m_materials[2]->get_material_name());
			else
				st_tree->m_leafmesh_component->set_default_material();
		
		if( st_tree->m_leafcard_component )
			if(d->m_material_effects[3] && d->m_materials[3])
				st_tree->m_leafcard_component->set_material_effects(d->m_material_effects[3], d->m_materials[3]->get_material_name());
			else
				st_tree->m_leafcard_component->set_default_material();

		if( st_tree->m_billboard_component )
			if(d->m_material_effects[4] && d->m_materials[4])
				st_tree->m_billboard_component->set_material_effects(d->m_material_effects[4], d->m_materials[4]->get_material_name());
			else
				st_tree->m_billboard_component->set_default_material();

		d->m_parent_query->set_unmanaged_resource( d->m_model.c_ptr(), resources::nocache_memory, sizeof(speedtree_tree));
		d->m_parent_query->finish_query			( result_success );
	}
	DELETE		( d );
}


void speedtree_cook::delete_resource(resources::resource_base* resource)
{
	speedtree_tree* st_tree = static_cast<speedtree_tree*>(resource);
	DELETE(st_tree);
}



speedtree_instance_cook::speedtree_instance_cook(): 
	super(speedtree_instance_class, reuse_false, use_current_thread_id) 
{}

void speedtree_instance_cook::translate_query(resources::query_result_for_cook&	parent)
{
 	resources::query_resource(
 		parent.get_requested_path(),
		speedtree_class,
 		boost::bind	(&speedtree_instance_cook::on_speedtree_loaded, this, _1, &parent ),
 		xray::render::g_allocator,
		0,
		& parent
	);
}

void speedtree_instance_cook::on_speedtree_loaded(resources::queries_result& data, resources::query_result_for_cook* parent_query)
{
	if (!data[0].is_successful())
	{
		parent_query->finish_query				(result_error);
		return;
	}

	speedtree_tree_ptr st_tree_ptr				= static_cast_resource_ptr<speedtree_tree_ptr>(data[0].get_unmanaged_resource());
	
	speedtree_instance_ptr st_instance_ptr		= NEW(speedtree_instance_impl)(st_tree_ptr.c_ptr());
	
	// TODO: collision geometry
	
	parent_query->set_unmanaged_resource		(st_instance_ptr.c_ptr(), resources::nocache_memory, sizeof(speedtree_instance_impl));
	parent_query->finish_query					(result_success);
}

void speedtree_instance_cook::delete_resource(resources::resource_base* resource)
{
	speedtree_instance_impl* st_instance = static_cast<speedtree_instance_impl*>(resource);
	DELETE(st_instance);
}
} // namespace render
} // namespace xray
