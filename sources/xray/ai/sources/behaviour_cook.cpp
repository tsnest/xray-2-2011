////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour_cook.h"
#include "behaviour.h"
#include "brain_unit.h"
#include <xray/ai/action_types.h>
#include <xray/ai/goal_types.h>
#include <xray/ai/world.h>

namespace xray {
namespace ai {

behaviour_cook::behaviour_cook	( ai_world* world ) :
	translate_query_cook		( resources::behaviour_class, reuse_true, use_current_thread_id ),
	m_ai_world					( world )
{
}

void behaviour_cook::translate_request_path	( pcstr request, fs_new::virtual_path_string & new_request ) const
{
	new_request.assignf			( "resources/brain_units/behaviours/%s.behaviour", request );
}

void behaviour_cook::translate_query		( resources::query_result_for_cook& parent )
{
	pcstr persistent_options_path			= "resources/brain_units/persistent/human.persistent_options";
	
	behaviour_cook_params					cook_params;
	parent.user_data()->try_get				( cook_params );

	u32 const requests_size					= cook_params.behaviour_config == 0 ? 2 : 1; 

	resources::request						requests[2];
	
	requests[0].id							= resources::binary_config_class;
	requests[0].path						= persistent_options_path;

	if ( cook_params.behaviour_config == 0 )
	{
		requests[1].id						= resources::binary_config_class;
		requests[1].path					= parent.get_requested_path();
	}

	query_resources							(
		requests,
		requests_size,
		boost::bind( &behaviour_cook::on_behaviour_options_received, this, _1 ),
		g_allocator,
		0,
		&parent
	);
}

void behaviour_cook::delete_resource		( resources::resource_base* resource )
{
	behaviour* behavior_res					= static_cast_checked< behaviour* >( resource );
	
	animation_item const* it_begin1			= behavior_res->get_animations();
	animation_item const* it_end1			= it_begin1 + behavior_res->get_animations_count();
	for ( animation_item const* it = it_begin1; it != it_end1; ++it )
		it->~animation_item					( );

	sound_item const* it_begin2				= behavior_res->get_sounds();
	sound_item const* it_end2				= it_begin2 + behavior_res->get_sounds_count();
	for ( sound_item const* it = it_begin2; it != it_end2; ++it )
		it->~sound_item						( );

	movement_target const* it_begin3		= behavior_res->get_movement_targets();
	movement_target const* it_end3			= it_begin3 + behavior_res->get_movement_targets_count();
	for ( movement_target const* it = it_begin3; it != it_end3; ++it )
		it->~movement_target				( );

	behavior_res->~behaviour				( );
	XRAY_FREE_IMPL							( g_allocator, resource );
}

static void increment_resources_count		( configs::binary_config_value const& filter_value, u32& result )
{
	configs::binary_config_value const& values				= filter_value["filenames"];
	result													+= values.size();
			
	if ( filter_value.value_exists( "filters" ) )
	{
		configs::binary_config_value const& subvalues		= filter_value["filters"];
			
		configs::binary_config_value::const_iterator it		= subvalues.begin();
		configs::binary_config_value::const_iterator it_end = subvalues.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			increment_resources_count						( value, result );
		}
	}
}

static void increment_resources_count_for_movement_targets	( configs::binary_config_value const& filter_value, u32& result )
{
	configs::binary_config_value const& values				= filter_value["positions"];
	result													+= values.size();
			
	if ( filter_value.value_exists( "filters" ) )
	{
		configs::binary_config_value const& subvalues		= filter_value["filters"];
			
		configs::binary_config_value::const_iterator it		= subvalues.begin();
		configs::binary_config_value::const_iterator it_end = subvalues.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			increment_resources_count_for_movement_targets	( value, result );
		}
	}
}

