////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "animated_model_instance_cook.h"
#include "animated_model_instance.h"
#include <xray/render/facade/model.h>
#include <xray/physics/model.h>
#include <xray/collision/api.h>

namespace xray {
namespace physics {
	struct world;
} // namespace physics
} // namespace xray

namespace stalker2 {

animated_model_instance_cook::animated_model_instance_cook	( ) :
	super	( resources::game_animated_model_instance_class, reuse_false, use_resource_manager_thread_id )	
{
}

void animated_model_instance_cook::translate_request_path	( pcstr request, fs_new::virtual_path_string & new_request ) const
{
	new_request.assignf( "resources/animated_model_instances/%s.animated_model", request );
}

void animated_model_instance_cook::translate_query	( resources::query_result_for_cook&	parent )
{
	resources::query_resource						(
		parent.get_requested_path(),
		resources::binary_config_class,
		boost::bind( &animated_model_instance_cook::on_config_loaded, this, _1 ),
		resources::unmanaged_allocator(),
		0,
		&parent
	);
}

void animated_model_instance_cook::on_config_loaded		( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT										( data.is_successful(), "couldn't retrieve config for game animated model" );
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr config					= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& config_value	= config->get_root();
	configs::binary_config_value const& models			= config_value["models"];
	
	R_ASSERT											( models.value_exists( "render_animated_model" ) );
	pcstr render_model_path								= models["render_animated_model"];

	R_ASSERT											( models.value_exists( "physics_animated_model" ) );
	pcstr physics_model_path							= models["physics_animated_model"];
	
	R_ASSERT											( models.value_exists( "damage_collision_object" ) );
	pcstr damage_collision_path							= models["damage_collision_object"];
	fs_new::virtual_path_string							damage_config_path;
	damage_config_path.assignf							( "resources/animated_model_instances/collision_objects/%s.physics", damage_collision_path );
	
	R_ASSERT											( parent );
	resources::user_data_variant* user_data				= parent->user_data();
	R_ASSERT											( user_data );
	physics::world* physics								= 0;
	bool const res_user_data							= user_data->try_get( physics );
	R_ASSERT											( res_user_data );
	R_ASSERT											( physics );

	resources::user_data_variant						physics_world;
	physics_world.set									( physics );

	resources::user_data_variant const* params[] =
	{
		0,
		&physics_world,
		0
	};

	resources::request requests[] =
	{
		{ render_model_path, resources::render_animated_model_instance_class },
		{ physics_model_path, resources::physics_animated_model_instance_class },
		{ damage_config_path.c_str(), resources::binary_config_class }
	};
	
	query_resources										(
		requests,
		array_size( requests ),
		boost::bind( &animated_model_instance_cook::on_subresources_loaded, this, _1 ), 
		resources::unmanaged_allocator(),
		params,
		parent
	);
}

void animated_model_instance_cook::on_subresources_loaded	( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT									( data.is_successful(), "couldn't cook some of game animated model subresources" );
		parent->finish_query						( result_error );
		return;
	}

	animated_model_instance* new_model_instance		= XRAY_NEW_IMPL( resources::unmanaged_allocator(), animated_model_instance );
	new_model_instance->m_render_model				= static_cast_resource_ptr< render::animated_model_instance_ptr >( data[0].get_unmanaged_resource() );
	new_model_instance->m_physics_model				= static_cast_resource_ptr< physics::animated_model_instance_ptr >( data[1].get_unmanaged_resource() );

	configs::binary_config_ptr collision_config		= static_cast_resource_ptr< configs::binary_config_ptr >( data[2].get_unmanaged_resource() );
	new_model_instance->m_damage_collision			= collision::new_animated_object(
														collision_config->get_root(),
														new_model_instance->m_physics_model->m_skeleton,
														resources::unmanaged_allocator()
													);
	xray::animation::animation_player* player		= XRAY_NEW_IMPL( resources::unmanaged_allocator(), xray::animation::animation_player );
	new_model_instance->m_animation_player			= player;

	parent->set_unmanaged_resource					(
		new_model_instance, 
		resources::memory_usage_type(
			resources::nocache_memory,
			sizeof( animated_model_instance )
		)
	);
	parent->finish_query							( result_success );
}

void animated_model_instance_cook::delete_resource	( resources::resource_base* resource )
{
	animated_model_instance* model_instance			= static_cast_checked< animated_model_instance* >( resource );
	collision::delete_animated_object				( model_instance->m_damage_collision, resources::unmanaged_allocator() );
	xray::animation::animation_player* player		= model_instance->m_animation_player;
	XRAY_DELETE_IMPL								( resources::unmanaged_allocator(), player );
	XRAY_DELETE_IMPL								( resources::unmanaged_allocator(), model_instance );
}

} // namespace stalker2