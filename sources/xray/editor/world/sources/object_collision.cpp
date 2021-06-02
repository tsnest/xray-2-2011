////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision.h"
#include "collision_object_dynamic.h"

#pragma managed( push, off )
#include <xray/collision/geometry_instance.h>
#include <xray/collision/space_partitioning_tree.h>
#pragma managed( pop )

namespace xray {
namespace editor {


object_collision::object_collision( collision::space_partitioning_tree* tree )
:m_collision_obj	( NULL ),
m_collision_tree	( tree ), 
m_b_active			( false )
{
}

collision::object* object_collision::get_collision_object( )
{ 
	return m_collision_obj; 
}

void object_collision::destroy( memory::base_allocator* allocator )
{
	if ( initialized() )
	{
		remove						( );
		m_collision_obj->destroy	( allocator );
		DELETE						( m_collision_obj );
		m_collision_obj				= NULL;
	}
}
void object_collision::remove( )
{
	if ( m_b_active )
	{
		m_collision_tree->erase( m_collision_obj );
	}

	m_b_active = false;
}

void object_collision::insert( float4x4 const* m )
{
	m_collision_tree->insert	( m_collision_obj, *m );
	m_b_active					= true;
}

void object_collision::set_matrix( float4x4 const* m )
{
	ASSERT						(initialized());
	if(!m_b_active)	
		return;

	m_collision_tree->move( m_collision_obj, *m );
}

aabb object_collision::get_aabb( )
{
	return m_collision_obj->get_aabb();
}

bool object_collision::initialized( )
{
	return( m_collision_obj != NULL );
}

void object_collision::create_from_geometry( bool user_geom, object_base^ o, collision::geometry_instance* geom, collision::object_type t )
{
	if ( user_geom )
		m_user_geometry	= geom;

	m_collision_obj		= NEW (editor::collision_object_dynamic)( geom, o, t );
}

} // namespace editor
} // namespace xray