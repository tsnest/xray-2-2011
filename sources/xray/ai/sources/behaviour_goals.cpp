////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour.h"
#include "action_parameter.h"
#include "predicate_types.h"
#include "pddl_domain.h"

namespace xray {
namespace ai {

static planning::goal* create_goal_by_type	(
		planning::goal_types_enum goal_type,
		configs::binary_config_value const& goal_options,
		planning::pddl_domain const& domain,
		ai_world& world
	)
{
	using namespace				planning;
	
	u32 const priority			= goal_options["priority"];
	pcstr const goal_caption	= goals_captions[goal_type];
	goal* result				= NEW( goal )( goal_type, priority, goal_caption );

	if ( goal_type == goal_type_kill_enemy )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		configs::binary_config_value const& param1_value = goal_options["parameter1"];

		action_parameter* param0 = NEW( action_parameter )( parameter_type_npc );
		action_parameter* param1 = NEW( action_parameter )( parameter_type_weapon );

		behaviour::fill_action_parameter( param0_value, *param0 );
		behaviour::fill_action_parameter( param1_value, *param1 );

		result->add_parameter	( param0 );
		result->add_parameter	( param1 );
	}
	else if ( goal_type == goal_type_blind_fire )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		
		action_parameter* param0 = NEW( action_parameter )( parameter_type_weapon );

		behaviour::fill_action_parameter( param0_value, *param0 );

		result->add_parameter	( param0 );
	}
	else if ( goal_type == goal_type_play_animation )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		
		action_parameter* owner	 = NEW( action_parameter )( parameter_type_owner );
		action_parameter* param0 = NEW( action_parameter )( parameter_type_animation );

		behaviour::fill_action_parameter( param0_value, *param0 );

		result->add_parameter	( owner );
		result->add_parameter	( param0 );
	}
	else if ( goal_type == goal_type_play_sound )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		
		action_parameter* owner	 = NEW( action_parameter )( parameter_type_owner );
		action_parameter* param0 = NEW( action_parameter )( parameter_type_sound );

		behaviour::fill_action_parameter( param0_value, *param0 );

		result->add_parameter	( owner );
		result->add_parameter	( param0 );
	}
	else if ( goal_type == goal_type_play_animation_with_sound )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		configs::binary_config_value const& param1_value = goal_options["parameter1"];
		
		action_parameter* owner	 = NEW( action_parameter )( parameter_type_owner );
		action_parameter* param0 = NEW( action_parameter )( parameter_type_animation );
		action_parameter* param1 = NEW( action_parameter )( parameter_type_sound );

		behaviour::fill_action_parameter( param0_value, *param0 );
		behaviour::fill_action_parameter( param1_value, *param1 );

		result->add_parameter	( owner );
		result->add_parameter	( param0 );
		result->add_parameter	( param1 );
	}
	else if ( goal_type == goal_type_move_to_point )
	{
		configs::binary_config_value const& param0_value = goal_options["parameter0"];
		
		action_parameter* owner	 = NEW( action_parameter )( parameter_type_owner );
		action_parameter* param0 = NEW( action_parameter )( parameter_type_position );

		behaviour::fill_action_parameter( param0_value, *param0 );

		result->add_parameter	( owner );
		result->add_parameter	( param0 );
	}

	behaviour::fill_goal_filter_sets( goal_options, world, *result );
	
	R_ASSERT ( goal_options.value_exists( "target_world_state_properties" ), "target_world_state_properties section is missing" );

	configs::binary_config_value const& properties		= goal_options["target_world_state_properties"];
			
	configs::binary_config_value::const_iterator it		= properties.begin();
	configs::binary_config_value::const_iterator it_end = properties.end();
	
	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& property	= *it;
	
		u32	const property_id							= property["property_id"];
		predicate_types_enum const type					= (predicate_types_enum)property_id;
		bool const property_value						= property["value"];
		bool const shift_indices						= domain[type]->has_owner_parameter();

		pddl_world_state_property_impl::indices_type	parameters_indices;

		if ( property.value_exists( "parameters_indices" ) )
		{
			if ( shift_indices )
				parameters_indices.push_back			( 0 );
			
			configs::binary_config_value const& indices	= property["parameters_indices"];

			configs::binary_config_value::const_iterator it_indices		= indices.begin();
			configs::binary_config_value::const_iterator it_end_indices	= indices.end();
	
			for ( ; it_indices != it_end_indices; ++it_indices )
			{
				u32 const index							= *it_indices;
				parameters_indices.push_back			( index + shift_indices );
			}
		}
		
		result->add_target_property						( domain[type], property_value, parameters_indices );
	}

	return												result;
}

static bool sort_goals_by_priority	( planning::goal const* const first, planning::goal const* const second )
{
   return							first->get_priority() > second->get_priority();
}

void behaviour::sort_goals			( )
{
	using namespace					planning;
	
	typedef	buffer_vector< goal* >	sorted_goals_type;
	u32 const goals_count			= m_goals.size();
	sorted_goals_type sorted_goals	( ALLOCA( goals_count * sizeof( goal* ) ), goals_count );	

	for ( goal* it_goal = m_goals.front(); it_goal; it_goal = m_goals.get_next_of_object( it_goal ) )
		sorted_goals.push_back		( it_goal );
	
	std::sort						( sorted_goals.begin(), sorted_goals.end(), sort_goals_by_priority );
	m_goals.clear					( );

	for ( sorted_goals_type::iterator it = sorted_goals.begin(), it_end = sorted_goals.end(); it != it_end; ++it )
		m_goals.push_back			( *it );
}

void behaviour::create_goals	( configs::binary_config_value const& options, ai_world& world )
{
	using namespace											planning;

	R_ASSERT												( m_domain );
	R_ASSERT												( m_problem );
	
	configs::binary_config_value const& goals_value			= options["goals"];

	configs::binary_config_value::const_iterator it			= goals_value.begin();
	configs::binary_config_value::const_iterator it_end		= goals_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& goal_type_value	= *it;
		u32	const goal_type									= goal_type_value["type"];
		goal_types_enum const type							= (goal_types_enum)goal_type;
		if ( goal* new_goal									=
				create_goal_by_type							(
					type,
					goal_type_value,
					*m_domain,
					world
				)
			)
			m_goals.push_back								 ( new_goal );
		else
			LOG_ERROR( "Unknown goal type was declared - %d", goal_type );
	}

	sort_goals												( );
}

} // namespace ai
} // namespace xray