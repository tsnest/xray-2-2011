////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HUMAN_NPC_H_INCLUDED
#define HUMAN_NPC_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"
#include <xray/ai/npc.h>
#include <xray/ai/game_object.h>
#include <xray/ai/filter_types.h>
#include <xray/ai/engine.h>
#include <xray/sound/sound_producer.h>
#include "animated_model_instance.h"
#include <xray/sound/sound_receiver.h>
#include "ai_collision_object.h"
#include "object_weapon.h"
#include "game_material_visibility_parameters.h"
#include <xray/render/engine/base_classes.h>
#include <xray/sound/sound_emitter.h>
#include <xray/animation/animation_expression_emitter.h>
#include <xray/ai_navigation/sources/navigation_mesh_types.h>
#include <xray/animation/animation_expression_emitter.h>

namespace xray {

namespace render {
namespace game {
	class renderer;
} // namespace game
} // namespace render

namespace ai {
	struct world;
	class brain_unit;
	struct npc_statistics;
	
	typedef resources::unmanaged_resource_ptr brain_unit_res_ptr;
} // namespace ai

namespace sound {
	struct world;
} // namespace sound

namespace physics {
	struct world;
} // namespace physics

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

} // namespace xray

namespace stalker2 {

class animation_space_graph;
class search_service;
struct animation_space_vertex_id;
class game;

// dummy, just for testing purposes (till the collision/damage detectors will be implemented)
struct hit_object
{
	ai::game_object*			m_source;
	float3						m_position;
	u16							m_target_bone;
	float						m_power;
}; // struct hit_object

struct npc_cook_params
{
	ai::brain_unit_cook_params	brain_unit_params;
	render::scene_ptr			scene;
	sound::sound_scene_ptr		sound_scene;
	physics::world*				physics_world;
}; // struct npc_cook_params

class human_npc;
typedef resources::resource_ptr< human_npc, resources::unmanaged_intrusive_base > human_npc_ptr;

class human_npc :
	public ai::npc,
	public ai::game_object,
	public sound::sound_producer,
	public sound::sound_receiver,
	public object_controlled,
	public game_object_	
{
public:
								human_npc			(
									ai::world& ai_world,
									sound::world& sound_world,
									sound::sound_scene_ptr const& sound_scene,
									collision::space_partitioning_tree& spatial_tree,
									render::scene_ptr const& scene,
									render::game::renderer& renderer,
									game_world& game_world
								);
	virtual						~human_npc			( );

public:
	human_npc_ptr				next_npc;

public:
	typedef intrusive_list< object_weapon, object_weapon*, &object_weapon::m_next > weapons_type;

public:
	struct npc_game_attributes
	{
		npc_game_attributes							( );

		npc_game_attributes&	operator =			( npc_game_attributes& other );
		
		weapons_type								weapons;
		float3										initial_position;
		float3										initial_scale;
		float3										initial_rotation;
		math::color									debug_draw_color;
		fixed_string< 32 >							name;
		fixed_string< 32 >							description;
		float										initial_velocity;
		float										initial_luminosity;
		u32											id;
		u32											group_id;
		u32											class_id;
		u32											outfit_id;
	};
	
public:	
	virtual math::aabb			get_aabb			( ) const;
	virtual math::float4x4		get_eyes_matrix		( ) const;
	virtual math::color			get_color			( ) const	{ return m_game_attributes.debug_draw_color; }
	virtual float3				get_eyes_direction	( ) const;
	virtual void				set_eyes_direction	( float3 const& direction ) { XRAY_UNREFERENCED_PARAMETER( direction ); }
	virtual	float3				get_position		( float3 const& requester )	const;
	virtual	float3				get_eyes_position	( ) const;
	virtual	void				clear_resources		( );
	virtual	bool				debug_draw_allowed	( ) const;
	
	virtual	pcstr				get_name			( ) const { return m_game_attributes.name.c_str(); }
	virtual	u32					get_id				( ) const { return m_game_attributes.id; }
	
	virtual	u32					get_group_id		( ) const { return m_game_attributes.group_id; }
	virtual	u32					get_class_id		( ) const { return m_game_attributes.class_id; }
	virtual	u32					get_outfit_id		( ) const { return m_game_attributes.outfit_id; }

	virtual	bool				is_patrolling		( ) const	{ return m_is_patrolling; }
	virtual	bool				is_at_cover			( ) const	{ return false; }
	virtual	bool				is_safe				( ) const;
	virtual	bool				is_invisible		( ) const	{ return false; }
	virtual	bool			is_target_in_melee_range( npc const* const target ) const;
	virtual	bool				is_at_node			( ai::game_object const* const node ) const;
	virtual	bool				is_playing_animation( ) const;
	virtual	bool				is_moving			( ) const;

	virtual	void				prepare_to_attack	( npc const* const target, ai::weapon const* const gun );
	virtual	void				attack				( npc const* const target, ai::weapon const* const gun );
	virtual	void				attack_melee		( npc const* const target, ai::weapon const* const gun );
	virtual	void				attack_from_cover	( npc const* const target, ai::weapon const* const gun );
	virtual	void				stop_attack			( npc const* const target, ai::weapon const* const gun );
	virtual	void				survey_area			( );
	virtual	void				stop_patrolling		( );
	virtual	void				reload				( ai::weapon const* const gun );
	virtual	void				play_animation		( ai::animation_item const* const target );
	virtual	void				move_to_position	( ai::movement_target const* const target );
	virtual	void			stop_animation_playing	( );
		
	virtual void				on_sound_event		( sound::sound_producer const& sound_source );
	virtual pcstr				get_description		( ) const { return m_game_attributes.description.c_str(); }
	virtual	float3				get_source_position	( float3 const& requester ) const { return get_position( requester ); }
	virtual void				on_hit_event		( hit_object const& hit_source );
	
	virtual	npc*				cast_npc			( )			{ return this; }
	virtual	npc const*			cast_npc			( )	const	{ return this; }
	virtual	ai::weapon*			cast_weapon			( )			{ return 0; }
	virtual	ai::weapon const*	cast_weapon			( )	const	{ return 0; }
	virtual float				get_velocity		( ) const	{ return m_game_attributes.initial_velocity; }
	virtual float				get_luminosity		( ) const	{ return m_game_attributes.initial_luminosity; }
	virtual	ai::collision_object* get_collision_object ( ) const;
	virtual	float3				get_random_surface_point( u32 const current_time ) const;
	virtual	float4x4			local_to_cell		( float3 const& requester ) const;
	virtual game_object const*	cast_game_object	( ) const	{ return this; }

			void				set_filter			( ai::ignorable_game_object const* begin, ai::ignorable_game_object const* end );
			void				clear_filter		( );
			void				fill_stats			( ai::npc_statistics& stats ) const;

			void				draw				( render::game::renderer& render, render::scene_ptr const& scene ) const;
			void				tick				( u32 const current_time_in_ms );

			void				add_weapon			( object_weapon* weapon );
			void				remove_weapon		( object_weapon* weapon );
	object_weapon*				pop_weapon			( );
			void			get_available_weapons	( ai::weapons_list& list_to_be_filled ) const;

			void				enable				( );
			void				set_attributes		( npc_game_attributes& attributes );
			void				set_rotation		( float4x4 const& new_rotation );
	inline	float3				get_rotation_angles	( ) const { return m_transform.get_angles_xyz(); }
			
			void				set_behaviour		( resources::unmanaged_resource_ptr new_behaviour );

public:
	// for testing purposes only
	inline	render::game::renderer&	get_dbg_render	( ) const { return m_renderer; }
	inline	render::scene_ptr const& get_dbg_scene	( ) const { return m_scene; }

	inline	void				set_sound_dbg_mode	( bool val ) { m_dbg_sound = val; }
	inline	bool				get_sound_dbg_mode	( ) const { return m_dbg_sound; }

private:			
	// specially for human_npc_cook
	friend class human_npc_cook;		
			void	set_brain_unit					( ai::brain_unit_res_ptr const& brain_unit );
			void	set_model						( animated_model_instance_ptr const& model );
			
			void	set_idle_animation				( animation::skeleton_animation_ptr const& idle_animation );
			void	set_walk_forward_animation		( animation::skeleton_animation_ptr const& fwd_walk_animation );
			void	set_arc_left_animation			( animation::skeleton_animation_ptr const& arc_left_animation );
			void	set_arc_right_animation			( animation::skeleton_animation_ptr const& arc_right_animation );
	
			void	setup_animations_controller		( );
			void	play_animation					( animation::animation_expression_emitter_ptr const& emitter );
			void	stop_animation_playing_impl		( );
			void	tick_animation_player			( u32 current_time_in_ms );
			void	render_model					( );
	inline float4x4 const&	get_transform			( ) { return m_transform; }
			void	set_transform					( float4x4 const& transform );
			float3	get_position					( )	const;
	animation::callback_return_type_enum on_animation_end	(
						animation::skeleton_animation_ptr const& ended_animation,
						pcstr const subscribed_channel,
						u32 const callback_time_in_ms,
						u32 const domain_data
					);
	animation::callback_return_type_enum on_animation_interval_end	(
						animation::skeleton_animation_ptr const& ended_animation,
						pcstr const subscribed_channel,
						u32 const callback_time_in_ms,
						u32 const domain_data
					);
			void	tick_animation_controller		( );
			void	on_animation_controller_animations_arrived( xray::resources::queries_result const& result );
			void	setup_animations				( u32 current_time_in_ms );

private:
	ai::world&										m_ai_world;
	sound::world&									m_sound_world;
	ai::brain_unit_res_ptr							m_brain_unit;

	render::game::renderer&							m_renderer;
	animated_model_instance_ptr						m_model_instance;

	collision::space_partitioning_tree&				m_spatial_tree;
	ai_collision_object*							m_collision_object;
	
	game_material_visibility_parameters				m_visibility_parameters;
	npc_game_attributes								m_game_attributes;

	float4x4										m_transform;

	render::scene_ptr								m_scene;
	sound::sound_scene_ptr							m_sound_scene;

	ai::animation_item const*						m_current_animation;
	bool											m_last_animation_emitted;
	bool											m_is_current_animation_finished;

	ai::movement_target const*						m_current_movement_target;

	npc const*										m_current_target;
	ai::weapon const*								m_current_weapon;
	bool											m_is_patrolling;

	// for debug
	mutable bool									m_sound_perceived;
	mutable bool									m_sound_produced;
	bool											m_dbg_sound;

////////////////////////////////////////////////////////////////////////////
// planning animation controller
////////////////////////////////////////////////////////////////////////////
	animation::skeleton_animation_ptr				m_idle_animation;
	animation::skeleton_animation_ptr				m_walk_forward_animation;
	animation::skeleton_animation_ptr				m_walk_forward_arc_left_animation;
	animation::skeleton_animation_ptr				m_walk_forward_arc_right_animation;
	animation_space_graph*							m_animation_space_graph;
	search_service*									m_search_service;
	animation_space_vertex_id*						m_target_vertex;
	xray::ai::navigation::path_type					m_navigation_path;
	u32												m_next_key_point;
}; // class human_npc

} // namespace stalker2

#endif // #ifndef HUMAN_NPC_H_INCLUDED