static void get_resources_count_in_filter_sets				(
		configs::binary_config_value const& filters,
		behaviour_resource_type_enum const resource_type,
		u32& result
	)
{
	configs::binary_config_value::const_iterator it_filters		= filters.begin();
	configs::binary_config_value::const_iterator it_filters_end	= filters.end();

	for ( ; it_filters != it_filters_end; ++it_filters )
	{
		configs::binary_config_value const& filters_value	= *it_filters;
		
		u32 const max_params_count							= 4;
		for ( u32 i = 0; i < max_params_count; ++i )
		{
			fixed_string< 20 >								param_filter;
			sprintf											( param_filter.c_str(), param_filter.fixed_size, "parameter%u_filter", i );
			if ( filters_value.value_exists( param_filter.c_str() ) )
			{
				configs::binary_config_value const& params	= filters_value[(pcstr)param_filter.c_str()];

				configs::binary_config_value::const_iterator it_param	  = params.begin();
				configs::binary_config_value::const_iterator it_param_end = params.end();

				for ( ; it_param != it_param_end; ++it_param )
				{
					configs::binary_config_value const& value	= *it_param;
					u32	const filter_id							= value["type"];
					filter_types_enum const filter_type			= (filter_types_enum)filter_id;
					
					if ( ( resource_type == resource_type_animation && filter_type == filter_type_animation ) ||
						 ( resource_type == resource_type_sound && filter_type == filter_type_sound ) )
						increment_resources_count				( value, result );
					else if ( ( resource_type == resource_type_animation && filter_type == filter_type_position ) ||
							  ( resource_type == resource_type_movement_target && filter_type == filter_type_position ) )
						increment_resources_count_for_movement_targets( value, result );
				}
			}
		}
	}
}

static u32 get_count_of_needed_resources	( configs::binary_config_value const& behaviour_value, behaviour_resource_type_enum const resource_type )
{
	u32 result													= 0;
	if ( behaviour_value.value_exists( "actions" ) )
	{
		configs::binary_config_value const& actions_value		= behaviour_value["actions"];

		configs::binary_config_value::const_iterator it			= actions_value.begin();
		configs::binary_config_value::const_iterator it_end		= actions_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& instance_value	= *it;
			u32	const action_type								= instance_value["id"];
			planning::action_types_enum const type				= (planning::action_types_enum)action_type;
			
			if ( ( resource_type == resource_type_animation && type == planning::action_type_play_animation ) ||
				 ( resource_type == resource_type_animation && type == planning::action_type_move_to_point ) ||
				 ( resource_type == resource_type_movement_target && type == planning::action_type_move_to_point ) ||
				 ( resource_type == resource_type_sound && type == planning::action_type_play_sound ) ||
				 ( type == planning::action_type_play_animation_with_sound ) )
			{
				if ( !instance_value.value_exists( "filter_sets" ) )
					continue;
				
				get_resources_count_in_filter_sets				( instance_value["filter_sets"], resource_type, result );
			}
		}
	}
	if ( behaviour_value.value_exists( "goals" ) )
	{
		configs::binary_config_value const& goals_value			= behaviour_value["goals"];

		configs::binary_config_value::const_iterator it			= goals_value.begin();
		configs::binary_config_value::const_iterator it_end		= goals_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& goal_value		= *it;
			u32	const goal_type									= goal_value["type"];
			planning::goal_types_enum const type				= (planning::goal_types_enum)goal_type;
			
			if ( ( resource_type == resource_type_animation && type == planning::goal_type_play_animation ) ||
				 ( resource_type == resource_type_animation && type == planning::goal_type_move_to_point ) ||
				 ( resource_type == resource_type_movement_target && type == planning::goal_type_move_to_point ) ||
				 ( resource_type == resource_type_sound && type == planning::goal_type_play_sound ) ||
				 ( type == planning::goal_type_play_animation_with_sound ) )
			{
				if ( !goal_value.value_exists( "filter_sets" ) )
					continue;
				
				get_resources_count_in_filter_sets				( goal_value["filter_sets"], resource_type, result );
			}
		}
	}
	return														result;
}

