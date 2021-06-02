////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Dmitriy Iassenev
//	Description	: a cuboid is a solid figure bounded by six faces, forming a convex polyhedron
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::math::cuboid;
using xray::math::plane;
using xray::math::float4x4;
using xray::math::aabb;
using xray::math::sphere;
using xray::math::intersection;

cuboid::cuboid					( plane const (&planes)[plane_count] )
{
	aabb_plane* i				= &m_planes[0];
	aabb_plane const* const e	= &m_planes[plane_count];
	plane const* j				= &planes[0];
	for ( ; i != e; ++i, ++j ) {
		(*i).plane				= *j;
		(*i).normalize			( );
	}
}

cuboid::cuboid					( aabb const& aabb, float4x4 const& matrix )
{
	float3 const vertices[] = {
		matrix.transform_position( aabb.vertex(0) ),
		matrix.transform_position( aabb.vertex(1) ),
		matrix.transform_position( aabb.vertex(2) ),
		matrix.transform_position( aabb.vertex(3) ),
		matrix.transform_position( aabb.vertex(4) ),
		matrix.transform_position( aabb.vertex(5) ),
		matrix.transform_position( aabb.vertex(6) ),
		matrix.transform_position( aabb.vertex(7) ),
	};

	m_planes[0].plane			= math::create_plane( vertices[0], vertices[1], vertices[2] );
	m_planes[1].plane			= math::create_plane( vertices[0], vertices[2], vertices[4] );
	m_planes[2].plane			= math::create_plane( vertices[0], vertices[4], vertices[1] );
	m_planes[3].plane			= math::create_plane( vertices[7], vertices[5], vertices[6] );
	m_planes[4].plane			= math::create_plane( vertices[7], vertices[3], vertices[5] );
	m_planes[5].plane			= math::create_plane( vertices[7], vertices[6], vertices[3] );

	aabb_plane* i				= &m_planes[0];
	aabb_plane const* const e	= &m_planes[plane_count];
	for ( ; i != e; ++i )
		(*i).plane				= transform( (*i).plane, matrix );
}

cuboid::cuboid					( cuboid const& other, float4x4 const& matrix )
{
	aabb_plane* i				= &m_planes[0];
	aabb_plane const* const e	= &m_planes[plane_count];
	aabb_plane const* j			= &other.m_planes[0];
	for ( ; i != e; ++i, ++j ) {
		(*i).plane				= transform( (*j).plane, matrix );
		(*i).normalize			( );
	}
}

intersection cuboid::test_inexact ( aabb const& aabb ) const
{
	u32 inside_count				= 0;

	aabb_plane const* i				= m_planes;
	aabb_plane const* const	e		= m_planes + plane_count;
	for ( ; i != e; ++i) {
		switch ( (*i).test(aabb) ) {
			case intersection_outside	:
				return				intersection_outside;
			case intersection_intersect :
				continue;
			case intersection_inside	: {
				++inside_count;
				continue;
			}
			default	:		NODEFAULT();
		}
	}

	if ( inside_count < plane_count )
		return						intersection_intersect;

	ASSERT							( inside_count == plane_count );
	return							intersection_inside;
}

intersection cuboid::test				( sphere const& sphere ) const
{
	bool inside						= true;

	aabb_plane const* i				= m_planes;
	aabb_plane const* const e		= m_planes + plane_count;
	for ( ; i != e; ++i ) {
		plane const& plane			= i->plane;
		float const signed_distance	= plane.classify( sphere.center );
		if ( signed_distance + sphere.radius < 0.f )
			return					intersection_outside;

		inside						= inside && ( signed_distance - sphere.radius <= 0.f );
	}

	if ( inside )
		return						intersection_inside;

	return							intersection_intersect;
}