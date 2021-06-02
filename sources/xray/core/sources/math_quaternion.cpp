////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::math::quaternion;
using xray::math::float4x4;

static float const zero_tolerance = 0.1f; // ?!

inline float sqrt_safe( float val )
{
	return val<=0.f ? 0 : sqrt( val );	
}

quaternion::quaternion	( float4x4 matrix )
{
	matrix.set_scale( float3(1.f, 1.f, 1.f) );
	float const	trace = matrix.e00 + matrix.e11 + matrix.e22;
	float		s;
	if (trace > 0.0f){
		s = sqrt(trace + 1.0f);
		w 		= s * 0.5f;
		s 		= 0.5f / s;

		x 		= ( matrix.e21 - matrix.e12 ) * s;
		y 		= ( matrix.e02 - matrix.e20 ) * s;
		z 		= ( matrix.e10 - matrix.e01 ) * s;
		return;
	}

	enum {
		A,
		E,
		I
	};

	int biggest;
	if (matrix.e00 > matrix.e11){
		if (matrix.e22 > matrix.e00)
			biggest = I;	
		else
			biggest = A;
	}
	else {
		if (matrix.e22 > matrix.e00)
			biggest = I;
		else
			biggest = E;
	}

	// in the unusual case the original trace fails to produce a good sqrt, try others...
	switch (biggest){
		case A:
			s = sqrt_safe( matrix.e00 - (matrix.e11 + matrix.e22) + 1.0f);
			if (s > zero_tolerance){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e21 - matrix.e12) * s;
				y = (matrix.e01 + matrix.e10) * s;
				z = (matrix.e02 + matrix.e20) * s;
				break;
			}
			// I
			s = sqrt_safe( matrix.e22 - (matrix.e00 + matrix.e11) + 1.0f);
			if (s > zero_tolerance){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e10 - matrix.e01) * s;
				x = (matrix.e20 + matrix.e02) * s;
				y = (matrix.e21 + matrix.e12) * s;
				break;
			}
			// E
			s = sqrt_safe( matrix.e11 - (matrix.e22 + matrix.e00) + 1.0f);
			if (s > zero_tolerance){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e02 - matrix.e20) * s;
				z = (matrix.e12 + matrix.e21) * s;
				x = (matrix.e10 + matrix.e01) * s;
				break;
			}
			break;
		case E:
			s = sqrt_safe( matrix.e11 - (matrix.e22 + matrix.e00) + 1.0f);
			if (s > zero_tolerance){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e02 - matrix.e20) * s;
				z = (matrix.e12 + matrix.e21) * s;
				x = (matrix.e10 + matrix.e01) * s;
				break;
			}
			// I
			s = sqrt_safe( matrix.e22 - (matrix.e00 + matrix.e11) + 1.0f);
			if (s > zero_tolerance){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e10 - matrix.e01) * s;
				x = (matrix.e20 + matrix.e02) * s;
				y = (matrix.e21 + matrix.e12) * s;
				break;
			}
			// A
			s = sqrt_safe( matrix.e00 - (matrix.e11 + matrix.e22) + 1.0f);
			if (s > zero_tolerance){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e21 - matrix.e12) * s;
				y = (matrix.e01 + matrix.e10) * s;
				z = (matrix.e02 + matrix.e20) * s;
				break;
			}
			break;
		case I:
			s = sqrt_safe( matrix.e22 - (matrix.e00 + matrix.e11) + 1.0f);
			if (s > zero_tolerance){
				z = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e10 - matrix.e01) * s;
				x = (matrix.e20 + matrix.e02) * s;
				y = (matrix.e21 + matrix.e12) * s;
				break;
			}
			// A
			s = sqrt_safe( matrix.e00 - (matrix.e11 + matrix.e22) + 1.0f);
			if (s > zero_tolerance){
				x = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e21 - matrix.e12) * s;
				y = (matrix.e01 + matrix.e10) * s;
				z = (matrix.e02 + matrix.e20) * s;
				break;
			}
			// E
			s = sqrt_safe( matrix.e11 - (matrix.e22 + matrix.e00) + 1.0f);
			if (s > zero_tolerance){
				y = s * 0.5f;
				s = 0.5f / s;
				w = (matrix.e02 - matrix.e20) * s;
				z = (matrix.e12 + matrix.e21) * s;
				x = (matrix.e10 + matrix.e01) * s;
				break;
			}
			break;
	}
	R_ASSERT		( is_unit() );
}

quaternion xray::math::slerp ( quaternion const& left, quaternion const& right, float const time_delta )
{
	R_ASSERT		( left.is_unit() );
	R_ASSERT		( right.is_unit() );
	R_ASSERT		( (time_delta >= 0.f) && (time_delta <= 1.f), "Quaternion::slerp - invalid 'time_delta' arrived: %f", time_delta );
	
	float cosom	=  left.vector.x * right.vector.x +  
				   left.vector.y * right.vector.y +
				   left.vector.z * right.vector.z +
				   left.vector.w * right.vector.w;

	float		Scale0, Scale1, sign;
	if ( cosom < 0.f ) {
		cosom	= -cosom;
		sign	= -1.f;
	} else {
		sign	= 1.f;
	}
	
	if ( (1.0f - cosom) > epsilon_5 ) {
		float	omega	= acos( cosom );
		float	i_sinom = 1.f / sin( omega );
		float	t_omega	= time_delta*omega;
		Scale0	= sin( omega - 	t_omega ) * i_sinom;
		Scale1	= sin( t_omega			) * i_sinom;
	}
	else {
		// has numerical difficulties around cosom == 0
		// in this case degenerate to linear interpolation
		Scale0	= 1.0f - time_delta;
		Scale1	= time_delta;
	}

	Scale1		*= sign;
	return		quaternion( left.vector*Scale0 + right.vector*Scale1 );
}