void behaviour_cook::load_sounds			(
		resources::query_result_for_cook* const parent,
		configs::binary_config_value const& behaviour_value,
		behaviour* const new_behaviour
	)
{
 	u32 const sounds_count					= new_behaviour->get_sounds_count();
	if ( sounds_count > 0 )
	{
		requests_type requests				( ALLOCA( sizeof( resources::request ) * sounds_count ), sounds_count );
		get_requests_for_subresources		( behaviour_value, requests, resource_type_sound );
		if ( !requests.empty() )
		{
			query_resources					(
				&requests.front(), 
				requests.size(), 
				boost::bind( &behaviour_cook::on_sounds_loaded, this, _1, boost::cref( behaviour_value ), new_behaviour ), 
				g_allocator,
				0,
				parent
			);
			return;
		}
	}
	load_movement_targets					( parent, behaviour_value, new_behaviour );
}

static void fill_movement_targets_data		(
		configs::binary_config_value const& filter_value,
		behaviour* const new_behaviour,
		u32& current_target_number
	)
{
	configs::binary_config_value const& positions_values			= filter_value["positions"];
			
	configs::binary_config_value::const_iterator it_positions		= positions_values.begin();
	configs::binary_config_value::const_iterator it_positions_end	= positions_values.end();

	for ( ; it_positions != it_positions_end; ++it_positions )
	{
		configs::binary_config_value const& value	= *it_positions;
		
		float3 const& position						= value["target_point"];
		float3 const& direction						= value["direction"];
		float3 const& velocity						= value["velocity"];
		pcstr const animation_name					= value["animation"]["name"];
		animation_item const* const animation		= new_behaviour->find_animation_by_filename( animation_name );
		R_ASSERT									( animation, "animation %s wasn't found in this behaviour", animation_name );
		
		movement_target* it_target					= pointer_cast< movement_target* >( pointer_cast< pbyte >(
														pointer_cast< pbyte >( new_behaviour ) +
														sizeof( *new_behaviour ) +
														new_behaviour->get_animations_count() *
														sizeof( animation_item ) +
														new_behaviour->get_sounds_count() * 
														sizeof( sound_item ) +
														current_target_number
													) );
		new( it_target )movement_target				( position, direction, velocity, animation );
		++current_target_number;
	}

	if ( filter_value.value_exists( "filters" ) )
	{
		configs::binary_config_value const& subvalues		= filter_value["filters"];
			
		configs::binary_config_value::const_iterator it		= subvalues.begin();
		configs::binary_config_value::const_iterator it_end = subvalues.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			fill_movement_targets_data						( value, new_behaviour, current_target_number );
		}
	}
}

static void parse_movement_targets_filter_sets				(
		configs::binary_config_value const& filters,
		behaviour* const new_behaviour,
		u32& current_target_number
	)
{
	configs::binary_config_value::const_iterator it_filters		= filters.begin();
	configs::binary_config_value::const_iterator it_filters_end	= filters.end();

	for ( ; it_filters != it_filters_end; ++it_filters )
	{
		configs::binary_config_value const& filters_value	= *it_filters;
		
		u32 const max_params_count							= 4;
		for ( u32 i = 0; i < max_params_count; ++i )
		{
			fixed_string< 20 >								param_filter;
			sprintf											( param_filter.c_str(), param_filter.fixed_size, "parameter%u_filter", i );
			if ( filters_value.value_exists( param_filter.c_str() ) )
			{
				configs::binary_config_value const& params	= filters_value[(pcstr)param_filter.c_str()];
			
				configs::binary_config_value::const_iterator it_param	  = params.begin();
				configs::binary_config_value::const_iterator it_param_end = params.end();

				for ( ; it_param != it_param_end; ++it_param )
				{
					configs::binary_config_value const& value	= *it_param;
					u32	const filter_id							= value["type"];
					filter_types_enum const filter_type			= (filter_types_enum)filter_id;
					
					if ( filter_type == filter_type_position )
						fill_movement_targets_data				( value, new_behaviour, current_target_number );
				}
			}
		}
	}
}

