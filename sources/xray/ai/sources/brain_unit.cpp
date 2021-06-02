////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "brain_unit.h"
#include "sensed_object.h"
#include "ai_world.h"
#include <xray/ai/npc.h>
#include "specified_problem.h"
#include "goal_selector.h"

namespace xray {
namespace ai {

brain_unit::brain_unit		(
		npc& npc,
		sound_player_ptr sound_player,
		navigation_environment& navigation,
		ai_world& world,
		configs::binary_config_value const& brain_unit_options
	) :
	m_npc					( npc ),
	m_sound_player			( sound_player ),
	m_navigation			( navigation ),
	m_world					( world ),
	m_current_sound			( 0 ),
	m_last_tick_time_in_ms	( world.get_current_time_in_ms() ),
	m_next_for_tick			( 0 ),
	m_behaviour				( 0 ),
	m_goal_selector			( NEW( planning::goal_selector )() ),
	m_specified_problem		( 0 ),
	m_working_memory		( brain_unit_options.value_exists( "working_memory_size" ) ? (u32)brain_unit_options["working_memory_size"] : 0, world ),
	m_is_activity_suspended	( true )
{
	if ( brain_unit_options.value_exists( "sensors" ) )
		create_sensors		( brain_unit_options );

	if ( brain_unit_options.value_exists( "perceptors" ) )
		create_perceptors	( brain_unit_options );

	if ( brain_unit_options.value_exists( "target_selectors" ) )
		create_selectors	( brain_unit_options );
}

brain_unit::~brain_unit		( )
{
	//LOG_INFO				( "destroying brain_unit 0x%8x", this );
}

void brain_unit::set_behaviour	( behaviour_ptr const& new_behaviour )
{
	stop_activity				( );
	
	if ( m_specified_problem )
		DELETE					( m_specified_problem );
	
	m_behaviour					= new_behaviour;
	
	set_sensors_parameters		( );
	m_specified_problem			= NEW( planning::specified_problem )( m_behaviour->get_domain(), m_behaviour->get_problem(), this );

	m_is_activity_suspended		= false;
}

struct clear_targets_predicate : private boost::noncopyable
{
	inline void operator()		( selectors::target_selector_base* selector ) const
	{
		selector->clear_targets	( );
	}
};

void brain_unit::stop_activity	( )
{
	m_is_activity_suspended		= true;
	
	if ( m_current_sound )
		on_finish_sound_playing	( );

	m_npc.stop_animation_playing( );
	m_blackboard.clear			( );

	m_goal_selector->finalize	( );

	m_target_selectors.for_each	( clear_targets_predicate() );
}

void brain_unit::clear_resources	( )
{
	m_working_memory.clear_resources( );
	m_sound_player->clear_resources	( );

	DELETE							( m_goal_selector );
	DELETE							( m_specified_problem );

	while ( sensors::active_sensor_base* active_sensor = m_active_sensors.pop_front() )
		DELETE						( active_sensor );

	while ( sensors::passive_sensor_base* passive_sensor = m_passive_sensors.pop_front() )
		DELETE						( passive_sensor );

	while ( perceptors::perceptor_base* perceptor = m_perceptors.pop_front() )
		DELETE						( perceptor );

	while ( selectors::target_selector_base* selector = m_target_selectors.pop_front() )
		DELETE						( selector );
}

struct tick_predicate : private boost::noncopyable
{
	template < typename T >
	inline void operator()		( T* object ) const
	{
		object->tick			( );
	}
};

void brain_unit::tick			( )
{
	m_sound_player->tick		( );

	if ( m_is_activity_suspended )
		return;

	m_active_sensors.for_each	( tick_predicate() );

	fixed_string2048			output_text;
	if ( m_world.get_current_time_in_ms() - m_last_tick_time_in_ms >= 500 )
	{
		m_goal_selector->set_current_goal( m_world.get_search_service(), *m_specified_problem, m_behaviour, m_blackboard );
		m_last_tick_time_in_ms	= m_world.get_current_time_in_ms();
	}
}

void brain_unit::subscribe_on_sensors_events		( perceptors::sensors_subscriber& subscriber )
{
	m_perceptors_subscriptions_manager.subscribe	( subscriber );
}

void brain_unit::unsubscribe_from_sensors_events	( perceptors::sensors_subscriber& subscriber )
{
	m_perceptors_subscriptions_manager.unsubscribe	( subscriber );
}

void brain_unit::on_sensed_hit	( sensors::sensed_object const& hit_object )
{
	m_behaviour->get_ignorance_filter().on_hit_event( c_ptr( hit_object.object ) );
	m_perceptors_subscriptions_manager.on_event		( hit_object );

	m_sound_player->play							( sound_collection_type_npc_pain, true, m_npc.get_position( float3( 0, 0, 0 ) ) );
}

void brain_unit::on_sensed_sound( sensors::sensed_object const& sound_object ) const
{
	m_perceptors_subscriptions_manager.on_event		( sound_object );
}

void brain_unit::on_seen_object	( sensors::sensed_object const& visual_object ) const
{
	m_perceptors_subscriptions_manager.on_event		( visual_object );
}

void brain_unit::on_interacting_object	( sensors::sensed_object const& pickup_object ) const
{
	m_perceptors_subscriptions_manager.on_event		( pickup_object );
}

void brain_unit::set_filter		( ignorable_game_object const* begin, ignorable_game_object const* end )
{
	m_behaviour->get_ignorance_filter().ignore( begin, end );
}

struct find_selector_by_name_predicate : private boost::noncopyable
{
	inline find_selector_by_name_predicate	( pcstr name_to_find ) :
		selector_name						( name_to_find )
	{
	}

