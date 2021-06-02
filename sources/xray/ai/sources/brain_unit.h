////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev && Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BRAIN_UNIT_H_INCLUDED
#define BRAIN_UNIT_H_INCLUDED

#include "active_sensor_base.h"
#include "passive_sensor_base.h"
#include "perceptor_base.h"
#include "working_memory.h"
#include "subscriptions_manager.h"
#include "sensors_subscriber.h"
#include "target_selector_base.h"
#include "blackboard.h"
#include <xray/ai/sound_player.h>
#include "behaviour.h"
#include <xray/ai/sound_item.h>

namespace xray {
namespace ai {

struct npc;
struct navigation_environment;
class ai_world;
struct sensed_object;
struct sensors_subscriber;
struct npc_statistics;
struct animation_item;

namespace planning {
	class goal_selector;
	class specified_problem;
} // namespace planning

class brain_unit :
	public resources::unmanaged_resource,
	private boost::noncopyable
{
public:
					brain_unit				(
						npc& npc,
						sound_player_ptr player,
						navigation_environment& navigation,
						ai_world& world,
						configs::binary_config_value const& brain_unit_options
					);
	virtual			~brain_unit				( );

			void	tick					( );
			void	set_behaviour			( behaviour_ptr const& new_behaviour );
			void	get_available_animations( animation_items_type& destination ) const;
			void	get_available_sounds	( sound_items_type& destination ) const;
			void	get_available_movement_targets	( movement_targets_type& destination ) const;

	inline	npc&		get_npc				( ) { return m_npc; }
	inline	npc const&	get_npc				( ) const { return m_npc; }
	inline	bool	is_object_ignored		( game_object const* const object ) const { return m_behaviour->get_ignorance_filter().is_object_ignored( object ); }

			void	subscribe_on_sensors_events		( perceptors::sensors_subscriber& subscriber );
			void	unsubscribe_from_sensors_events	( perceptors::sensors_subscriber& subscriber );

			void	clear_resources			( );
			void	stop_activity			( );

			void	on_sensed_hit			( sensors::sensed_object const& hit_object );
			void	on_sensed_sound			( sensors::sensed_object const& sound_object ) const;
			void	on_seen_object			( sensors::sensed_object const& visual_object ) const;
			void	on_interacting_object	( sensors::sensed_object const& pickup_object ) const;

			void	set_filter				( ignorable_game_object const* begin, ignorable_game_object const* end );
			void	retrieve_statistics		( npc_statistics& stats ) const;
			
			bool	is_patrolling			( ) const;
			bool	is_at_cover				( ) const;
			bool	is_invisible			( ) const;
			bool	is_target_in_melee_range( npc const* const target ) const;
			bool	is_at_node				( game_object const* const node ) const;
			bool	is_playing_animation	( ) const;
			bool	is_feeling_safe			( ) const;

			void	prepare_to_attack		( npc const* const target, weapon const* const gun ) const;	
			void	attack					( npc const* const target, weapon const* const gun ) const;
			void	attack_melee			( npc const* const target, weapon const* const gun ) const;
			void	attack_from_cover		( npc const* const target, weapon const* const gun ) const;
			void	stop_attack				( npc const* const target, weapon const* const gun ) const;
			void	survey_area				( ) const;
			void	stop_patrolling			( ) const;
			void	reload					( weapon const* const gun ) const;

			void	play_animation			( animation_item const* const animation_to_be_played ) const;
			void	play_sound				( sound_item const* const sound_to_be_played );
			void	move_to_point			( movement_target const* const target );
			bool	is_playing_sound		( ) const;
			bool	is_moving				( ) const;
			bool	is_at_position			( movement_target const* const target ) const;
			void	play_animation_with_sound		(
						animation_item const* const animation_to_be_played,
						sound_item const* const sound_to_be_played
					);
			bool	was_played_animation_with_sound	( animation_item const* const animation, sound_item const* const sound ) const;
			void	on_finish_animation_playing		( animation_item const* const animation );
			bool	was_animation_played	( animation_item const* const target ) const;
			bool	was_sound_played		( sound_item const* const target ) const;
	
	selectors::target_selector_base* get_selector_by_name	( pcstr selector_name );
	inline	bool	is_suspended			( ) const { return m_is_activity_suspended; }

public:
	brain_unit*								m_next_for_tick;

private:
	typedef subscriptions_manager< perceptors::sensors_subscriber,
								   sensors::sensed_object > perceptors_subscriptions_manager_type;

	typedef intrusive_list<	sensors::active_sensor_base,
							sensors::active_sensor_base*,
							&sensors::active_sensor_base::m_next >		active_sensors_list;
	typedef intrusive_list<	sensors::passive_sensor_base,
							sensors::passive_sensor_base*,
							&sensors::passive_sensor_base::m_next >		passive_sensors_list;
	typedef intrusive_list<	perceptors::perceptor_base,
							perceptors::perceptor_base*,
							&perceptors::perceptor_base::m_next >		perceptors_list;
	typedef intrusive_list<	selectors::target_selector_base,
							selectors::target_selector_base*,
							&selectors::target_selector_base::m_next >	selectors_list;

private:	
			void	create_sensors			( configs::binary_config_value const& options );
			void	create_perceptors		( configs::binary_config_value const& options );
			void	create_selectors		( configs::binary_config_value const& options );
			
			void	set_sensors_parameters	( );
			void	on_finish_sound_playing	( );

private:
	working_memory							m_working_memory;
	blackboard								m_blackboard;
	perceptors_subscriptions_manager_type	m_perceptors_subscriptions_manager;

	sound_player_ptr						m_sound_player;
	sound_item const*						m_current_sound;

	active_sensors_list						m_active_sensors;
	passive_sensors_list					m_passive_sensors;
	perceptors_list							m_perceptors;
	selectors_list							m_target_selectors;

	npc&									m_npc;
	navigation_environment&					m_navigation;
	ai_world&								m_world;

	behaviour_ptr							m_behaviour;
	planning::goal_selector*				m_goal_selector;
	planning::specified_problem*			m_specified_problem;
	
	// temporary solution
	bool									m_is_activity_suspended;

	// testing
	mutable	u32								m_last_tick_time_in_ms;
}; // class brain_unit

typedef resources::resource_ptr< brain_unit, resources::unmanaged_intrusive_base > brain_unit_ptr;

} // namespace ai
} // namespace xray

#endif // #ifndef BRAIN_UNIT_H_INCLUDED
