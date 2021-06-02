////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_geometry_base.h"
#include "triangle_mesh_geometry.h"
#include "opcode_include.h"

#include <xray/collision/ray_collision_utils.h>

using xray::collision::colliders::ray_geometry_base;
using xray::collision::triangle_mesh_geometry;
using xray::collision::ray_triangles_type;
using xray::collision::triangles_predicate_type;
using xray::collision::object;
using xray::math::float3;
using IceMaths::Point;

ray_geometry_base::ray_geometry_base	(
		triangle_mesh_geometry const& geometry,
		float3 const& origin,
		float3 const& direction,
		float const max_distance,
		triangles_predicate_type const& predicate
	) :
	m_ray_aabb_collider		( origin, direction ),
	m_geometry				( geometry ),
	m_predicate				( predicate),
	m_max_distance			( max_distance ),
	m_result				( false )
{
}

bool ray_geometry_base::intersects_aabb	( Opcode::AABBNoLeafNode const* node, float& distance ) const
{
	Point const&			node_center = node->mAABB.mCenter;
	float3					center;
	center					= (float3&)node_center;

	Point const&			node_extents = node->mAABB.mExtents;
	float3					extents;
	extents					= (float3&)node_extents;

	XRAY_ALIGN(16) sse::aabb_a16	aabb;
	
	sse::construct_aabb_a16( aabb, center, extents );

	return					( m_ray_aabb_collider.intersects_aabb_sse( aabb, distance ) );
}

bool ray_geometry_base::intersects_aabb_vertical	( Opcode::AABBNoLeafNode const* node, float& distance ) const
{
	Point const&			node_center = node->mAABB.mCenter;
	Point const&			node_extents = node->mAABB.mExtents;
	
	float3	center	(node_center.x, node_center.y, node_center.z ), 
			extents	(node_extents.x, node_extents.y, node_extents.z );

	return m_ray_aabb_collider.intersects_aabb_vertical( center, extents, distance );

}

bool ray_geometry_base::test_triangle		(u32 const& triangle, float& range)
{
	u32 const* const indices		= m_geometry.indices(triangle);
	float3 const* const vertices	= m_geometry.vertices();
	float3 const& v0				= vertices[indices[0]];
	float3 const& v1				= vertices[indices[1]];
	float3 const& v2				= vertices[indices[2]];
	return							(
		collision::test_triangle(
			v0,
			v1,
			v2,
			m_ray_aabb_collider.origin(),
			m_ray_aabb_collider.direction(),
			m_max_distance,
			range
		)
	);
}

