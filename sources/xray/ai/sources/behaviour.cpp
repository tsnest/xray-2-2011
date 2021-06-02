////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour.h"
#include "pddl_domain.h"
#include "pddl_problem.h"

namespace xray {
namespace ai {

behaviour::behaviour			(
		configs::binary_config_value const& general_options,
		configs::binary_config_value const& behaviour_options,
		ai_world& world,
		u32 const animations_count,
		u32 const sounds_count,
		u32 const movement_targets_count
	) :
	m_domain					( 0 ),
	m_problem					( 0 ),
	m_animations_count			( animations_count ),
	m_sounds_count				( sounds_count ),
	m_movement_targets_count	( movement_targets_count )
{
	deserialize_parameters		( behaviour_options );
	create_domain				( general_options, world );
	create_problem				( behaviour_options, world );
	create_goals				( behaviour_options, world );

	if ( behaviour_options.value_exists( "pre_perceptors_filter" ) )
		fill_ignorance_filter	( behaviour_options["pre_perceptors_filter"], world );
}

behaviour::~behaviour		( )
{
	using namespace			planning;
	
	while ( base_filter* filter = m_ignorance_filter.pop_aux_filter() )
		delete_filter		( filter );
	
	while ( goal* first_goal = m_goals.pop_front() )
	{
 		while ( action_parameter* parameter = first_goal->pop_parameter() )
 			DELETE			( parameter );

		while ( parameters_filters_type* filters = first_goal->pop_filter_list() )
			delete_parameters_filters( filters );

		DELETE				( first_goal );
	}
	
	while ( action_instance* action = m_problem->pop_action_instance() )
	{
		while ( action_parameter* parameter = action->pop_parameter() )
 			DELETE			( parameter );

		while ( parameters_filters_type* filters = action->pop_filter_list() )
			delete_parameters_filters( filters );
 		
		DELETE				( action );
	}
	DELETE					( m_problem );
	
	while ( generalized_action* action = m_domain->pop_action() )
		DELETE				( action );

	DELETE					( m_domain );
}

void behaviour::deserialize_parameters	( configs::binary_config_value const& options )
{
	if ( options.value_exists( "vision_sensor_parameters" ) )
	{
		configs::binary_config_value const& vision_parameters		= options["vision_sensor_parameters"];
		m_vision_parameters.deserialize								( vision_parameters );
	}
	
	if ( options.value_exists( "interaction_sensor_parameters" ) )
	{
		configs::binary_config_value const& interaction_parameters	= options["interaction_sensor_parameters"];
		m_interaction_parameters.deserialize						( interaction_parameters );
	}

	if ( options.value_exists( "damage_sensor_parameters" ) )
	{
		configs::binary_config_value const& damage_parameters		= options["damage_sensor_parameters"];
		m_damage_parameters.deserialize								( damage_parameters );
	}

	if ( options.value_exists( "hearing_sensor_parameters" ) )
	{
		configs::binary_config_value const& hearing_parameters		= options["hearing_sensor_parameters"];
		m_hearing_parameters.deserialize							( hearing_parameters );
	}

	if ( options.value_exists( "smell_sensor_parameters" ) )
	{
		configs::binary_config_value const& smell_parameters		= options["smell_sensor_parameters"];
		m_smell_parameters.deserialize								( smell_parameters );
	}
}

void behaviour::fill_action_parameter	( configs::binary_config_value const& options, planning::action_parameter& parameter )
{
	bool const iterate_first_only	= options.value_exists( "iterate_first_only" ) ? options["iterate_first_only"] : true;
	parameter.iterate_first			( iterate_first_only );

	pcstr selector_name				= options.value_exists( "selector" ) ? (pcstr)options["selector"] : "";
	parameter.set_selector_name		( selector_name );
}

void behaviour::get_available_animations	( animation_items_type& destination ) const
{
	animation_item const* it_begin			= get_animations();
	animation_item const* it_end			= it_begin + m_animations_count;

	for ( animation_item const* it = it_begin; it != it_end; ++it )
		if ( destination.size() < destination.max_size() )
			destination.push_back			( it );
}

void behaviour::get_available_sounds		( sound_items_type& destination ) const
{
	sound_item const* it_begin				= get_sounds();
	sound_item const* it_end				= it_begin + m_sounds_count;

	for ( sound_item const* it = it_begin; it != it_end; ++it )
		if ( destination.size() < destination.max_size() )
			destination.push_back			( it );
}

void behaviour::get_available_movement_targets	( movement_targets_type& destination ) const
{
	movement_target const* it_begin			= get_movement_targets();
	movement_target const* it_end			= it_begin + m_movement_targets_count;

	for ( movement_target const* it = it_begin; it != it_end; ++it )
		if ( destination.size() < destination.max_size() )
			destination.push_back			( it );
}

animation_item const* behaviour::find_animation_by_filename	( pcstr animation_filename ) const
{
	animation_item const* it_begin			= get_animations();
	animation_item const* it_end			= it_begin + m_animations_count;

	for ( animation_item const* it = it_begin; it != it_end; ++it )
		if ( strings::equal( it->name.c_str(), animation_filename ) )
			return							it;

	return									0;
}

} // namespace ai
} // namespace xray