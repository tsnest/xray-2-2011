////////////////////////////////////////////////////////////////////////////
//	Module 		: math_constants.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : math constants
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_CONSTANTS_H_INCLUDED
#define CS_CORE_MATH_CONSTANTS_H_INCLUDED

#include <limits>
#include <cs/config.h>

namespace cs {

namespace core {

namespace math {

const float EPS_3		= .0010000f;
const float EPS_5		= .0000100f;
const float EPS_7		= .0000001f;

const float	PI			= 3.1415926535897932384626433832795f;

const float	PI_MUL_2	= 6.2831853071795864769252867665590f;
const float	PI_MUL_4	= 12.566370614359172953850573533118f;

const float	PI_DIV_2	= 1.5707963267948966192313216916398f;
const float	PI_DIV_4	= 0.7853981633974483096156608458199f;

const float	PI_DIV_180	= PI/180.f;
const float _180_DIV_PI = 180.f/PI;

} // namespace math

} // namespace core

} // namespace cs

#define type_max(T)		(std::numeric_limits<T>::max())
#define type_min(T)		(-std::numeric_limits<T>::max())
#define type_eps(T)		(-std::numeric_limits<T>::epsilon())

#ifndef flt_max
#	define flt_max		type_max(float)
#endif // flt_max

#ifndef flt_min
#	define flt_min		type_min(float)
#endif // flt_max

#ifndef flt_eps
#	define flt_eps		type_eps(float)
#endif // flt_max

#endif // #ifndef CS_CORE_MATH_CONSTANTS_H_INCLUDED