	bool operator()			( selectors::target_selector_base* selector ) const
	{
		return				strings::equal( selector->get_name(), selector_name );
	}
	
	pcstr					selector_name;
};

selectors::target_selector_base* brain_unit::get_selector_by_name	( pcstr selector_name )
{
	find_selector_by_name_predicate find_selector_predicate			( selector_name );
	return m_target_selectors.find_if								( find_selector_predicate );
}

bool brain_unit::is_feeling_safe	( ) const
{
	return !m_working_memory.has_facts_about_danger();
}

struct dump_state_predicate : private boost::noncopyable
{
	inline	dump_state_predicate	( npc_statistics& stats ) :
		stats_to_be_filled			( stats )
	{
	}
	
	template < typename T >
	inline void operator()			( T* object ) const
	{
		object->dump_state			( stats_to_be_filled );
	}

	npc_statistics&					stats_to_be_filled;
};

void brain_unit::retrieve_statistics( npc_statistics& stats ) const
{
// 	if ( is_activity_suspended )
// 		return;
	
	dump_state_predicate			dumper_predicate( stats );
	m_active_sensors.for_each		( dumper_predicate );
	m_passive_sensors.for_each		( dumper_predicate );
	m_target_selectors.for_each		( dumper_predicate );
	m_working_memory.dump_state		( stats );
	m_blackboard.dump_state			( stats );
}

void brain_unit::get_available_animations	( animation_items_type& destination ) const
{
	if ( m_behaviour )
		m_behaviour->get_available_animations( destination );
}

void brain_unit::get_available_sounds		( sound_items_type& destination ) const
{
	if ( m_behaviour )
		m_behaviour->get_available_sounds	( destination );
}

void brain_unit::get_available_movement_targets		( movement_targets_type& destination ) const
{
	if ( m_behaviour )
		m_behaviour->get_available_movement_targets	( destination );
}

void brain_unit::play_animation		( animation_item const* const animation_to_be_played ) const
{
	m_npc.play_animation			( animation_to_be_played );
}

bool brain_unit::was_animation_played		( animation_item const* const target ) const
{
	return m_blackboard.is_animation_played	( target );
}

bool brain_unit::was_sound_played			( sound_item const* const target ) const
{
	return m_blackboard.is_sound_played		( target );
}

bool brain_unit::is_playing_sound			( ) const
{
	return m_current_sound					!= 0;
}

bool brain_unit::was_played_animation_with_sound	(
		animation_item const* const animation,
		sound_item const* const sound
	) const
{
	return m_blackboard.is_animation_played( animation ) || m_blackboard.is_sound_played( sound );
}

void brain_unit::on_finish_sound_playing			( )
{
	if ( !m_current_sound )
		return;

	LOG_INFO										( "finish_sound_playing: %s", m_current_sound->name.c_str() );
	m_blackboard.add_played_sound					( m_current_sound );
	m_current_sound									= 0;
}

void brain_unit::on_finish_animation_playing		( animation_item const* const animation )
{
	m_blackboard.add_played_animation				( animation );
}

void brain_unit::play_sound			( sound_item const* const sound_to_be_played )
{
	m_current_sound					= sound_to_be_played;
	m_sound_player->play			( m_current_sound->sound, boost::bind( &brain_unit::on_finish_sound_playing, this ), m_npc.get_position( float3( 0, 0, 0 ) ) );
	LOG_INFO						( "%s: playing sound %s", m_npc.cast_game_object()->get_name(), sound_to_be_played->name.c_str() );
}

void brain_unit::play_animation_with_sound			(
		animation_item const* const animation_to_be_played,
		sound_item const* const sound_to_be_played
	)
{
	LOG_INFO						(
		"%s: playing animation %s with sound %s",
		m_npc.cast_game_object()->get_name(),
		animation_to_be_played->name.c_str(),
		sound_to_be_played->name.c_str()
	);
}

bool brain_unit::is_playing_animation	( ) const
{
	return m_npc.is_playing_animation	( );
}

bool brain_unit::is_at_node				( game_object const* const node ) const
{
	return m_npc.is_at_node				( node );
}

bool brain_unit::is_patrolling			( ) const
{
	return m_npc.is_patrolling			( );
}

bool brain_unit::is_target_in_melee_range	( npc const* const target ) const
{
	return m_npc.is_target_in_melee_range	( target );
}

bool brain_unit::is_invisible			( ) const
{
	return m_npc.is_invisible			( );
}

bool brain_unit::is_at_cover			( ) const
{
	return m_npc.is_at_cover			( );
}

void brain_unit::attack_from_cover		( npc const* const target, weapon const* const gun ) const
{
	m_npc.attack_from_cover				( target, gun );
}

void brain_unit::reload					( weapon const* const gun ) const
{
	m_npc.reload						( gun );
}

void brain_unit::survey_area			( ) const
{
	m_npc.survey_area					( );
}

void brain_unit::stop_patrolling		( ) const
{
	m_npc.stop_patrolling				( );
}

void brain_unit::attack_melee			( npc const* const target, weapon const* const gun ) const
{
	m_npc.attack_melee					( target, gun );
}

void brain_unit::prepare_to_attack		( npc const* const target, weapon const* const gun ) const
{
	m_npc.prepare_to_attack				( target, gun );
}

void brain_unit::attack					( npc const* const target, weapon const* const gun ) const
{
	m_npc.attack						( target, gun );
}

void brain_unit::stop_attack			( npc const* const target, weapon const* const gun ) const
{
	m_npc.stop_attack					( target, gun );
}

bool brain_unit::is_at_position			( movement_target const* const target ) const
{
	return math::length					( target->target_position - m_npc.get_position( float3( 0.f, 0.f, 0.f ) ) ) <= 0.3f;//math::epsilon_3;
}

void brain_unit::move_to_point			( movement_target const* const target )
{
	m_npc.move_to_position				( target );
}

bool brain_unit::is_moving				( ) const
{
	return m_npc.is_moving				( );
}

} // namespace ai
} // namespace xray