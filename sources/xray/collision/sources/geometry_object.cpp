////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "geometry_object.h"
#include <xray/collision/geometry.h>

using xray::collision::detail::geometry_object;
using xray::collision::geometry;
using xray::collision::detail::world;
using xray::collision::query_type;
using xray::collision::object_type;
using xray::collision::ray_triangles_type;
using xray::collision::triangles_type;
using xray::collision::triangles_predicate_type;
using xray::math::float4x4;
using xray::math::float3;
using xray::math::aabb;
using xray::math::cuboid;
using xray::non_null;
using xray::memory::base_allocator;

geometry_object::geometry_object	( base_allocator* allocator, object_type const object_type, float4x4 const& matrix, non_null<geometry const>::ptr geometry ) :
	super					( allocator, matrix, geometry )
{
	set_type				( object_type );

	math::aabb temp			= get_geometry()->get_aabb( temp );
	m_aabb_center			= temp.center();
	m_aabb_extents			= temp.extents();
}

geometry_object::~geometry_object	( )
{
}

void geometry_object::render		( xray::render::debug::renderer& renderer ) const
{
	get_geometry()->render	( renderer, get_matrix() );
}

bool geometry_object::aabb_query	( aabb const& aabb, triangles_type& triangles ) const
{
	return					super::aabb_query( this, aabb, triangles );
}

bool geometry_object::cuboid_query	( cuboid const& cuboid, triangles_type& triangles ) const
{
	return					super::cuboid_query( this, cuboid, triangles );
}

bool geometry_object::ray_query		(
		float3 const& origin,
		float3 const& direction,
		float const max_distance,
		float& distance,
		ray_triangles_type& triangles,
		triangles_predicate_type const& predicate
	) const
{
	return					super::ray_query( this, origin, direction, max_distance, distance, triangles, predicate );
}

bool geometry_object::aabb_test		( aabb const& aabb ) const
{
	return					super::aabb_test( aabb );
}

bool geometry_object::cuboid_test	( cuboid const& cuboid ) const
{
	return					super::cuboid_test( cuboid );
}

bool geometry_object::ray_test		( float3 const& origin, float3 const& direction, float max_distance, float& distance ) const
{
	return					super::ray_test( origin, direction, max_distance, distance );
}

void geometry_object::add_triangles	( triangles_type& triangles ) const
{
	super::add_triangles	( triangles );
}