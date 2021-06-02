////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_WORLD_H_INCLUDED
#define GAME_WORLD_H_INCLUDED

#include "game_scene.h"
#include "game_project.h"
#include "camera_director.h"
#include "logic_fwd_defines.h"
#include <xray/render/facade/game_renderer.h>
#include <xray/physics/engine.h>

namespace xray{

namespace physics{struct world;}
namespace collision{struct space_partitioning_tree;};

};

namespace stalker2{

class game;
class cell_manager;
class bullet_manager;
class free_fly_handler;
class free_fly_camera;
class hud;
struct test_anim_object;

class game_world	:public game_scene,
					private physics::engine

{
	typedef			game_scene				super;
public:
					game_world				( game& game );
	virtual			~game_world				( );
	game_project_ptr const	get_project		( ) const;

public:
			void	start_game				( );
	virtual void	tick					( );
			u32		game_time_ms			( );
			float	game_time_sec			( );
			u32		last_frame_time_ms		( );
			float	last_frame_time_sec		( );
 
	virtual void	on_activate				( );
	virtual void	on_deactivate			( );

	void			load					( pcstr project_resource_name, pcstr project_resource_path=NULL );
	void			unload					( );
	bool			empty					( );
	game&			get_game				( ) const { return m_game; }
	bullet_manager&	get_bullet_manager		( ) const { return *m_bullet_manager; }
	xray::collision::space_partitioning_tree*	get_collision_tree	( ) const		{ return m_collision_tree; };

	pcstr			project_resource_path	( ){ return m_project_resource_path.c_str();}

	camera_director_ptr& get_camera_director	( )						{ return m_camera_director; }
	game_object_ptr_	 get_object_by_name	( pcstr object_id ) const;
	void			query_object_by_name	( pcstr object_id, object_loaded_callback_type const& callback ) const;
	void			on_scene_start			( object_scene_ptr scene );
	void			on_scene_stop			( object_scene_ptr scene );
	
	void			switch_to_free_fly_camera( );
	void			switch_to_hud_camera	( );
	bool			is_loading_or_unloading	( ){return false;}
	hud*			get_hud					( ) { return m_hud; }
	xray::physics::world*					get_physics_world		( ) { return m_physics_world;}

protected:
	void			on_project_loaded		( resources::queries_result& data );
	void			turn_rtp_debug			( pcstr params );
	void			time_update				( );
private:
	fs_new::virtual_path_string				m_project_resource_path;
	game_project_ptr						m_game_project;
	game&									m_game;
	camera_director_ptr						m_camera_director;
	hud*									m_hud;
	free_fly_camera*						m_free_fly_camera;

	unique_ptr< cell_manager >				m_cell_manager;
	unique_ptr< bullet_manager >			m_bullet_manager;
	collision::space_partitioning_tree*		m_collision_tree;

	timing::timer							m_timer;
	u32										m_game_time_ms;
	float									m_game_time_sec;
	u32										m_last_frame_time_ms;
	float									m_last_frame_time_sec;
	
	scenes_list								m_active_scenes;
	xray::physics::world*					m_physics_world;
	test_anim_object*						m_test_anim_object;
public:
	void			init_physics					( );
	void			test_physics1					( );
	void			test_physics2					( );
	void			test_physics3					( );

}; // class game_world

} // namespace stalker2

#endif // #ifndef GAME_WORLD_H_INCLUDED