void behaviour_cook::fill_movement_targets	( configs::binary_config_value const& behaviour_value, behaviour* const new_behaviour )
{
	u32 current_target_number									= 0;
	if ( behaviour_value.value_exists( "actions" ) )
	{
		configs::binary_config_value const& actions_value		= behaviour_value["actions"];

		configs::binary_config_value::const_iterator it			= actions_value.begin();
		configs::binary_config_value::const_iterator it_end		= actions_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& instance_value	= *it;
			u32	const action_type								= instance_value["id"];
			planning::action_types_enum const type				= (planning::action_types_enum)action_type;
			
			if ( type == planning::action_type_move_to_point && instance_value.value_exists( "filter_sets" ) )
				parse_movement_targets_filter_sets				( instance_value["filter_sets"], new_behaviour, current_target_number );
		}
	}
	if ( behaviour_value.value_exists( "goals" ) )
	{
		configs::binary_config_value const& goals_value			= behaviour_value["goals"];

		configs::binary_config_value::const_iterator it			= goals_value.begin();
		configs::binary_config_value::const_iterator it_end		= goals_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& goal_value		= *it;
			u32	const goal_type									= goal_value["type"];
			planning::goal_types_enum const type				= (planning::goal_types_enum)goal_type;
			
			if ( type == planning::goal_type_move_to_point && goal_value.value_exists( "filter_sets" ) )
				parse_movement_targets_filter_sets				( goal_value["filter_sets"], new_behaviour, current_target_number );
		}
	}
}

void behaviour_cook::load_movement_targets	(
		resources::query_result_for_cook* const parent,
		configs::binary_config_value const& behaviour_value,
		behaviour* const new_behaviour
	)
{
	if ( new_behaviour->get_movement_targets_count() > 0)
		fill_movement_targets					( behaviour_value, new_behaviour );
	
	finish_creation							( parent, new_behaviour );
}

void behaviour_cook::load_behaviour_data	( 
		resources::query_result_for_cook* const parent,
		configs::binary_config_value const& behaviour_value,
		behaviour* const new_behaviour
	)
{
 	u32 const animations_count				= new_behaviour->get_animations_count();
	if ( animations_count > 0 )
	{
		requests_type requests				( ALLOCA( sizeof( resources::request ) * animations_count ), animations_count );
		get_requests_for_subresources		( behaviour_value, requests, resource_type_animation );
		if ( !requests.empty() )
		{
			query_resources					(
				&requests.front(), 
				requests.size(), 
				boost::bind( &behaviour_cook::on_animations_loaded, this, _1, boost::cref( behaviour_value ), new_behaviour ), 
				g_allocator,
				0,
				parent
			);
			return;
		}
	}	
	load_sounds								( parent, behaviour_value, new_behaviour );
}

void behaviour_cook::on_behaviour_options_received	( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT							( data.is_successful(), "couldn't receive options for behaviour or general options" );
		parent->finish_query				( result_error );
		return;
	}

	behaviour_cook_params					cook_params;
	resources::user_data_variant* user_data	= parent->user_data();
	bool const user_data_result				= user_data->try_get( cook_params );
	R_ASSERT_U								( user_data_result );
	
	configs::binary_config_value const& persistent_value	= static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() )->get_root();
	if ( cook_params.behaviour_config == 0 )
		m_loaded_binary_config				= static_cast_resource_ptr< configs::binary_config_ptr >( data[1].get_unmanaged_resource() );

	configs::binary_config_value const& behaviour_value		= cook_params.behaviour_config == 0 ? m_loaded_binary_config->get_root() : *cook_params.behaviour_config;

	u32 const animations_count 				= get_count_of_needed_resources( behaviour_value, resource_type_animation );
	u32 const sounds_count					= get_count_of_needed_resources( behaviour_value, resource_type_sound );
	u32 const movement_targets_count		= get_count_of_needed_resources( behaviour_value, resource_type_movement_target );
	
	u32 const behaviour_buffer_size			= sizeof( behaviour ) +
											  animations_count * sizeof( animation_item ) +
											  sounds_count * sizeof( sound_item ) +
											  movement_targets_count * sizeof( movement_target );
	u8* behaviour_buffer					= static_cast< u8* >( XRAY_MALLOC_IMPL( g_allocator, behaviour_buffer_size, "new_behaviour" ) );

	behaviour* const new_behaviour			= new( behaviour_buffer )behaviour(
												persistent_value,
												behaviour_value,
												*m_ai_world,
												animations_count,
												sounds_count,
												movement_targets_count
											);
	load_behaviour_data						( parent, behaviour_value, new_behaviour );
}

