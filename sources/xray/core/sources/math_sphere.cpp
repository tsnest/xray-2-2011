////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(core_math_sphere)

using xray::math::sphere;
using xray::math::float3;
using xray::math::intersection;

intersection sphere::intersect_ray	( float3 const& position, float3 const& direction, type range, u32& count, type (&results)[2] ) const
{
	ASSERT				( math::is_similar( direction.length(), 1.f ) );
	float3 const		center2position = position - center;
	type const a		= sqr( range );
	type const b		= ( center2position | direction ) * range;
	type const c		= center2position.squared_length() - sqr( radius );
	type const			discriminant = sqr( b ) - a*c;
	if ( discriminant < 0.f ) {
		count			= 0;
		return			( intersection_none );
	}

	if ( discriminant == 0.f ) {
		results[0]		= range*( -b/a );
		if ( results[0] >= 0.f ) {
			count		= 1;
			return		( intersection_outside );
		}

		count			= 0;
		return			( intersection_none );
	}

	float const root	= sqrt( discriminant );
	float invert_a 		= 1.f/a;
	results[0]			= range*( -b - root)*invert_a;
	results[1]			= range*( -b + root)*invert_a;

	if ( results[0] >= 0.f ) {
		count			= 2;
		return			( intersection_outside );
	}

	if ( results[1] >= 0.f ) {
		count			= 1;
		results[0]		= results[1];
		return			( intersection_inside );
	}

	count				= 0;
	return				( intersection_none);
}

intersection sphere::intersect_ray	( float3 const& position, float3 const& direction, type& range ) const
{
	float3 const		position2center = center - position;
	float const			radius_sqr = sqr( radius );
	float const			distance_sqr = position2center.squared_length	();
	float v				= position2center | direction;
	float d				= radius_sqr - ( distance_sqr - sqr( v ) );

	if (d <= 0.f)
		return			( intersection_none );

	float const distance= v - sqrt( d );
	if ( distance >= range )
		return			( intersection_none );

	range				= distance;
	return				( distance_sqr < radius_sqr ? intersection_inside : intersection_outside );
}