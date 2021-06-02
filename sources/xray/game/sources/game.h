////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <xray/engine/engine_user.h>
#include <xray/input/engine.h>
#include <xray/ui/engine.h>
#include <xray/physics/engine.h>
#include <xray/animation/engine.h>
#include <xray/rtp/engine.h>
#include <xray/ai_navigation/engine.h>
#include <xray/ai/engine.h>
#include "camera_director.h"
#include "human_npc.h"
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace ui {	struct world;}
namespace rtp {	struct world;}
namespace physics {	struct world;}
namespace input {	struct world;}
namespace collision { struct space_partitioning_tree;}
namespace sound { class sound_debug_stats;}

namespace render {
namespace game {
	class renderer;
} // namespace game

typedef math::rectangle< math::float2 >	viewport_type;

} // namespace render

namespace ai {
namespace navigation {
	struct world;
} // namespace navigation

	struct world;
} // namespace ai

} // namespace xray

namespace stalker2 {

class game_scene;
class game_world;
class game_object;
class main_menu;
class game_console;
class stats;
class npc_stats;
class stats_graph;
class human_npc;
class building_object;
class composite_building;

enum human_npc_behaviour_type_enum
{
	human_npc_neutral,
	human_npc_aggressive
};

class game :
	public xray::engine_user::world,
	private input::engine,
	private ui::engine,
	private physics::engine,
	private rtp::engine,
	private animation::engine,
	private ai::navigation::engine,
	private ai::engine,
	private boost::noncopyable
{
public:
							game					(
														xray::engine_user::engine& engine,
														xray::render::world& render_world,
														xray::sound::world& sound,
														xray::network::world& network );

	virtual					~game					( );

	virtual	void			enable					( bool value );
	virtual	void			tick					( u32 current_frame_id );
	virtual	void			clear_resources			( );
	virtual ui::world&		ui_world				( )		{return *m_ui_world;}
	virtual input::world&	input_world				( )		{return *m_input_world;}

			u32				time_ms					( );

	virtual	void			load					( pcstr project_resource_name, pcstr project_resource_path = NULL );
	virtual	void			unload					( pcstr, bool );

	virtual	void			get_colliding_objects	( math::aabb const& query_aabb, xray::ai::ai_objects_type& results );
	virtual	void			get_visible_objects		( math::cuboid const& cuboid, xray::ai::update_frustum_callback_type const& update_callback );
	virtual	bool			ray_query				(
								ai::collision_object const* const object_to_pick,
								ai::collision_object const* const object_to_ignore,
								float3 const& origin,
								float3 const& direction,
								float const max_distance,
								float const transparency_threshold,
								float& visibility_value
							) const;
	virtual	void			draw_frustum			(
								float fov_in_radians,
								float far_plane_distance,
								float aspect_ratio,
								float3 const& position,
								float3 const& direction,
								math::color color
							) const;
	virtual	void			draw_ray				(
								float3 const& start_point,
								float3 const& end_point,
								bool sees_something
							) const;
	virtual	u32				get_node_by_name		( pcstr node_name ) const;
	virtual	void			get_available_weapons	( ai::npc* owner, ai::weapons_list& list_to_be_filled ) const;
	
	game_world&				get_game_world			( ) const	{ return *m_game_world; }
	xray::ai::world&		get_ai_world			( ) const	{ return *m_ai_world; }
	xray::sound::world&		get_sound_world			( ) const	{ return m_sound_world; }
	xray::collision::space_partitioning_tree&		get_spatial_tree		( ) const	{ return *m_spatial_tree; }

	sound::sound_scene_ptr const&	get_sound_scene			( ) { return m_sound_scene; }

	xray::render::scene_ptr const&	get_render_scene( )			{ return m_scene; }
	xray::render::scene_view_ptr const&	get_render_scene_view( ){ return m_scene_view; }

	inline	engine_user::engine&	engine			( ) const	{ return m_engine; }
	xray::render::world&			render_world	( )			{ return m_render_world; }
	xray::rtp::world&				rtp				( )			{ return *m_rtp_world; }
	xray::physics::world&			physics			( )			{ return *m_physics_world; }
	xray::animation::world&			animation_world	( )			{ return *m_animation_world; }
	xray::sound::world&				sound_world		( )			{ return m_sound_world; }
	xray::ai::navigation::world& ai_navigation_world( ) const	{ return *m_ai_navigation_world; }

	xray::render::game::renderer&	renderer		( )			{ return m_renderer; }
			void			apply_camera			( camera_director_ptr cd );
			void			toggle_console			( );

			void			scene_close_query		( );
			void			exit					( pcstr str );
			void			check_selected_npc		( );
			void			setup_movement_target	( );
	inline	float3&			movement_target			( ) { return m_movement_target; }
			void			toggle_npc_creation_mode( );
			void			query_npc				( human_npc_behaviour_type_enum const behaviour_type, float3 const& initial_position = float3( 0.f, 0.f, 0.f ) );
	inline	float3 const&	get_camera_position		( ) const { return m_inverted_view_matrix.c.xyz(); }
			void			rotate_selected_npc		( float const y_angle );
			void			delete_selected_npc		( );
			void		on_npc_attributes_received	( configs::binary_config_value const& attributes_config, human_npc_ptr owner );
			void			assign_behaviour		( );

private:
			void			test					( );
			void			switch_to_scene			( game_scene* scene );
			void			update_stats			( u32 const current_frame_id );
			void			register_console_commands( );

			void			register_cooks			( );
			
			void			load_cmd				( pcstr project_name );
			void			unload_cmd				( pcstr project_name );
private:
			void			initialize_input		( );
			void			initialize_ui			( );
			void			initialize_physics		( );
			void			initialize_rtp			( );
			void			initialize_animation	( );
			void			initialize_ai_navigation( );
			void			initialize_ai			( );
			void			initialize_modules		( );
			void			deinitialize_modules	( );

	virtual	void 		on_application_activate		( );
	virtual	void 		on_application_deactivate	( );

	virtual	void			set_navmesh_info		( pcstr text ) const;
	
			void			query_scene				( );
			void			on_scene_created		( resources::queries_result& data );

			void			query_sound_scene		( );
			void			on_sound_scene_created	( resources::queries_result& data );

	// for AI tests	
			void			query_npc_dictionary	( );
			void		on_npc_dictionary_created	( resources::queries_result& data );
			void		on_queried_npc_attributes_received	( resources::queries_result& data, human_npc_ptr owner );
			void			on_behaviour_created	( resources::queries_result& data );
			void			on_npc_created			( resources::queries_result& data, float3 const camera_position );
			void		fill_npc_attributes_randomly( human_npc_ptr owner, float3 const& initial_position );
			void		fill_npc_attributes_manually( human_npc_ptr owner );
			void			finish_npc_creation		( human_npc_ptr& new_npc, human_npc::npc_game_attributes& attributes );
			bool			is_npc_id_available		( u32 const npc_id ) const;
			void		get_frustum_objects_callback(
							xray::ai::update_frustum_callback_type const* update_callback,
							xray::collision::object const& frustum_object
						);
	
			void			run_ai_tests			( u32 const current_frame_id );
			void			update_npc_stats		( );
			human_npc* find_npc_in_camera_direction	( ) const;
	
	template < typename T >
	inline	void			destroy					( T*& object_to_be_destroyed )
	{
		m_ai_world->on_destruction_event			( *object_to_be_destroyed );
		DELETE										( object_to_be_destroyed );
	}

private:
	typedef intrusive_list< human_npc,
							human_npc_ptr,
							&human_npc::next_npc,
							threading::single_threading_policy,
							size_policy >	npcs_type;

private:
	timing::timer							m_timer;
	threading::mutex						m_application_activation;

	xray::render::scene_ptr					m_scene;
	xray::render::scene_view_ptr			m_scene_view;
	xray::render::render_output_window_ptr	m_render_output_window;
	xray::render::viewport_type				m_viewport;

	xray::sound::sound_scene_ptr			m_sound_scene;

	stats_graph*							m_fps_graph;
	xray::engine::console*					m_console;
	stats*									m_stats;
	npc_stats*								m_active_npc_stats;

	xray::collision::space_partitioning_tree* m_spatial_tree;

	xray::engine_user::engine&				m_engine;
	xray::render::world&					m_render_world;
	xray::sound::world&						m_sound_world;
	xray::network::world&					m_network_world;

	xray::input::world*						m_input_world;
	xray::ui::world*						m_ui_world;
	xray::physics::world*					m_physics_world;
	xray::animation::world*					m_animation_world;
	xray::rtp::world*						m_rtp_world;
	xray::ai::navigation::world*			m_ai_navigation_world;
	xray::ai::world*						m_ai_world;

	render::game::renderer&					m_renderer;

	game_world*								m_game_world;
	main_menu*								m_main_menu;
	game_scene*								m_active_scene;

#ifndef MASTER_GOLD
	xray::sound::sound_debug_stats*			m_sound_stats;
#endif //#ifndef MASTER_GOLD

	bool									m_is_active;

	// tests for AI
	bool									m_is_dictionary_created;
	bool									m_is_npc_auto_creation_enabled;
	human_npc_ptr							m_selected_npc;
	bool									m_active_npc_set;
	npcs_type								m_npcs;
	u32										m_npc_queries_count;
	
	float4x4								m_inverted_view_matrix;
	float4x4								m_projection_matrix;

	u32										m_last_frame_time_ms;
	float									m_last_frame_time;
	bool									m_enabled;
	bool									m_initialized;
	
	// for sound test
	bool									m_sound_test_allowed;

	float3									m_movement_target;
}; // class game

} // namespace stalker2

#endif // #ifndef GAME_H_INCLUDED
