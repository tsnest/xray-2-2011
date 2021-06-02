////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_CONSTANTS_H_INCLUDED
#define XRAY_MATH_CONSTANTS_H_INCLUDED

#include <limits>

namespace xray {

template < typename first_type, typename second_type >
union horrible_cast {
	first_type			first;
	second_type			second;

	inline	horrible_cast	( first_type const& first ) :
		first			( first )
	{
	}
}; // union horrible_cast

namespace math {

float const	SNaN		= horrible_cast< int, float > ( 0x7f800001 ).second;	// 0x7f800001..0x7fbfffff and 0xff800001..0xffbfffff
float const	QNaN		= horrible_cast< int, float > ( 0x7fc00001 ).second;	// 0x7fc00001..0x7fffffff and 0xffc00001..0xffffffff

float const	infinity	= horrible_cast< int, float > ( 0x7f800000 ).second;	// -infinity = 0xff800000

float const epsilon_3	= .001f;
float const epsilon_5	= .00001f;
float const epsilon_6	= .000001f;
float const epsilon_7	= .0000001f;

float const pi			= 3.1415926535897932384626433832795f;
float const pi_x2		= 6.2831853071795864769252867665590f;
float const pi_x3		= 9.4247779607693797153879301498385f;
float const pi_x4		= 12.566370614359172953850573533118f;
float const pi_x6		= 18.849555921538759430775860299677f;
float const pi_x8		= 25.132741228718345907701147066236f;
float const pi_d2		= 1.5707963267948966192313216916398f;
float const pi_d3		= 1.0471975511965977461542144610932f;
float const pi_d4		= 0.7853981633974483096156608458199f;
float const pi_d6		= 0.5235987755982988730771072305466f;
float const pi_d8		= 0.3926990816987241548078304229099f;

float const float_max	=  std::numeric_limits< float >::max( );
float const float_min	= -std::numeric_limits< float >::max( );

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_CONSTANTS_H_INCLUDED