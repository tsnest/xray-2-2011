////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_cuboid_geometry.h"
#include "opcode_include.h"
#include "triangle_mesh_geometry.h"

using xray::collision::colliders::cuboid_geometry;
using xray::collision::triangle_mesh_geometry;
using xray::collision::object;
using xray::collision::triangles_type;
using xray::math::float3;
using xray::math::intersection;
using IceMaths::Point;

cuboid_geometry::cuboid_geometry				(
		triangle_mesh_geometry const& geometry,
		object const* object,
		xray::math::cuboid const& cuboid,
		triangles_type& triangles
	) :
	m_geometry				( geometry ),
	m_cuboid				( cuboid ),
	m_triangles				( triangles ),
	m_object				( object )
{
	u32 const size_before	= m_triangles.size( );
	query					( m_geometry.root( ) );
	m_result				= m_triangles.size( ) > size_before;
}

intersection cuboid_geometry::intersects_aabb	( Opcode::AABBNoLeafNode const* const node) const
{
	Point const&			node_center = node->mAABB.mCenter;
	float3 const center		= float3( node_center.x, node_center.y, node_center.z );

	Point const&			node_extents = node->mAABB.mExtents;
	float3 const extents	= float3( node_extents.x, node_extents.y, node_extents.z );

	math::aabb const aabb	= create_aabb_center_radius( center , extents );
	return					( m_cuboid.test_inexact( aabb ) );
}

void cuboid_geometry::add_triangle	( u32 const triangle_id ) const
{
	m_triangles.push_back	( triangle_result(m_object, triangle_id) );
}

void cuboid_geometry::add_triangles			( Opcode::AABBNoLeafNode const* const node ) const
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

void cuboid_geometry::query					( Opcode::AABBNoLeafNode const* const node ) const
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
		default : NODEFAULT();
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