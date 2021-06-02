////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_obb.h>

using xray::math::obb;
using xray::math::float3;

static bool clip		( float const denominator, float numerator, float& t0, float& t1 )
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if ( denominator == 0.0f )
		return						( numerator <= 0.f );

	if ( denominator > 0.f ) {
        if ( numerator > denominator*t1 )
			return					( false );

        if ( numerator > denominator*t0 )
			t0						= numerator/denominator;

        return						( true );
	}

	if ( numerator > denominator*t0 )
		return						( false );

	if ( numerator > denominator*t1 )
		t1							= numerator/denominator;

	return							( true );
}

static bool intersect	( float3 const& position, float3 const& direction, float3 const& extent, float& t0, float& t1)
{
	float const	safe_t0				= t0;
	float const	safe_t1				= t1;
	bool const entirely_clipped =
		(
			!clip ( +direction.x, -position.x - extent.x, t0, t1) ||
			!clip ( -direction.x, +position.x - extent.x, t0, t1) ||
			!clip ( +direction.y, -position.y - extent.y, t0, t1) ||
			!clip ( -direction.y, +position.y - extent.y, t0, t1) ||
			!clip ( +direction.z, -position.z - extent.z, t0, t1) ||
			!clip ( -direction.z, +position.z - extent.z, t0, t1)
		);

	if ( entirely_clipped )
		return						( false );

	return							( ( t0 != safe_t0 ) || ( t1 != safe_t1 ) );
}

bool obb::intersect		( float3 const& position, float3 const& direction, type& distance )
{
	float3 const center2position	= position - c.xyz( );
	float3 const test_position		= float3( center2position | i.xyz( ), center2position | j.xyz( ), center2position | k.xyz( ) );
	float3 const test_direction		= float3( direction | i.xyz( ), direction | j.xyz( ), direction | k.xyz( ) );
	float							t0, t1;
	if ( !::intersect( test_position, test_direction, float3( e00, e11, e22 ) * .5f, t0, t1 ) )
		return						( false );

	if ( t0 <= 0.f ) {
		if ( t1 >= distance )
			return					( false );

		distance					= t1;
		return						( true );
	}

	if ( t0 < distance ) {
		distance					= t0;
		return						( true );
	}

	if ( t1 < distance ) {
		distance					= t1;
		return						( true );
	}

	return							( false );
}
