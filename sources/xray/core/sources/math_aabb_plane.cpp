////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
//	Description : http://www.flipcode.com/archives/Octrees_For_Visibility.shtml
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_aabb_plane.h>

using xray::math::aabb_plane;
using xray::math::intersection;
using xray::math::aabb;

static u32 const min_x	= 0;
static u32 const min_y	= 1;
static u32 const min_z	= 2;
static u32 const max_x	= 3;
static u32 const max_y	= 4;
static u32 const max_z	= 5;
static u32 const aabb_lut[8][3]	= {
	{ min_x, min_y, min_z }, 
	{ max_x, min_y, min_z }, 
	{ min_x, max_y, min_z }, 
	{ max_x, max_y, min_z }, 
	{ min_x, min_y, max_z }, 
	{ max_x, min_y, max_z }, 
	{ min_x, max_y, max_z }, 
	{ max_x, max_y, max_z }
};

static u32 lut_id				( xray::math::float3 const& normal )
{
	// there is an error in the paper, where << 2 is for x, not for z
	u32 const x			= ((*(u32*)&normal.x >> 31) ^ 1) << 0; // if ( normal.x >= 0.f ) x = 1; else x = 0;
	u32	const y			= ((*(u32*)&normal.y >> 31) ^ 1) << 1; // if ( normal.y >= 0.f ) y = 2; else y = 0;
	u32	const z			= ((*(u32*)&normal.z >> 31) ^ 1) << 2; // if ( normal.z >= 0.f ) z = 4; else z = 0;
	return				x | y | z;
}

void aabb_plane::setup_lut_id	( )
{
	m_lut_id			= lut_id( plane.normal );
	m_lut_id			|= (m_lut_id ^ 7) << 3;
}

void aabb_plane::transform		( xray::math::float4x4 const& transform )
{
	plane.transform		( transform );
	setup_lut_id		( );
}

void aabb_plane::normalize		( )
{
	R_ASSERT			( plane.normal.length() > 0.f );
	plane.vector		*= 1.f / plane.normal.length( );
	setup_lut_id		( );
}

intersection aabb_plane::test	( aabb const& aabb ) const
{
	typedef float		values_type[6];
	values_type const&	values = (values_type const&)aabb.min.x;

	typedef u32			lut_ids_type[3];
	lut_ids_type const&	positive_ids = aabb_lut[ m_lut_id & 7 ];

	// Test for completely within the negative halfspace of this plane
	//
	// If the halfplane test returns negative, the negTestPoint (as
	// well as the rest of the node) is within the negative halfspace,
	// so we can bail early.
	if ( plane.classify( float3( values[ positive_ids[0] ], values[ positive_ids[1] ], values[ positive_ids[2] ] ) ) < 0.f )
		return			intersection_outside;

	lut_ids_type const&	negative_ids = aabb_lut[ m_lut_id >> 3 ];
	// Test for completely within the positive halfspace of this plane
	//
	// If the halfplane test returns negative, the posTestPoint is _not_
	// completely within the positive halfspace, and therefore must be
	// bisected by the plane.
	if ( plane.classify( float3( values[ negative_ids [0] ], values[ negative_ids [1] ], values[ negative_ids [2] ] ) ) < 0.f )
		return			intersection_intersect;

	return				intersection_inside;
}