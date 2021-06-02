////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_aabb.h>

using xray::math::aabb;
using xray::math::float3;
using xray::math::float4x4;

static inline void process	( float const test, float& min, float& max )
{
	if ( xray::math::negative( test ) )
		min			+= test;
	else
		max			+= test;
}

aabb& aabb::modify			( float4x4 const& transform )
{
	float3 const	double_size = max - min;
	float3 const vx	= transform.i.xyz() * double_size.x;
	float3 const vy = transform.j.xyz() * double_size.y;
	float3 const vz = transform.k.xyz() * double_size.z;

	max = min		= min*transform;

	process			( vx.x, min.x, max.x );
	process			( vx.y, min.y, max.y );
	process			( vx.z, min.z, max.z );
	process			( vy.x, min.x, max.x );
	process			( vy.y, min.y, max.y );
	process			( vy.z, min.z, max.z );
	process			( vz.x, min.x, max.x );
	process			( vz.y, min.y, max.y );
	process			( vz.z, min.z, max.z );

	return				( *this );
}