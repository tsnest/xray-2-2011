////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_cell_manager.h"
#include "object_base.h"
#include "project.h"
#include "level_editor.h"
#include "editor_world.h"
#include "terrain_object.h"

namespace xray {
namespace editor {

#define cell_size (64.0f)

cell_key pick( float3 const& point )
{
	cell_key			result;
	result.X			= (int)((point.x) / cell_size) -1;
	result.Y			= (int)((point.z) / cell_size);
	if(point.z>0.0f)	++result.Y;
	if(point.x>0.0f)	++result.X;

	return result;
}

editor_cell_manager::editor_cell_manager( project^ p )
:m_project		( p ),
m_current_key	( 99999, 99999)
{
	m_project->get_level_editor()->view_window()->subscribe_on_camera_move( gcnew editor_base::camera_moved_delegate(this, &editor_cell_manager::on_camera_move));
	on_camera_move		( );
}

editor_cell_manager::~editor_cell_manager( )
{
}

void editor_cell_manager::render( )
{
	for each (cell_key key in m_loaded_keys_list)
	{
		if( m_storage.ContainsKey(key) )
		{
			cell_objects^ cell				= m_storage[key];
			object_base_list^ object_list	= cell->m_objects_list;

			for each (object_base^ o in object_list)
				o->render();
		}
	}
}

void editor_cell_manager::clear( )
{
	m_storage.Clear				( );
	m_loaded_keys_list.Clear	( );
	current_key_changed			( );
}

void editor_cell_manager::register_terrain_node( terrain_node^ terrain )
{
	cell_objects^	c = nullptr;

	cell_key k(terrain->m_tmp_key.x,terrain->m_tmp_key.z);

	if(!m_storage.ContainsKey(k))
	{
		c					= gcnew cell_objects;
		c->m_objects_list	= gcnew object_base_list;
		m_storage.Add	( k, c );
	}else
		c = m_storage[k];

	c->m_terrain_node	= terrain;

	if(m_loaded_keys_list.Contains(k))
		terrain->load_contents	( );
}

void editor_cell_manager::unregister_terrain_node( terrain_node^ terrain )
{
	cell_objects^	c = nullptr;

	cell_key k(terrain->m_tmp_key.x,terrain->m_tmp_key.z);

	R_ASSERT	(m_storage.ContainsKey(k));
	c			= m_storage[k];

	R_ASSERT	( c->m_terrain_node	== terrain);
	c->m_terrain_node	= nullptr;
	if(m_loaded_keys_list.Contains(k))
		terrain->unload_contents	( true );
}

void editor_cell_manager::register_object( object_base^ o )
{
	cell_key k						= pick( o->get_position() );
    object_base_list^ objects_list	= nullptr;

	if(!m_storage.ContainsKey(k))
	{
		cell_objects^	c	= gcnew cell_objects;
		c->m_objects_list	= gcnew object_base_list;
		c->m_terrain_node	= nullptr;

		m_storage.Add	( k, c );
		objects_list	= c->m_objects_list;
	}else
	{
		objects_list	= m_storage[k]->m_objects_list;
	}

	// todo fix it
	if( !objects_list->Contains(o) )
		objects_list->Add	( o );

	if(m_loaded_keys_list.Contains(k))
		o->load_contents	( );
}

void editor_cell_manager::unregister_object( object_base^ o )
{
	cell_key k						= pick( o->get_position() );
	R_ASSERT						(m_storage.ContainsKey(k));

	object_base_list^ objects_list	= m_storage[k]->m_objects_list;

	if( objects_list->Contains(o) )
		objects_list->Remove	( o );

	if(m_loaded_keys_list.Contains(k))
		o->unload_contents	( true );

}

void editor_cell_manager::move_object( object_base^ o, math::float3 const& prev_position )
{
	cell_key prev_key			= pick( prev_position );
	cell_key current_key		= pick( o->get_position() );

	if(prev_key==current_key)
		return;

	if(m_storage.ContainsKey(prev_key))
	{
		object_base_list^ objects_list	= m_storage[prev_key]->m_objects_list;
		if(objects_list->Contains(o))
		{
			objects_list->Remove( o );
		
			register_object		( o );
		}
	}
}

void editor_cell_manager::on_camera_move( )
{
	math::float3 position, direction;
	m_project->get_level_editor()->view_window()->get_camera_props( position, direction );

   	cell_key k			= pick( position );
	
	if( k!= m_current_key)
	{
		m_current_key		= k;
		current_key_changed	( );
	}
}

void editor_cell_manager::current_key_changed( )
{
	u32 depth				= m_project->get_level_editor()->get_editor_world().get_cells_flood_depth();
	cell_keys_list			active_keys_list;

	get_active_list			( %active_keys_list, depth );
	
	for each( cell_key k in active_keys_list )
	{
		if( !m_loaded_keys_list.Contains(k) )
		{
			m_loaded_keys_list.Add			( k );

			if( m_storage.ContainsKey(k) )
			{
				cell_objects^ c		= m_storage[k];
				object_base_list^	objects_list = c->m_objects_list;

				for each( object_base^ o in objects_list )
					o->load_contents();

				if(c->m_terrain_node)
					c->m_terrain_node->load_contents();
			}
		}
	}

	cell_keys_list temp_list = %m_loaded_keys_list;

	for each( cell_key k in temp_list )
	{
		cell_key dist		= m_current_key - System::Drawing::Size(k);
		float d				= math::sqrt( float(dist.X*dist.X) + float(dist.Y*dist.Y) );
		if( d > depth+2 )
		{
			m_loaded_keys_list.Remove			( k );

			if( m_storage.ContainsKey(k) )
			{
				cell_objects^ c						= m_storage[k];
				object_base_list^	objects_list	= c->m_objects_list;

				for each( object_base^ o in objects_list )
					o->unload_contents( false );

				if(c->m_terrain_node)
					c->m_terrain_node->unload_contents( false );

			}
		}
	}
}

void editor_cell_manager::get_active_list( cell_keys_list^ list, int depth )
{
	for( int ix=-depth; ix<=depth; ++ix)
		for( int iz=-depth; iz<=depth; ++iz)
			list->Add(cell_key(m_current_key.X+ix, m_current_key.Y+iz));
}

cell_key editor_cell_manager::get_object_key( object_base^ o )
{
	return pick( o->get_position() );
}

} // namespace editor
} // namespace xray