////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_query_geometry.h"
#include "triangle_mesh_geometry.h"
#include "opcode_include.h"

using xray::collision::colliders::ray_query_geometry;
using xray::collision::triangle_mesh_geometry;
using xray::collision::ray_triangles_type;
using xray::collision::triangles_predicate_type;
using xray::collision::object;
using xray::math::float3;

ray_query_geometry::ray_query_geometry	(
		triangle_mesh_geometry const& geometry,
		object const* object,
		float3 const& origin,
		float3 const& direction,
		float const max_distance,
		ray_triangles_type& triangles,
		triangles_predicate_type const& predicate
	) :
	super					( geometry, origin, direction, max_distance, predicate ),
	m_object				( object ),
	m_triangles				( triangles )
{
	u32 const size_before	= m_triangles.size( );
	if ( math::abs( m_ray_aabb_collider.direction().y ) == 1.f )
		query				(
			m_geometry.root( ),
			geometry::vertical_predicate< true >( )
		);
	else
		query				(
			m_geometry.root( ),
			geometry::vertical_predicate< false >( )
		);
	m_result				= m_triangles.size( ) > size_before;
}

void ray_query_geometry::test_primitive	(u32 const& triangle)
{
	float					range;
	if ( !test_triangle( triangle, range ) )
		return;

	ray_triangle_result result	= ray_triangle_result(m_object, triangle, range);
	
	(m_predicate)			( result );

	m_triangles.push_back	( result );
}

template < typename T >
void ray_query_geometry::query				( Opcode::AABBNoLeafNode const* const node, T const& predicate )
{
	XRAY_ALIGN(16) float	range;
	if ( !predicate( *this, node, range ) )
		return;

	if ( range > m_max_distance )
		return;

	if ( node->HasPosLeaf( ) )
		test_primitive		( node->GetPosPrimitive( ) );
	else
		query				( node->GetPos( ), predicate );

	if ( node->HasNegLeaf( ) )
		test_primitive		( node->GetNegPrimitive( ) );
	else
		query				( node->GetNeg( ), predicate );
}