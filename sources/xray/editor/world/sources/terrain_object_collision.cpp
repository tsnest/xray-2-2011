////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "terrain_collision_geometry.h"
#include "editor_world.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#include <xray/physics/world.h>
#pragma managed( pop )

namespace xray {
namespace editor {

void terrain_node::destroy_collision()
{
	if( m_collision->initialized() )
		m_collision->destroy	( g_allocator );
}

void terrain_node::initialize_collision()
{
	destroy_collision				( );
	
	m_vertices.get_height_bounds( m_min_height, m_max_height );

	terrain_collision_geometry* geom			= NEW(terrain_collision_geometry)( this, *m_transform );

	m_collision->create_from_geometry			(
					true,
					this,
					geom,
					editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain
				);
	m_collision->insert					( m_transform );
//	update_collision_transform			( );

}

void terrain_node::initialize_ph_collision( )
{
	if(m_rigid_body)
		destroy_ph_collision();

	if(m_vertices.m_vertices.Count==0)
		return;

	physics::bt_rigid_body_construction_info	info;
	info.m_collisionShape			= (*m_model)->m_bt_collision_shape;
	info.m_mass						= 0.0f;
	info.m_render_model_offset		= float3( float(m_core->node_size_)/2.0f, m_min_height+(m_max_height-m_min_height)/2.0f, -(float(m_core->node_size_)/2.0f) );

	m_rigid_body					= physics::create_rigid_body( info );

	float4x4 t						= create_translation(get_position());
	m_rigid_body->set_transform		( t );
	
	get_editor_world().get_physics_world().add_rigid_body( m_rigid_body );
}

void terrain_node::destroy_ph_collision( )
{
	if(!m_rigid_body)
		return;

	get_editor_world().get_physics_world().remove_rigid_body( m_rigid_body );
	physics::destroy_rigid_body	( m_rigid_body );

	m_rigid_body				= NULL;
}

} // namespace editor
} // namespace xray
