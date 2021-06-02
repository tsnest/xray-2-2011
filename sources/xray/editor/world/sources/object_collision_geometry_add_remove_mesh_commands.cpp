////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry_add_remove_mesh_commands.h"
#include "object_collision_geometry.h"
#include "project.h"
#include "project_items.h"
#include <xray/collision/primitives.h>

namespace xray{
namespace editor{

object_collision_geometry_add_mesh_command::object_collision_geometry_add_mesh_command( object_collision_geometry^ geometry, collision::primitive_type type, Boolean is_anti_mesh )
	:m_geometry_path	( geometry->owner_project_item->get_full_name( ) ),
	m_project			( geometry->owner_project_item->m_project ),
	m_type				( type ),
	m_is_anti_mesh		( is_anti_mesh )
{ }

bool object_collision_geometry_add_mesh_command::commit	( )
{
	object_collision_geometry^ geometry	= safe_cast<object_collision_geometry^>( m_project->find_by_path( m_geometry_path, true )->get_object( ) );

	if( m_is_anti_mesh )
		m_added_collision_index				= geometry->add_anti_mesh		( m_type );
	else
		m_added_collision_index				= geometry->add_collision_mesh	( m_type );

	return true;
}

void object_collision_geometry_add_mesh_command::rollback ( )
{
	object_collision_geometry^ geometry	= safe_cast<object_collision_geometry^>( m_project->find_by_path( m_geometry_path, true )->get_object( ) );

	if( m_is_anti_mesh )
		geometry->remove_anti_mesh		( m_added_collision_index );
	else
		geometry->remove_collision_mesh	( m_added_collision_index );
}

object_collision_geometry_remove_mesh_command::object_collision_geometry_remove_mesh_command ( object_collision_geometry^ geometry, collision::primitive_type type, Boolean is_anti_mesh, Int32 index )
	:m_geometry_path	( geometry->owner_project_item->get_full_name( ) ),
	m_project			( geometry->owner_project_item->m_project ),
	m_type				( type ),
	m_is_anti_mesh		( is_anti_mesh ),
	m_remove_collision_index ( index )
{ }

bool object_collision_geometry_remove_mesh_command::commit ( )
{
	object_collision_geometry^ geometry	= safe_cast<object_collision_geometry^>( m_project->find_by_path( m_geometry_path, true )->get_object( ) );

	if( m_is_anti_mesh )
		geometry->remove_anti_mesh		( m_remove_collision_index, m_position, m_rotation, m_scale );
	else
		geometry->remove_collision_mesh	( m_remove_collision_index, m_position, m_rotation, m_scale );
	return true;
}

void object_collision_geometry_remove_mesh_command::rollback ( )
{
	object_collision_geometry^ geometry	= safe_cast<object_collision_geometry^>( m_project->find_by_path( m_geometry_path, true )->get_object( ) );

	if( m_is_anti_mesh )
		geometry->add_anti_mesh			( m_type, m_position, m_rotation, m_scale, m_remove_collision_index );
	else
		geometry->add_collision_mesh	( m_type, m_position, m_rotation, m_scale, m_remove_collision_index );
}

} // namespace editor
} // namespace xray