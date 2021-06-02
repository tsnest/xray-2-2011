////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour.h"
#include "pddl_problem.h"
#include <xray/ai/action_types.h>
#include <xray/ai/npc.h>
#include <xray/ai/weapon.h>
#include "brain_unit.h"

namespace xray {
namespace ai {

template < typename T >
inline boost::function< void ( T ) > action			( boost::function< void ( T ) > const& result )
{
	return							result;
}

template < typename T1, typename T2 >
inline boost::function< void ( T1, T2 ) > action	( boost::function< void ( T1, T2 ) > const& result )
{
	return							result;
}

template < typename T1, typename T2, typename T3 >
inline boost::function< void ( T1, T2, T3 ) > action( boost::function< void ( T1, T2, T3 ) > const& result )
{
	return							result;
}

template < typename T1, typename T2, typename T3, typename T4 >
inline boost::function< void ( T1, T2, T3, T4 ) > action ( boost::function< void ( T1, T2, T3, T4 ) > const& result )
{
	return							result;
}

static void fill_action_instances	(
		configs::binary_config_value const& action_options,
		ai_world& world,
		planning::pddl_domain const& domain,
		planning::pddl_problem& problem
	)
{
	using namespace											planning;

	configs::binary_config_value const& actions_value		= action_options["actions"];

	configs::binary_config_value::const_iterator it			= actions_value.begin();
	configs::binary_config_value::const_iterator it_end		= actions_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& instance_value	= *it;
		u32	const action_type								= instance_value["id"];
		action_types_enum const type						= (action_types_enum)action_type;
		generalized_action const* const prototype			= domain.get_action_by_type( type );
		R_ASSERT											( prototype, "generalized action wasn't found in domain" );

		action_instance* result								= NEW( action_instance )( prototype );

		if ( action_type == action_type_attack )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			configs::binary_config_value const& param1		= instance_value["parameter1"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_npc );
			action_parameter* parameter1					= NEW( action_parameter )( parameter_type_weapon );

			behaviour::fill_action_parameter				( param0, *parameter0 );
			behaviour::fill_action_parameter				( param1, *parameter1 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			result->add_parameter							( parameter1 );

			problem.add_action_instance						(
				*result,
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::prepare_to_attack, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::attack, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::stop_attack, ::_1, ::_2, ::_3 ) )
			);
		}
		else if ( action_type == action_type_attack_melee )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			configs::binary_config_value const& param1		= instance_value["parameter1"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_npc );
			action_parameter* parameter1					= NEW( action_parameter )( parameter_type_weapon );

			behaviour::fill_action_parameter				( param0, *parameter0 );
			behaviour::fill_action_parameter				( param1, *parameter1 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			result->add_parameter							( parameter1 );

			problem.add_action_instance						(
				*result,
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::prepare_to_attack, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::attack_melee, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::stop_attack, ::_1, ::_2, ::_3 ) )
			);
		}
		else if ( action_type == action_type_survey_area )
		{
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			
			result->add_parameter							( parameter_owner );
			
			boost::function< void ( brain_unit const* ) >	empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit const* >( boost::bind( &brain_unit::survey_area, ::_1 ) ),
				action< brain_unit const* >( boost::bind( &brain_unit::stop_patrolling, ::_1 ) )
			);
		}
		else if ( action_type == action_type_reload_weapon )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_weapon );
			
			behaviour::fill_action_parameter				( param0, *parameter0 );
			
			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );

			boost::function< void ( brain_unit const*, weapon const* ) > empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit const*, weapon const* >( boost::bind( &brain_unit::reload, ::_1, ::_2 ) ),
				empty_function
			);
		}
		else if ( action_type == action_type_uncover )
		{
		}
		else if ( action_type == action_type_cloak )
		{
		}
		else if ( action_type == action_type_take_cover )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_node );
			behaviour::fill_action_parameter				( param0, *parameter0 );
			
			result->add_parameter							( parameter0 );

			boost::function< void ( game_object const* ) >	empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				empty_function,
				empty_function
			);
		}
		else if ( action_type == action_type_attack_from_cover )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			configs::binary_config_value const& param1		= instance_value["parameter1"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_npc );
			action_parameter* parameter1					= NEW( action_parameter )( parameter_type_weapon );

			behaviour::fill_action_parameter				( param0, *parameter0 );
			behaviour::fill_action_parameter				( param1, *parameter1 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			result->add_parameter							( parameter1 );
			
			problem.add_action_instance						(
				*result,
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::prepare_to_attack, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::attack_from_cover, ::_1, ::_2, ::_3 ) ),
				action< brain_unit const*, npc const*, weapon const* >( boost::bind( &brain_unit::stop_attack, ::_1, ::_2, ::_3 ) )
			);
		}
		else if ( action_type == action_type_play_animation )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_animation );

			behaviour::fill_action_parameter				( param0, *parameter0 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			
			boost::function< void ( brain_unit const*, animation_item const* ) > empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit const*, animation_item const* >( boost::bind( &brain_unit::play_animation, ::_1, ::_2 ) ),
				empty_function
			);
		}
		else if ( action_type == action_type_play_sound )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_sound );

			behaviour::fill_action_parameter				( param0, *parameter0 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			
			boost::function< void ( brain_unit*, sound_item const* ) > empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit*, sound_item const* >( boost::bind( &brain_unit::play_sound, ::_1, ::_2 ) ),
				empty_function
			);
		}
		else if ( action_type == action_type_play_animation_with_sound )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			configs::binary_config_value const& param1		= instance_value["parameter1"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_animation );
			action_parameter* parameter1					= NEW( action_parameter )( parameter_type_sound );

			behaviour::fill_action_parameter				( param0, *parameter0 );
			behaviour::fill_action_parameter				( param1, *parameter1 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			result->add_parameter							( parameter1 );
			
			boost::function< void ( brain_unit*, animation_item const*, sound_item const* ) > empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit*, animation_item const*, sound_item const* >( boost::bind( &brain_unit::play_animation_with_sound, ::_1, ::_2, ::_3 ) ),
				empty_function
			);
		}
		else if ( action_type == action_type_move_to_point )
		{
			configs::binary_config_value const& param0		= instance_value["parameter0"];
			
			action_parameter* parameter_owner				= NEW( action_parameter )( parameter_type_owner );
			action_parameter* parameter0					= NEW( action_parameter )( parameter_type_position );

			behaviour::fill_action_parameter				( param0, *parameter0 );

			result->add_parameter							( parameter_owner );
			result->add_parameter							( parameter0 );
			
			boost::function< void ( brain_unit*, movement_target const* ) > empty_function;
			problem.add_action_instance						(
				*result,
				empty_function,
				action< brain_unit*, movement_target const* >( boost::bind( &brain_unit::move_to_point, ::_1, ::_2 ) ),
				empty_function
			);
		}
		else
			LOG_ERROR										( "Unknown action type was declared - %d", action_type );

		behaviour::fill_action_filter_sets					( instance_value, world, *result );
	}
}
	
void behaviour::create_problem	( configs::binary_config_value const& options, ai_world& world )
{
	R_ASSERT					( m_domain );
	m_problem					= NEW( planning::pddl_problem )( *m_domain );
	fill_action_instances		( options, world, *m_domain, *m_problem );	
}

} // namespace ai
} // namespace xray