static void retrieve_filename	( configs::binary_config_value const& filter_value, buffer_vector< resources::request >& requests )
{
	configs::binary_config_value const& values		= filter_value["filenames"];
			
	configs::binary_config_value::const_iterator it_names		= values.begin();
	configs::binary_config_value::const_iterator it_names_end	= values.end();

	for ( ; it_names != it_names_end; ++it_names )
	{
		configs::binary_config_value const& value	= *it_names;
		resources::request							request;
		request.path								= (pcstr)value["name"];
		u32	class_id								= value["type"];
		request.id									= (resources::class_id_enum)class_id;
		requests.push_back							( request );
	}

	if ( filter_value.value_exists( "filters" ) )
	{
		configs::binary_config_value const& subvalues		= filter_value["filters"];
			
		configs::binary_config_value::const_iterator it		= subvalues.begin();
		configs::binary_config_value::const_iterator it_end = subvalues.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			retrieve_filename								( value, requests );
		}
	}
}

static void retrieve_filename_from_position_data	(
		configs::binary_config_value const& filter_value,
		buffer_vector< resources::request >& requests
	)
{
	configs::binary_config_value const& positions_values			= filter_value["positions"];
			
	configs::binary_config_value::const_iterator it_positions		= positions_values.begin();
	configs::binary_config_value::const_iterator it_positions_end	= positions_values.end();

	for ( ; it_positions != it_positions_end; ++it_positions )
	{
		configs::binary_config_value const& value	= *it_positions;
		resources::request							request;
		request.path								= value["animation"]["name"];
		u32	class_id								= value["animation"]["type"];
		request.id									= (resources::class_id_enum)class_id;
		requests.push_back							( request );
	}

	if ( filter_value.value_exists( "filters" ) )
	{
		configs::binary_config_value const& subvalues		= filter_value["filters"];
			
		configs::binary_config_value::const_iterator it		= subvalues.begin();
		configs::binary_config_value::const_iterator it_end = subvalues.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			retrieve_filename_from_position_data			( value, requests );
		}
	}
}

static void parse_filter_sets		(
		configs::binary_config_value const& filters,
		behaviour_resource_type_enum const resource_type,
		buffer_vector< resources::request >& requests
	)
{
	configs::binary_config_value::const_iterator it_filters		= filters.begin();
	configs::binary_config_value::const_iterator it_filters_end	= filters.end();

	for ( ; it_filters != it_filters_end; ++it_filters )
	{
		configs::binary_config_value const& filters_value	= *it_filters;
		
		u32 const max_params_count							= 4;
		for ( u32 i = 0; i < max_params_count; ++i )
		{
			fixed_string< 20 >								param_filter;
			sprintf											( param_filter.c_str(), param_filter.fixed_size, "parameter%u_filter", i );
			if ( filters_value.value_exists( param_filter.c_str() ) )
			{
				configs::binary_config_value const& params	= filters_value[(pcstr)param_filter.c_str()];
			
				configs::binary_config_value::const_iterator it_param	  = params.begin();
				configs::binary_config_value::const_iterator it_param_end = params.end();

				for ( ; it_param != it_param_end; ++it_param )
				{
					configs::binary_config_value const& value	= *it_param;
					u32	const filter_id							= value["type"];
					filter_types_enum const filter_type			= (filter_types_enum)filter_id;
					
					if ( ( resource_type == resource_type_animation && filter_type == filter_type_animation ) ||
						 ( resource_type == resource_type_sound && filter_type == filter_type_sound ) )
						retrieve_filename					( value, requests );
					else if ( resource_type == resource_type_animation && filter_type == filter_type_position )
						retrieve_filename_from_position_data( value, requests );
				}
			}
		}
	}
}

