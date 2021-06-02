////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_CELL_MANAGER_H_INCLUDED
#define EDITOR_CELL_MANAGER_H_INCLUDED

#include "project_defines.h"

namespace xray {
namespace editor {

ref class object_base;
ref class terrain_node;
ref class project;

typedef System::Drawing::Point													cell_key;
typedef System::Collections::Generic::List<cell_key>							cell_keys_list;

public ref struct cell_objects
{
	object_base_list^	m_objects_list;
	terrain_node^		m_terrain_node;
};

typedef System::Collections::Generic::Dictionary<cell_key, cell_objects^>	cells_storage;

public ref class editor_cell_manager
{
public:
			editor_cell_manager		( project^ p );
			~editor_cell_manager	( );
	void	register_object			( object_base^ o );
	void	unregister_object		( object_base^ o );
	void	register_terrain_node	( terrain_node^ terrain );
	void	unregister_terrain_node	( terrain_node^ terrain );
	void	move_object				( object_base^ o, math::float3 const& prev_position );
	void	clear					( );
	void	render					( );
	void	on_camera_move			( );
	void	on_depth_changed		( )	{ current_key_changed(); }

	cells_storage^	get_cells_storage	( ) { return %m_storage; }
	cell_keys_list^	get_loaded_keys_list( ) { return %m_loaded_keys_list;}

private:
	void		current_key_changed	( );
	void		get_active_list		( cell_keys_list^ list, int depth );
	cell_key	get_object_key		( object_base^ o );

	project^						m_project;
	cells_storage					m_storage;
	cell_key						m_current_key;
	cell_keys_list					m_loaded_keys_list;
};//editor_cell_manager

} // namespace editor
} // namespace xray

#endif // #ifndef EDITOR_CELL_MANAGER_H_INCLUDED