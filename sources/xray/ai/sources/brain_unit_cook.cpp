////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "brain_unit_cook.h"
#include "brain_unit.h"
#include <xray/ai/navigation_environment.h>
#include <xray/ai/npc.h>
#include <xray/ai/world.h>

namespace xray {
namespace ai {

brain_unit_cook::brain_unit_cook	( ai_world* world ) :
	translate_query_cook			( resources::brain_unit_class, reuse_false, use_current_thread_id ),
	m_ai_world						( world )
{
}

void brain_unit_cook::translate_request_path	( pcstr request, fs_new::virtual_path_string& new_request ) const
{
	new_request.assignf					( "resources/brain_units/%s.brain_unit", request );
}

void brain_unit_cook::translate_query	( resources::query_result_for_cook& parent )
{
	resources::query_resource			(
		parent.get_requested_path(),
		resources::binary_config_class,
		boost::bind( &brain_unit_cook::on_brain_unit_options_received, this, _1 ),
		g_allocator,
		parent.user_data(),
		&parent
	);
}

void brain_unit_cook::delete_resource	( resources::resource_base* resource )
{
	XRAY_DELETE_IMPL					( g_allocator, resource );
}

void brain_unit_cook::on_brain_unit_options_received	( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT							( data.is_successful(), "couldn't retrieve some of brain_unit options" );
		parent->finish_query				( result_error );
		return;
	}
	
	configs::binary_config_ptr config		= static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() );
	
	brain_unit_cook_params					cook_params;
	resources::user_data_variant* user_data	= parent->user_data();
	R_ASSERT								( user_data );
	bool const user_data_result				= user_data->try_get( cook_params );
	R_ASSERT								( user_data_result );
		
	query_resource							(
		parent->get_requested_path(),
		cook_params.world_user_type,
		boost::bind( &brain_unit_cook::on_sound_player_loaded, this, _1, config ),
		g_allocator,
		0,
		parent
	);
}

void brain_unit_cook::on_sound_player_loaded	( resources::queries_result& data, configs::binary_config_ptr config )	
{   
	resources::query_result_for_cook* const	parent = data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT								( data.is_successful(), "couldn't create ai sound player" );
		parent->finish_query					( result_error );
		return;
	}
	
	ai::sound_player_ptr sound_player			= static_cast_resource_ptr< ai::sound_player_ptr >( data[0].get_unmanaged_resource() );
	
	navigation_environment						navigation;
	configs::binary_config_value const& value	= config->get_root();
	
	brain_unit_cook_params						brain_cook_params;
	resources::user_data_variant* user_data		= parent->user_data();
	bool const user_data_result					= user_data->try_get( brain_cook_params );
	R_ASSERT_U									( user_data_result );

	R_ASSERT									( value.value_exists( "default_behaviour" ) );
	pcstr behaviour_name						= value["default_behaviour"];
	
	brain_unit* const brain						= XRAY_NEW_IMPL( g_allocator, brain_unit )(
													*brain_cook_params.npc,
													sound_player,
													navigation,
													*m_ai_world,
													value
												);
	behaviour_cook_params						behaviour_params;
	resources::user_data_variant				new_params;
	new_params.set								( behaviour_params );

	query_resource								(
		behaviour_name,
		resources::behaviour_class,
		boost::bind( &brain_unit_cook::on_default_behaviour_loaded, this, _1, brain ), 
		g_allocator,
		&new_params,
		parent
	);
}

void brain_unit_cook::on_default_behaviour_loaded	( resources::queries_result& data, brain_unit* const new_brain )
{
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT								( data.is_successful(), "couldn't create default behaviour" );
		parent->finish_query					( result_error );
		return;
	}

	behaviour_ptr default_behaviour_ptr			= static_cast_resource_ptr< behaviour_ptr >( data[0].get_unmanaged_resource() );
	new_brain->set_behaviour					( default_behaviour_ptr );
	
	parent->set_unmanaged_resource				(
				new_brain, 
				resources::memory_usage_type	( resources::nocache_memory, sizeof( brain_unit ) )
			);
	parent->finish_query						( result_success );
}

} // namespace ai
} // namespace xray