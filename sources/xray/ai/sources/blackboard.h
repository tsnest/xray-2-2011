////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BLACKBOARD_H_INCLUDED
#define BLACKBOARD_H_INCLUDED

#include <xray/ai/game_object.h>
#include <xray/ai/animation_item.h>
#include <xray/ai/sound_item.h>

namespace xray {
namespace ai {

struct npc_statistics;

namespace planning {
	class goal;
} // namespace planning

class blackboard : private boost::noncopyable
{
public:
							blackboard				( );
							~blackboard				( );

	inline	game_object_ptr	get_current_enemy		( ) const { return m_current_enemy; }
	inline	void			set_current_enemy		( game_object_ptr const& enemy ) { m_current_enemy = enemy; }
	
	inline	game_object_ptr	get_current_threat		( ) const { return m_current_threat; }
	inline	void			set_current_threat		( game_object_ptr const& threat ) { m_current_threat = threat; }

	inline	game_object_ptr	get_current_pickup_item	( ) const { return m_current_pickup_item; }
	inline	void			set_current_pickup_item	( game_object_ptr const& pickup_item ) { m_current_pickup_item = pickup_item; }

	inline	game_object_ptr	get_current_disturbance	( ) const { return m_current_disturbance; }
	inline	void			set_current_disturbance	( game_object_ptr const& disturbance ) { m_current_disturbance = disturbance; }

	inline	game_object_ptr	get_current_weapon		( ) const { return m_current_weapon; }
	inline	void			set_current_weapon		( game_object_ptr const& weapon ) { m_current_weapon = weapon; }

	inline	planning::goal const* get_current_goal	( ) const { return m_current_goal; }
	inline	void			set_current_goal		( planning::goal const* current_goal ) { m_current_goal = current_goal; }

			void			add_played_animation	( animation_item const* const collection );
			bool			is_animation_played		( animation_item const* const collection ) const;

			void			add_played_sound		( sound_item const* const item );
			bool			is_sound_played			( sound_item const* const item ) const;

			void			dump_state				( npc_statistics& stats ) const;
			void			clear					( );

private:
			void			delete_played_animations( );
			void			delete_played_sounds	( );

private:
	animations_type			m_played_animations;
	sounds_type				m_played_sounds;
	
	game_object_ptr			m_current_enemy;
	game_object_ptr			m_current_threat;
	game_object_ptr			m_current_pickup_item;
	game_object_ptr			m_current_disturbance;
	game_object_ptr			m_current_weapon;
	planning::goal const*	m_current_goal;
}; // class blackboard

} // namespace ai
} // namespace xray

#endif // #ifndef BLACKBOARD_H_INCLUDED