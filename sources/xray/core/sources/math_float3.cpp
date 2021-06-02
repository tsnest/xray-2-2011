////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
#	define XRAY_COPY_SIGN	_copysign
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX360
#	include <SnMathLibPPU.h>
#	define XRAY_COPY_SIGN	copysign
#else // #elif XRAY_PLATFORM_PS3
#	error define XRAY_COPY_SIGN for your platform here
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX360

using xray::math::float3;

void xray::math::generate_orthonormal_basis	( float3 const& direction, float3& up, float3& right )
{
    if ( abs( direction.x ) >= abs( direction.y ) ) {
        // W.x or W.z is the largest length component, swap them
        float const invert_length = 1.f/sqrt( sqr( direction.x ) + sqr( direction.z ) );
        up.x		= -direction.z*invert_length;
        up.y		= 0.f;
        up.z		= +direction.x*invert_length;
    }
    else {
        // W.y or W.z is the largest length component, swap them
        float const invert_length = 1.f/sqrt( sqr( direction.y ) + sqr( direction.z ) );
        up.x		= 0.f;
        up.y		= +direction.z*invert_length;
        up.z		= -direction.y*invert_length;
    }

    right			= up ^ direction;
}

inline double rsqrt							( double const value )
{
	return 1.0/sqrt(value);
}

float3 xray::math::normalize_precise		( float3 const& object )
{
	double	sqr_length	= object.squared_length();
	double	epsilon			= 1.192092896e-05F;
	if		(sqr_length > epsilon) {
		double const l = rsqrt(sqr_length);
		return	float3 ( float( l*double(object.x) ), float( l*double(object.y) ), float( l*double(object.z) ) ) ;
	}

	double a0,a1,a2,aa0,aa1,aa2,l;
	a0 = object.x;
	a1 = object.y;
	a2 = object.z;
	aa0 = ::abs(a0);
	aa1 = ::abs(a1);
	aa2 = ::abs(a2);
	if (aa1 > aa0) {
		if (aa2 > aa1)
			goto	aa2_largest;

		a0			/= aa1;
		a2			/= aa1;
		l			= rsqrt (a0*a0 + a2*a2 + 1);

		float3		result;
		result.x	= float(a0*l);
		result.y	= float(XRAY_COPY_SIGN(l,a1));
		result.z	= float(a2*l);
		return		result;
	}

	if (aa2 > aa0) {
aa2_largest:	// aa2 is largest
		a0			/= aa2;
		a1			/= aa2;
		l			= rsqrt (a0*a0 + a1*a1 + 1);

		float3		result;
		result.x	= float(a0*l);
		result.y	= float(a1*l);
		result.z	= float(XRAY_COPY_SIGN(l,a2));
		return		result;
	}

	if (aa0 <= 0)
		return		float3( 0.f, 1.f, 0.f );

	a1				/= aa0;
	a2				/= aa0;
	l				= rsqrt (a1*a1 + a2*a2 + 1);
	float3			result;
	result.x		= float(XRAY_COPY_SIGN(l,a0));
	result.y		= float(a1*l);
	result.z		= float(a2*l);
	return			result;
}
