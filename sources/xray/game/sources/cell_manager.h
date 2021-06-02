////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CELL_MANAGER_H_INCLUDED
#define CELL_MANAGER_H_INCLUDED

#include "cell.h"
#include "game_project.h"

namespace stalker2 {

class game_world;
class terrain_manager;

typedef associative_vector<shared_string, game_object_ptr_, vector>			named_object_registry;

class cell_manager : public boost::noncopyable 
{

public:
	explicit						cell_manager		( game& game );
									~cell_manager		( ) { }
	void							load				( game_project_ptr const& game_project );
	void							unload				( );
	void							tick				( );
	void							set_inv_view_matrix	( float4x4 const& inv_view_matrix );
	bool							empty				( ) const;
	inline game_project_ptr const&	get_project			( ) const { return m_game_project; }
	game_object_ptr_				get_object_by_name	( pcstr object_id ) const;
	void							query_object_by_name( pcstr object_id, object_loaded_callback_type const& callback );
	void							register_object		( shared_string const& name, game_object_ptr_ const& o );
	named_object_registry			m_named_registry;

private:
	void							update_active_keys	( int max_depth );
	void							game_cell_loaded	( resources::queries_result& data );
	bool							has_cell			( cell_key const& k ) const;
	bool							load_cell			( cell_key const& k );
	bool							unload_cell			( cell_key const& k );
	void							debug_draw			( );
	void							debug_draw_key		( cell_key& k, math::color const& clr );

	void							load_scenes			( );
	void							on_scenes_loaded	(  resources::queries_result& data );

private:
	struct queried_object_data
	{
		shared_string						name;
		object_loaded_callback_type			callback;
	};

	typedef vector<cell_key>		keys_container;

	typedef vector<game_object_ptr_>	scenes_list;
	scenes_list							m_scenes;

	typedef vector<queried_object_data>	queried_objects_list;
	queried_objects_list				m_queried_objects;

private:
	keys_container					m_active_keys;
	keys_container					m_loaded_keys;

	float3							m_camera_position;
	float3							m_camera_direction;

	cell_key						m_current_key;

	game_project_ptr				m_game_project;
	game&							m_game;
}; // class cell_manager

} // namespace stalker2

#endif // #ifndef CELL_MANAGER_H_INCLUDED