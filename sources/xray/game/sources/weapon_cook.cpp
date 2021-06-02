////////////////////////////////////////////////////////////////////////////
//	Created		: 13.11.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "weapon_cook.h"
#include "game.h"
#include "weapon.h"

namespace stalker2{
weapon_cook::weapon_cook(  game& game )
:super	( resources::weapon_class, reuse_false, use_current_thread_id ),
m_game	( game )
{}

void weapon_cook::translate_query( resources::query_result_for_cook& parent )
{
	fs_new::virtual_path_string	weapon_config_name;
	weapon_config_name.assignf("resources/weapons/%s.options", parent.get_requested_path());

	resources::query_resource(
		weapon_config_name.c_str(),
		resources::binary_config_class,
		boost::bind( &weapon_cook::on_weapon_config_ready, this, _1, &parent ),
		g_allocator,
		NULL,
		&parent
	);
}

void weapon_cook::on_weapon_config_ready( resources::queries_result& data, resources::query_result_for_cook* parent )
{
	R_ASSERT(data.is_successful());
	configs::binary_config_ptr config_ptr = static_cast_resource_ptr<configs::binary_config_ptr>(data[0].get_unmanaged_resource());

	configs::binary_config_value const& config = config_ptr->get_root();
	
	resources::request r[]={
		// base part(animated)
		{ config["base"]["model"],						resources::skeleton_model_instance_class },
		{ "resources/animations/single/weapon/idle",	resources::animation_class },
		{ "resources/animations/single/weapon/reload",	resources::animation_class },
		{ "resources/animations/single/weapon/shoot",	resources::animation_class },

		{ config["cover"]["model"],						resources::static_model_instance_class },
		{ config["stock"]["model"],						resources::static_model_instance_class },
		{ config["handle"]["model"],					resources::static_model_instance_class },
		{ config["ammo"]["model"],						resources::static_model_instance_class },
		{ config["bolt"]["model"],						resources::static_model_instance_class },
		{ config["fore_end"]["model"],					resources::static_model_instance_class },
		{ config["barrel"]["model"],					resources::static_model_instance_class },
		{ config["barrel_end"]["model"],				resources::static_model_instance_class },
	};
	resources::query_resources(
		r,
		boost::bind(&weapon_cook::on_weapon_parts_ready, this, _1, config_ptr, parent ),
		g_allocator
		);
}

void weapon_cook::on_weapon_parts_ready( resources::queries_result& data, 
										configs::binary_config_ptr config_ptr,
										resources::query_result_for_cook* parent )
{
	R_ASSERT(data.is_successful());

	configs::binary_config_value const& config = config_ptr->get_root();
	u32 resource_idx =0;
	render::skeleton_model_ptr	base_model			= static_cast_resource_ptr<render::skeleton_model_ptr>(data[resource_idx].get_unmanaged_resource());
	animation::skeleton_animation_ptr idle_anim		= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[++resource_idx].get_managed_resource());
	animation::skeleton_animation_ptr reload_anim	= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[++resource_idx].get_managed_resource());
	animation::skeleton_animation_ptr shoot_anim	= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[++resource_idx].get_managed_resource());

	weapon* w			= NEW(weapon)();

	w->m_base->load	( config["base"], base_model, idle_anim, reload_anim, shoot_anim );

	w->m_cover->load	( config["cover"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_stock->load	( config["stock"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_handle->load	( config["handle"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_ammo->load	( config["ammo"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_bolt->load	( config["bolt"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_fore_end->load	( config["fore_end"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_barrel->load	( config["barrel"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	w->m_barrel_end->load	( config["barrel_end"], 
		static_cast_resource_ptr<render::static_model_ptr>(data[++resource_idx].get_unmanaged_resource()) );

	parent->set_unmanaged_resource			(
				w, 
				resources::memory_usage_type( resources::nocache_memory, sizeof( weapon ) )
			);

	parent->finish_query					( result_success );
}

void weapon_cook::delete_resource( resources::resource_base* resource )
{
 	XRAY_DELETE_IMPL					( g_allocator, resource );
}


}