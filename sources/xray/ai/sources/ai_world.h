////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef AI_WORLD_H_INCLUDED
#define AI_WORLD_H_INCLUDED

#include <xray/ai/world.h>
#include <xray/ai/engine.h>
#include "npc_subscriptions_manager.h"
#include "subscriptions_manager.h"
#include "sound_subscriber.h"
#include "damage_subscriber.h"
#include "game_object_subscriber.h"
#include "brain_unit.h"
#include <xray/ai/weapon.h>
#include <xray/ai/search/search.h>

namespace xray {
namespace ai {

struct sensed_sound_object;
struct animation_item;

class ai_world :
	public ai::world,
	private boost::noncopyable
{
public:
						ai_world			( ai::engine& engine );
	virtual				~ai_world			( );
	
	virtual	void		tick				( );
	
	virtual	void		add_brain_unit		( brain_unit_res_ptr brain );
	virtual	void		remove_brain_unit	( brain_unit_res_ptr brain );
	
	virtual	void		on_sound_event		(
							npc& npc,
							sensed_sound_object const& perceived_sound
						) const;
	virtual	void		on_hit_event		(
							npc& npc,
							sensed_hit_object const& perceived_hit
						) const;
	virtual void		on_destruction_event( game_object const& object_to_be_destroyed ) const;

	virtual	void		set_behaviour		( resources::unmanaged_resource_ptr new_behaviour, brain_unit_res_ptr brain );

	virtual	void		set_ignore_filter	(
							brain_unit_res_ptr brain,
							ignorable_game_object const* begin,
							ignorable_game_object const* end
						);
	// it's just a shortcut for set_ignore_filter( brain_unit, 0, 0 );
	virtual	void		clear_ignore_filter	( brain_unit_res_ptr brain );

	virtual	void		clear_resources		( );
	virtual	void		fill_npc_stats		( npc_statistics& stats, brain_unit_res_ptr brain ) const;

	virtual	u32		get_characters_count	( ) const	{ return m_npc_characters.size(); }
	virtual	u32		get_groups_count		( ) const	{ return m_npc_groups.size(); }
	virtual	u32		get_classes_count		( ) const	{ return m_npc_classes.size(); }
	virtual	u32		get_outfits_count		( ) const	{ return m_npc_outfits.size(); }
	virtual	u32		get_weapons_count		( u32 const weapons_type ) const;
	virtual	pcstr	get_weapon_name_by_id	( u32 const weapon_type, u32 const id ) const;
	virtual	character_type get_character_attributes_by_index( u32 const index ) const;
	
	// to be removed soon
	virtual	void	fill_objects_dictionary	( configs::binary_config_value const& dictionary );
	virtual	bool	is_npc_safe				( brain_unit_res_ptr brain ) const;
	virtual	u32		get_current_time_in_ms	( ) const { return m_npc_lives_timer.get_elapsed_msec(); }
	virtual	void	on_animation_finish		( animation_item const* const target, brain_unit_res_ptr brain );
			
			void	get_available_weapons	( npc* owner, weapons_list& list_to_be_filled );
			
			void	get_colliding_objects	( math::aabb const& query_aabb, ai_objects_type& results );
			void	get_visible_objects		( math::cuboid const& cuboid, update_frustum_callback_type const& update_callback );
			bool	ray_query				(
						collision_object const* const object_to_tick,
						collision_object const* const object_to_ignore,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						float const transparency_threshold,
						float& visibility_value
					) const;
	inline	planning::search& get_search_service	( ) { return m_search_service; }

			void		draw_frustum		(
							float fov_in_radians,
							float far_plane_distance,
							float aspect_ratio,
							float3 const& position,
							float3 const& direction,
							math::color color
						) const;
			void		draw_ray			( float3 const& start_point, float3 const& end_point, bool sees_something ) const;

	// for testing purposes, to be refactored soon
			u32		get_group_id_by_name	( pcstr group_name )		const;
			u32		get_character_id_by_name( pcstr character_name )	const;
			u32		get_class_id_by_name	( pcstr class_name )		const;
			u32		get_outfit_id_by_name	( pcstr outfit_name )		const;
			
			u32	get_melee_weapon_id_by_name	( pcstr weapon_name ) 		const;
			u32	get_sniper_weapon_id_by_name( pcstr weapon_name ) 		const;
			u32	get_heavy_weapon_id_by_name	( pcstr weapon_name ) 		const;
			u32	get_light_weapon_id_by_name	( pcstr weapon_name ) 		const;
			u32	get_energy_weapon_id_by_name( pcstr weapon_name ) 		const;

			u32		get_node_id_by_name		( pcstr node_name )			const;

			bool	is_target_dead			( npc const* target )		const;
			bool	is_weapon_loaded		( weapon const* target )	const;

public:
	typedef npc_subscriptions_manager< sensors::sound_subscriber, sensed_sound_object > sounds_subscriptions_manager_type;
	inline	sounds_subscriptions_manager_type& sounds_manager( ) { return m_sounds_subscriptions_manager; }
	
	typedef npc_subscriptions_manager< sensors::damage_subscriber, sensed_hit_object > damage_subscriptions_manager_type;
	inline	damage_subscriptions_manager_type& damage_manager( ) { return m_damage_subscriptions_manager; }

	typedef subscriptions_manager< game_object_subscriber, game_object > destruction_subscriptions_manager_type;
	inline	destruction_subscriptions_manager_type& destruction_manager( ) { return m_destruction_subscriptions_manager; }

	typedef intrusive_list< brain_unit, brain_unit*, &brain_unit::m_next_for_tick > brain_units;
	
	typedef fixed_vector< pstr, 32 >		available_objects_types;
	typedef fixed_vector< character_type, 32 > available_characters_types;

private:
			void	register_cooks			( );
			void	clear_dictionary		( );

private:
	sounds_subscriptions_manager_type		m_sounds_subscriptions_manager;
	damage_subscriptions_manager_type		m_damage_subscriptions_manager;
	destruction_subscriptions_manager_type	m_destruction_subscriptions_manager;

	brain_units								m_brain_units;
	timing::timer							m_npc_lives_timer;

	planning::search						m_search_service;

	engine&									m_engine;

	// for testing purposes, to be refactored soon
	available_characters_types				m_npc_characters;
	available_objects_types					m_npc_groups;
	available_objects_types					m_npc_classes;
	available_objects_types					m_npc_outfits;
	available_objects_types					m_melee_weapons;
	available_objects_types					m_sniper_weapons;
	available_objects_types					m_heavy_weapons;
	available_objects_types					m_light_weapons;
	available_objects_types					m_energy_weapons;
}; // class ai_world

} // namespace ai
} // namespace xray

#endif // #ifndef AI_WORLD_H_INCLUDED
