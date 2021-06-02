////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour.h"
#include "pddl_domain.h"
#include "predicate_types.h"
#include "brain_unit.h"
#include "predicate.h"
#include "ai_world.h"
#include <xray/ai/npc.h>
#include <xray/ai/action_types.h>

namespace xray {
namespace ai {

static planning::generalized_action* create_action_prototype(
		planning::action_types_enum action_type,
		configs::binary_config_value const& action_options,
		planning::pddl_domain const& domain
	)
{
	using namespace					planning;
	
	u32 const action_cost			= action_options["cost"];
	pcstr const caption				= actions_captions[action_type];
	generalized_action* result		= NEW( generalized_action )( domain, action_type, caption, action_cost );
	
	if ( action_type == action_type_attack )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_npc );
		result->add_parameter_type	( parameter_type_weapon );

		result->set_preconditions	( predicate( predicate_type_weapon_loaded, _2 ) );
		result->set_effects			( predicate( predicate_type_target_dead, _1 ) );
	}
	else if ( action_type == action_type_attack_melee )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_npc );
		result->add_parameter_type	( parameter_type_weapon );

		result->set_preconditions	( predicate( predicate_type_target_in_melee_range, _0, _1 ) );
		result->set_effects			( predicate( predicate_type_target_dead, _1 ) );
	}
	else if ( action_type == action_type_survey_area )
	{
		result->add_parameter_type	( parameter_type_owner );
		
		result->set_preconditions	( !predicate( predicate_type_patrolling, _0 ) );
		result->set_effects			( predicate( predicate_type_patrolling, _0 ) );
	}
	else if ( action_type == action_type_reload_weapon )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_weapon );

		result->set_preconditions	( !predicate( predicate_type_weapon_loaded, _1 ) );
		result->set_effects			( predicate( predicate_type_weapon_loaded, _1 ) );
	}
	else if ( action_type == action_type_uncover )
	{
		result->add_parameter_type	( parameter_type_owner );
		
		result->set_preconditions	( predicate( predicate_type_at_cover, _0 ) );
		result->set_effects			( !predicate( predicate_type_at_cover, _0 ) );
	}
	else if ( action_type == action_type_cloak )
	{
		result->add_parameter_type	( parameter_type_owner );
		
		result->set_preconditions	( !predicate( predicate_type_invisible, _0 ) );
		result->set_effects			( predicate( predicate_type_invisible, _0 ) );
	}
	else if ( action_type == action_type_take_cover )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_node );

		result->set_preconditions	( !predicate( predicate_type_at_cover, _0 ) );
		result->set_effects			( predicate( predicate_type_at_cover, _0 ) );
	}
	else if ( action_type == action_type_attack_from_cover )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_npc );
		result->add_parameter_type	( parameter_type_weapon );

		result->set_preconditions	( predicate( predicate_type_at_cover, _0 ) &&
									  predicate( predicate_type_weapon_loaded, _2 )
									);
		result->set_effects			( predicate( predicate_type_target_dead, _1 ) );
	}
	else if ( action_type == action_type_play_animation )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_animation );
		
		result->set_preconditions	( !predicate( predicate_type_animation_played, _0, _1 ) &&
									  !predicate( predicate_type_animation_playing, _0 ) &&
									  !predicate( predicate_type_is_moving, _0 ) &&
									  !predicate( predicate_type_sound_playing, _0 ) );
		result->set_effects			( predicate( predicate_type_animation_played, _0, _1 ) );
	}
	else if ( action_type == action_type_play_sound )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_sound );
		
		result->set_preconditions	( !predicate( predicate_type_sound_played, _0, _1 ) &&
									  !predicate( predicate_type_sound_playing, _0 ) &&
									  !predicate( predicate_type_is_moving, _0 ) &&
									  !predicate( predicate_type_animation_playing, _0 ) );
		result->set_effects			( predicate( predicate_type_sound_played, _0, _1 ) );
	}
	else if ( action_type == action_type_play_animation_with_sound )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_animation );
		result->add_parameter_type	( parameter_type_sound );
		
		result->set_preconditions	( !predicate( predicate_type_animation_with_sound_played, _0, _1, _2 ) &&
									  !predicate( predicate_type_animation_playing, _0 ) &&
									  !predicate( predicate_type_is_moving, _0 ) &&
									  !predicate( predicate_type_sound_playing, _0 ) );
		result->set_effects			( predicate( predicate_type_animation_with_sound_played, _0, _1, _2 ) );
	}
	else if ( action_type == action_type_move_to_point )
	{
		result->add_parameter_type	( parameter_type_owner );
		result->add_parameter_type	( parameter_type_position );
		
		result->set_preconditions	( !predicate( predicate_type_is_moving, _0 ) &&
									  !predicate( predicate_type_is_at_position, _0, _1 ) );
		result->set_effects			( predicate( predicate_type_is_at_position, _0, _1 ) );
	}
	
	return							result; 
}

template < typename T >
inline boost::function< bool ( T ) > predicate			( boost::function< bool ( T ) > const& result )
{
	return result;
}

