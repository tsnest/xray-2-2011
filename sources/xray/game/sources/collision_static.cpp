////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_static.h"
#include <xray/collision/api.h>
#include <xray/physics/world.h>
#include <xray/collision/object.h>
#include <xray/collision/geometry_instance.h>

namespace stalker2{

enum {
	object_type_geometry	= u32(1) << 0,
	object_type_object		= u32(1) << 1,
}; // enum

collision_static::collision_static():
	m_collision			( 0 ),
	m_collision_object	( 0 ),
	m_statics_shell		( 0 ),
	m_instance			( 0 )
{
}

void collision_static::create( xray::resources::managed_resource_ptr const mesh_resource, physics::world &w  )
{
	ASSERT							( !m_collision );
	ASSERT							( !m_collision_object );
	ASSERT							( !m_statics_shell );
	m_collision						= &*collision::new_triangle_mesh_geometry( g_allocator, mesh_resource );
	m_instance						= &*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), m_collision );
	m_collision_object				= &*collision::new_collision_object( g_allocator, object_type_geometry, m_instance );

	collision::geometry_instance* gi =  dynamic_cast<collision::geometry_instance*>( m_collision_object );
	m_statics_shell					= &(*w.new_static_shell( gi ));//static_cast_checked<collision::geometry_instance*,collision::object*>( s_collision_object )

	w.set_ready						( true );
}

void	collision_static::destroy( physics::world &w )
{
	if ( m_collision_object )
		collision::delete_object	( g_allocator, m_collision_object );

	if ( m_instance )
		collision::delete_geometry_instance	( g_allocator, m_instance );

	if ( m_statics_shell )
		w.delete_shell( m_statics_shell );

	if ( m_collision )
		collision::delete_geometry	( g_allocator, m_collision );

}

}