void behaviour_cook::get_requests_for_subresources				(
		configs::binary_config_value const& behaviour_value,
		requests_type& requests,
		behaviour_resource_type_enum const resource_type
	)
{
	if ( behaviour_value.value_exists( "actions" ) )
	{
		configs::binary_config_value const& actions_value		= behaviour_value["actions"];

		configs::binary_config_value::const_iterator it			= actions_value.begin();
		configs::binary_config_value::const_iterator it_end		= actions_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& instance_value	= *it;
			u32	const action_type								= instance_value["id"];
			planning::action_types_enum const type				= (planning::action_types_enum)action_type;
			
			if ( ( resource_type == resource_type_animation && type == planning::action_type_play_animation ) ||
				 ( resource_type == resource_type_animation && type == planning::action_type_move_to_point ) ||
				 ( resource_type == resource_type_sound && type == planning::action_type_play_sound ) ||
				 ( type == planning::action_type_play_animation_with_sound ) )
			{
				if ( !instance_value.value_exists( "filter_sets" ) )
					continue;
				
				parse_filter_sets								( instance_value["filter_sets"], resource_type, requests );
			}
		}
	}
	if ( behaviour_value.value_exists( "goals" ) )
	{
		configs::binary_config_value const& goals_value			= behaviour_value["goals"];

		configs::binary_config_value::const_iterator it			= goals_value.begin();
		configs::binary_config_value::const_iterator it_end		= goals_value.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& goal_value		= *it;
			u32	const goal_type									= goal_value["type"];
			planning::goal_types_enum const type				= (planning::goal_types_enum)goal_type;
			
			if ( ( resource_type == resource_type_animation && type == planning::goal_type_play_animation ) ||
				 ( resource_type == resource_type_animation && type == planning::goal_type_move_to_point ) ||
				 ( resource_type == resource_type_sound && type == planning::goal_type_play_sound ) ||
				 ( type == planning::goal_type_play_animation_with_sound ) )
			{
				if ( !goal_value.value_exists( "filter_sets" ) )
					continue;
				
				parse_filter_sets								( goal_value["filter_sets"], resource_type, requests );
			}
		}
	}
}

void behaviour_cook::on_animations_loaded	(
		resources::queries_result& data,
		configs::binary_config_value const& behaviour_value,
		behaviour* const new_behaviour
	)
{
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT							( data.is_successful(), "couldn't load animations for behaviour" );
		parent->finish_query				( result_error );

	}
	else
	{
		animation_item* it_animations		= pointer_cast< animation_item* >( pointer_cast< pbyte >( new_behaviour ) + sizeof( *new_behaviour ) );
		R_ASSERT_CMP						( data.size(), ==, new_behaviour->get_animations_count() );
		for ( u32 i = 0; i < data.size(); ++i )
			new( it_animations++ )animation_item( data[i].get_unmanaged_resource(), data[i].get_requested_path() );
	}
	load_sounds								( parent, behaviour_value, new_behaviour );
}

void behaviour_cook::on_sounds_loaded		(
		resources::queries_result& data,
		configs::binary_config_value const& behaviour_value,
		behaviour* const new_behaviour
	)
{
	resources::query_result_for_cook* const	parent = data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT							( data.is_successful(), "couldn't load sounds for behaviour" );
		parent->finish_query				( result_error );
	}
	else
	{
		sound_item* it_sound				= pointer_cast< sound_item* >( pointer_cast< pbyte >( pointer_cast< pbyte >( new_behaviour ) + sizeof( *new_behaviour ) + new_behaviour->get_animations_count() * sizeof( animation_item ) ) );
		R_ASSERT_CMP						( data.size(), ==, new_behaviour->get_sounds_count() );
		for ( u32 i = 0; i < data.size(); ++i )
			new( it_sound++ )sound_item		( data[i].get_unmanaged_resource(), data[i].get_requested_path() );
	}
	load_movement_targets					( parent, behaviour_value, new_behaviour );
}

void behaviour_cook::finish_creation		( resources::query_result_for_cook* const parent, behaviour* const new_behaviour )
{
	parent->set_unmanaged_resource			(
				new_behaviour, 
				resources::memory_usage_type( resources::nocache_memory, sizeof( behaviour ) )
			);
	parent->finish_query					( result_success );

	m_loaded_binary_config					= 0;
}

} // namespace ai
} // namespace xray