template < typename T1, typename T2 >
inline boost::function< bool ( T1, T2 ) > predicate		( boost::function< bool ( T1, T2 ) > const& result )
{
	return result;
}

template < typename T1, typename T2, typename T3 >
inline boost::function< bool ( T1, T2, T3 ) > predicate ( boost::function< bool ( T1, T2, T3 ) > const& result )
{
	return result;
}

template < typename T1, typename T2, typename T3, typename T4 >
inline boost::function< bool ( T1, T2, T3, T4 ) > predicate ( boost::function< bool ( T1, T2, T3, T4 ) > const& result )
{
	return result;
}

void behaviour::create_domain	( configs::binary_config_value const& options, ai_world& world )
{
	m_domain					= NEW( planning::pddl_domain );

	m_domain->register_parameter_type< brain_unit const* >( planning::parameter_type_owner );
	m_domain->register_parameter_type< npc const* >( planning::parameter_type_npc );
	m_domain->register_parameter_type< weapon const* >( planning::parameter_type_weapon );
	m_domain->register_parameter_type< game_object const* >( planning::parameter_type_node );
	m_domain->register_parameter_type< animation_item const* >( planning::parameter_type_animation );
	m_domain->register_parameter_type< sound_item const* >( planning::parameter_type_sound );
	m_domain->register_parameter_type< movement_target const* >( planning::parameter_type_position );

	m_domain->add_predicate		(
		predicate_type_patrolling,
		predicates_captions[predicate_type_patrolling],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_patrolling, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_at_cover,
		predicates_captions[predicate_type_at_cover],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_at_cover, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_no_danger,
		predicates_captions[predicate_type_no_danger],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_feeling_safe, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_invisible,
		predicates_captions[predicate_type_invisible],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_invisible, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_target_dead,
		predicates_captions[predicate_type_target_dead],
		predicate< npc const* >( boost::bind( &ai_world::is_target_dead, &world, _1 ) )
	);
 	m_domain->add_predicate		(
 		predicate_type_target_in_melee_range,
 		predicates_captions[predicate_type_target_in_melee_range],
 		predicate< brain_unit const*, npc const* >( boost::bind( &brain_unit::is_target_in_melee_range, _1, _2 ) )
 	);
	m_domain->add_predicate		(
		predicate_type_weapon_loaded,
		predicates_captions[predicate_type_weapon_loaded],
		predicate< weapon const* >( boost::bind( &ai_world::is_weapon_loaded, &world, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_at_node,
		predicates_captions[predicate_type_at_node],
		predicate< brain_unit const*, game_object const* >( boost::bind( &brain_unit::is_at_node, _1, _2 ) )
	);
	m_domain->add_predicate		(
		predicate_type_animation_played,
		predicates_captions[predicate_type_animation_played],
		predicate< brain_unit const*, animation_item const* >( boost::bind( &brain_unit::was_animation_played, _1, _2 ) )
	);
	m_domain->add_predicate		(
		predicate_type_animation_playing,
		predicates_captions[predicate_type_animation_playing],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_playing_animation, _1 ) )
	);
	m_domain->add_predicate		(
		predicate_type_sound_played,
		predicates_captions[predicate_type_sound_played],
		predicate< brain_unit const*, sound_item const* >( boost::bind( &brain_unit::was_sound_played, _1, _2 ) )
	);
	m_domain->add_predicate		(
		predicate_type_sound_playing,
		predicates_captions[predicate_type_sound_playing],
		predicate< brain_unit const* >( boost::bind( &brain_unit::is_playing_sound, _1 ) )
	);
  	m_domain->add_predicate		(
  		predicate_type_animation_with_sound_played,
  		predicates_captions[predicate_type_animation_with_sound_played],
  		predicate< brain_unit const*, animation_item const*, sound_item const* >( boost::bind( &brain_unit::was_played_animation_with_sound, _1, _2, _3 ) )
  	);
	m_domain->add_predicate		(
  		predicate_type_is_at_position,
  		predicates_captions[predicate_type_is_at_position],
  		predicate< brain_unit const*, movement_target const* >( boost::bind( &brain_unit::is_at_position, _1, _2 ) )
  	);
	m_domain->add_predicate		(
  		predicate_type_is_moving,
  		predicates_captions[predicate_type_is_moving],
  		predicate< brain_unit const* >( boost::bind( &brain_unit::is_moving, _1 ) )
  	);

	configs::binary_config_value const& actions_value			= options["action_types"];

	configs::binary_config_value::const_iterator it				= actions_value.begin();
	configs::binary_config_value::const_iterator it_end			= actions_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& action_type_value	= *it;
		u32	const action_type									= action_type_value["id"];
		planning::action_types_enum const type					= (planning::action_types_enum)action_type;
		if ( planning::generalized_action* action				=
				create_action_prototype							(
					type,
					action_type_value,
					*m_domain
				)
			)
			m_domain->add_action								( action );
		else
			LOG_ERROR( "Unknown action type was declared - %d", action_type );
	}
}

} // namespace ai
} // namespace xray