////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ai_collision_object.h"
#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry.h>

namespace stalker2 {

ai_collision_object::ai_collision_object	(
		non_null< collision::geometry >::ptr geometry,
		float4x4 const& transform,
		ai::game_object& ai_object,
		u32 const object_type,
		pvoid const user_data
	) :
	m_geometry_instance	( *collision::new_composite_geometry_instance( g_allocator, transform, geometry ) ),
	m_ai_object			( ai_object )
{
	m_user_data			= user_data;
	set_type			( object_type );
}

ai_collision_object::ai_collision_object	(
		non_null< collision::geometry_instance >::ptr instance,
		ai::game_object& ai_object,
		u32 const object_type,
		pvoid const user_data
	) :
	m_geometry_instance	( *(&*instance) ),
	m_ai_object			( ai_object )
{
	m_user_data			= user_data;
	set_type			( object_type );
}

void ai_collision_object::render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	m_geometry_instance.render	( scene, renderer );
}

bool ai_collision_object::aabb_query		( math::aabb const& aabb, collision::triangles_type& triangles ) const
{
	return m_geometry_instance.aabb_query	( this, aabb, triangles );
}

bool ai_collision_object::cuboid_query		( math::cuboid const& cuboid, collision::triangles_type& triangles ) const
{
	return m_geometry_instance.cuboid_query	( this, cuboid, triangles );
}

bool ai_collision_object::ray_query			(
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance,
		collision::ray_triangles_type& triangles,
		collision::triangles_predicate_type const& predicate
	) const
{
	return m_geometry_instance.ray_query	( this, origin, direction, max_distance, distance, triangles, predicate );
}

bool ai_collision_object::aabb_test			( math::aabb const& aabb ) const
{
	return m_geometry_instance.aabb_test	( aabb );
}

bool ai_collision_object::cuboid_test		( math::cuboid const& cuboid ) const
{
	return m_geometry_instance.cuboid_test	( cuboid );
}

bool ai_collision_object::ray_test			(
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance
	) const
{
	return m_geometry_instance.ray_test		( origin, direction, max_distance, distance );
}

void ai_collision_object::add_triangles		( collision::triangles_type& triangles ) const
{
	m_geometry_instance.add_triangles		( triangles );
}

non_null< ai_collision_object >::ptr ai_collision_object::new_ai_geometry_object	(
		memory::base_allocator* allocator,
		ai::game_object* ai_game_object,
		u32 const object_type,
		float4x4 const& matrix,
		non_null< collision::geometry >::ptr geometry,
		pvoid user_data
	)
{
	return XRAY_NEW_IMPL( allocator, ai_collision_object )(
		geometry,
		matrix,
		*ai_game_object,
		object_type,
		user_data
	);
}

non_null< ai_collision_object >::ptr ai_collision_object::new_ai_geometry_object	(
		memory::base_allocator* allocator,
		ai::game_object* ai_game_object,
		u32 const object_type,
		non_null< collision::geometry_instance >::ptr instance,
		pvoid user_data
	)
{
	return XRAY_NEW_IMPL( allocator, ai_collision_object )(
		instance,
		*ai_game_object,
		object_type,
		user_data
	);
}

void ai_collision_object::delete_ai_collision_object( memory::base_allocator* allocator, ai_collision_object* object )
{
	if ( !allocator || !object )
		return;

	collision::delete_geometry_instance( allocator, &(object->m_geometry_instance) );
	
	XRAY_DELETE_IMPL		( allocator, object );
}

math::aabb ai_collision_object::update_aabb	( float4x4 const& local_to_world )
{
	m_geometry_instance.set_matrix	( local_to_world );
	return			m_geometry_instance.get_aabb( );//.modify( local_to_world );
}

} // namespace stalker2
