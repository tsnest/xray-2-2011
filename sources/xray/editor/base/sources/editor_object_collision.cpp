////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_object_collision.h"

#pragma managed(push, off)
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/geometry.h>
#pragma managed(pop)

namespace xray {
namespace editor_base {

collision_object_internal::collision_object_internal(collision::geometry const* geometry,
													editor_object_collision^ receiver,
													collision::object_type t )
:super		( g_allocator, t, geometry ),
m_receiver	( receiver )
{ 	
}

//object_base^ editor_object_collision::get_owner_object( ) const 
//{ 
//	object_base^ root_object = m_owner->get_root_object();
//	
//	if(root_object!=m_owner) // ( in compound? )
//	{
//		object_composite^ oc = safe_cast<object_composite^>(root_object);
//		if(oc->is_edit_mode()) // in edit mode allow to single select and transform modifying
//			return m_owner;
//		else
//			return oc;
//	}else
//		return m_owner; 
//}

bool collision_object_internal::touch( ) const
{
	return m_receiver->on_touch_collision( /*this*/ );
	//object_base^ root_object = m_owner->get_root_object();
	//if(root_object!=m_owner) // ( i am a compound member )
	//{
	//	object_composite^ oc = safe_cast<object_composite^>(root_object);
	//	return	oc->touch_object(m_owner);
	//}
	//return false;
}

////////////////
editor_object_collision::editor_object_collision( collision::space_partitioning_tree* tree,
												 collision_owner^ owner )
:m_collision_obj	( NULL ),
m_owner				( owner ),
m_collision_tree	( tree ), 
m_b_active			( false )
{
}

collision::object* editor_object_collision::get_collision_object( )
{ 
	return m_collision_obj; 
}

void editor_object_collision::destroy( memory::base_allocator* allocator )
{
	if ( initialized() )
	{
		remove						( );
		m_collision_obj->destroy	( g_allocator );
		DELETE						( m_collision_obj );
		m_collision_obj				= NULL;

		if ( m_user_geometry )
			collision::delete_geometry( allocator, m_user_geometry );
	}
}

void editor_object_collision::remove( )
{
	if ( m_b_active )
		m_collision_tree->erase( m_collision_obj );

	m_b_active = false;
}

void editor_object_collision::insert( )
{
	math::aabb					collision_aabb;
	m_collision_obj->get_object_geometry()->get_aabb	( collision_aabb );
	float3 center				= collision_aabb.center	( );
	float3 extents				= collision_aabb.extents( );
	m_collision_tree->insert	( m_collision_obj, center, extents );
	m_b_active					= true;
}

void editor_object_collision::set_matrix( float4x4 const* m )
{
	ASSERT						(initialized());
	if(!m_b_active)	
		return;

	m_collision_obj->set_matrix	(*m);
	m_collision_tree->move( m_collision_obj, m_collision_obj->get_center(), m_collision_obj->get_extents() );
}

aabb editor_object_collision::get_aabb( )
{
	return m_collision_obj->get_aabb();
}

bool editor_object_collision::initialized( )
{
	return( m_collision_obj != NULL );
}

void editor_object_collision::create_from_geometry( bool user_geom, 
												   collision::geometry* geom, 
												   collision::object_type t )
{
	if ( user_geom )
		m_user_geometry	= geom;

	m_collision_obj		= NEW (collision_object_internal)( geom, this, t );
}

bool editor_object_collision::on_touch_collision( /*collision_object_dynamic* object*/ )
{
	return m_owner->on_touch	( );
}


} // namespace editor_base
} // namespace xray
