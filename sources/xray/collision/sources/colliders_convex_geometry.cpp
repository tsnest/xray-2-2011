////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_convex_geometry.h"
#include "triangle_mesh_geometry.h"
#include "opcode_include.h"
#include <xray/math_convex.h>

namespace xray {
namespace collision {
namespace colliders {

convex_geometry::convex_geometry( triangle_mesh_geometry const& geometry, math::convex const& convex ):
	m_geometry				( geometry ),
	m_convex				( convex ),
	m_result				( 0 )
{
	query					( m_geometry.root( ) );
}

convex_geometry::intersection convex_geometry::intersects_aabb( Opcode::AABBNoLeafNode const* node ) const
{
	Point const&			node_center = node->mAABB.mCenter;
	float3 const center		= float3( node_center.x, node_center.y, node_center.z );

	Point const&			node_extents = node->mAABB.mExtents;
	float3 const extents	= float3( node_extents.x, node_extents.y, node_extents.z );

	math::aabb const aabb	= create_aabb_center_radius( center , extents );
	return					( m_convex.test_inexact( aabb ) );
}

void convex_geometry::calculate_triangles	( Opcode::AABBNoLeafNode const* node ) 
{
	if ( !node )
		return;

	if ( node->HasPosLeaf( ) )
		++m_result;
	else
		calculate_triangles	( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		++m_result;
	else
		calculate_triangles	( node->GetNeg( ) );
}

void convex_geometry::query			( Opcode::AABBNoLeafNode const* node ) 
{
	switch ( intersects_aabb( node ) ) {
		case math::intersection_outside :
			return;
		case math::intersection_intersect :
			break;
		case math::intersection_inside : {
			calculate_triangles	( node );
			return;
		}
		default : NODEFAULT();
	}

	if ( node->HasPosLeaf( ) )
		++m_result;
	else
		query				( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		++m_result;
	else
		query				( node->GetNeg( ) );
}

} // namespace colliders 
} // namespace collision
} // namespace xray