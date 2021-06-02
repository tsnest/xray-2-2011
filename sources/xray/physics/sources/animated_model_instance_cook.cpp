////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "animated_model_instance_cook.h"
#include <xray/physics/model.h>
#include <xray/physics/world.h>
#include <xray/collision/api.h>

namespace xray {
namespace physics {

animated_model_instance_cook::animated_model_instance_cook( )
:super	( resources::physics_animated_model_instance_class, reuse_false, use_current_thread_id  ),
m_allocator( g_ph_allocator )
{
}

void animated_model_instance_cook::translate_request_path	( pcstr request, fs_new::virtual_path_string& new_request ) const
{
	new_request.assignf( "resources/animated_model_instances/physics_animated_models/%s.physics_model", request );
}

void animated_model_instance_cook::translate_query		( resources::query_result_for_cook&	parent )
{
	resources::query_resource			(
		parent.get_requested_path(),
		resources::binary_config_class,
		boost::bind( &animated_model_instance_cook::on_config_loaded, this, _1 ),
		m_allocator,
		0,
		&parent
	);
}

void animated_model_instance_cook::on_config_loaded		( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr config					= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& config_value	= config->get_root();
	configs::binary_config_value const& attributes		= config_value["attributes"];
	
	pcstr skeleton_path									= attributes["skeleton"];
	fs_new::virtual_path_string								skeleton_config_path;
	skeleton_config_path.assignf						( "resources/models/%s.skinned_model/skeleton", skeleton_path );

	resources::query_resource							(
		skeleton_config_path.c_str(),
		resources::binary_config_class,
		boost::bind( &animated_model_instance_cook::on_skeleton_config_loaded, this, _1, config ),
		m_allocator,
		0,
		parent
	);
}

void animated_model_instance_cook::on_skeleton_config_loaded( resources::queries_result& data, configs::binary_config_ptr config )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr skeleton_config			= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& skeleton_value	= skeleton_config->get_root();
	configs::binary_config_value const& config_value	= config->get_root();
	configs::binary_config_value const& attributes		= config_value["attributes"];
	
	R_ASSERT											( skeleton_value.value_exists( "skeleton" ) );
	pcstr skeleton_path									= skeleton_value["skeleton"];

	R_ASSERT											( attributes.value_exists( "collision_object" ) );
	pcstr collision_object_path							= attributes["collision_object"];
	fs_new::virtual_path_string							collision_config_path;
	collision_config_path.assignf						( "resources/animated_model_instances/collision_objects/%s.physics", collision_object_path );


	R_ASSERT											( attributes.value_exists( "ragdoll_object" ) );
	pcstr ragdol_object_path							= attributes["ragdoll_object"];
	fs_new::virtual_path_string							ragdoll_config_path;
	ragdoll_config_path.assignf							( "resources/animated_model_instances/collision_objects/%s.physics", ragdol_object_path );
	//ragdoll_config_path.assignf							( "resources/animated_model_instances/collision_objects/human_ragdoll.physics");

	resources::request requests[] =
	{
		{ skeleton_path, resources::skeleton_class },
		{ collision_config_path.c_str(), resources::binary_config_class },
		{ ragdoll_config_path.c_str(), resources::binary_config_class }
	};
	
	query_resources										(
		requests,
		array_size( requests ),
		boost::bind( &animated_model_instance_cook::on_subresources_loaded, this, _1 ), 
		m_allocator,
		0,
		parent
	);
}

void animated_model_instance_cook::on_subresources_loaded	( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		parent->finish_query						( result_error );
		return;
	}

	animated_model_instance* new_model_instance		= XRAY_NEW_IMPL( m_allocator, animated_model_instance );
	new_model_instance->m_skeleton					= static_cast_resource_ptr< animation::skeleton_ptr >( data[0].get_unmanaged_resource() );

	configs::binary_config_ptr collision_config		= static_cast_resource_ptr< configs::binary_config_ptr >( data[1].get_unmanaged_resource() );
	new_model_instance->m_collision					= collision::new_animated_object( collision_config->get_root(), new_model_instance->m_skeleton, m_allocator );
	
	R_ASSERT( parent );
	resources::user_data_variant* user_data = parent->user_data();
	R_ASSERT( user_data );
	world *physics_world = 0;
	bool res_user_data = user_data->try_get( physics_world );
	R_ASSERT( res_user_data );
	R_ASSERT( physics_world );
	
	configs::binary_config_ptr ragdoll_config		= static_cast_resource_ptr< configs::binary_config_ptr >( data[2].get_unmanaged_resource() );
	//new_model_instance->m_physics					= &(*physics_world->new_physics_shell( ragdoll_config->get_root() ));
	//physics_world->set_ready( true );

	parent->set_unmanaged_resource					(
				new_model_instance, 
				resources::memory_usage_type		( resources::nocache_memory, sizeof( animated_model_instance ) )
			);
	parent->finish_query							( result_success );
}

void animated_model_instance_cook::delete_resource	( resources::resource_base* resource )
{
	animated_model_instance* model_instance			= static_cast_checked< animated_model_instance* >( resource );
	collision::delete_animated_object				( model_instance->m_collision, m_allocator );
	XRAY_DELETE_IMPL								( m_allocator, model_instance );
}

} // namespace physics
} // namespace xray
