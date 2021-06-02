////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_prismoid_geometry.h"
#include "triangle_mesh_base.h"
#include "opcode_include.h"

using xray::collision::colliders::prismoid_geometry;
using xray::collision::triangle_mesh_base;
using xray::collision::object;
using xray::collision::Results;
using xray::math::intersection;
using IceMaths::Point;

prismoid_geometry::prismoid_geometry					(
		triangle_mesh_base const& geometry,
		object const* const object,
		xray::math::prismoid const& prismoid,
		Results& results
	) :
	m_geometry				( geometry ),
	m_prismoid				( prismoid ),
	m_results				( results ),
	m_object				( object )
{
	u32 const size_before	= m_results.size( );
	query					( m_geometry.root( ) );
	m_result				= m_results.size( ) > size_before;
}

intersection prismoid_geometry::intersects_aabb			( Opcode::AABBNoLeafNode const* node ) const
{
	Point const&			node_center = node->mAABB.mCenter;
	float3 const center		= float3( node_center.x, node_center.y, node_center.z );

	Point const&			node_extents = node->mAABB.mExtents;
	float3 const extents	= float3( node_extents.x, node_extents.y, node_extents.z );

	math::aabb const aabb	= math::create_center_radius( center, extents );
	return					( m_prismoid.test_inexact( aabb ) );
}

void prismoid_geometry::add_triangle					( u32 triangle_id ) const
{
	collision::result const result = {
		m_object,
		triangle_id,
		math::SNaN
	};
	m_results.push_back		( result );
}

void prismoid_geometry::add_triangles					( Opcode::AABBNoLeafNode const* node ) const
{
	if ( !node )
		return;

	if ( node->HasPosLeaf( ) )
		add_triangle		( node->GetPosPrimitive( ) );
	else
		add_triangles		( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		add_triangle		( node->GetNegPrimitive( ) );
	else
		add_triangles		( node->GetNeg( ) );
}

void prismoid_geometry::query							( Opcode::AABBNoLeafNode const* node ) const
{
	switch ( intersects_aabb( node ) ) {
		case math::intersection_outside :
			return;
		case math::intersection_intersect :
			break;
		case math::intersection_inside : {
			add_triangles	( node );
			return;
		}
		default : NODEFAULT;
	}

	if ( node->HasPosLeaf( ) )
		add_triangle		( node->GetPosPrimitive( ) );
	else
		query				( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		add_triangle		( node->GetNegPrimitive( ) );
	else
		query				( node->GetNeg( ) );
}