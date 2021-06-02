////////////////////////////////////////////////////////////////////////////
//	Module 		: math_functions_inline.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : MATH functions inline
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_FUNCTIONS_INLINE_H_INCLUDED
#define CS_CORE_MATH_FUNCTIONS_INLINE_H_INCLUDED

#pragma warning(push)
#pragma warning(disable:4995)

#define MATH	cs::core::math

inline float MATH::_abs					(float const number_)
{
//	return			fabsf(number_);
	float			number = number_;
	u32&			integer = (u32&)number;
	integer			&= 0x7fffffff;
	return			number;
}

inline int	 MATH::_abs					(int const number)
{
	int				result = (number >> 31);
	return			(number ^ result) - result;
}

inline float MATH::_sqrt				(float const number)
{
	return			sqrtf(number);
}

inline float MATH::_sin					(float const angle)
{
	return			sinf(angle);
}

inline float MATH::_cos					(float const angle)
{
	return			cosf(angle);
}

inline float MATH::_tan					(float const angle)
{
	return			tanf(angle);
}

inline void MATH::_sincos				(float const angle, float& sine, float& cosine)
{
	sine			= _sin(angle);
	cosine			= _cos(angle);
}

inline bool MATH::fp_zero				(float const number, float const epsilon)
{
	return			_abs(number) < epsilon;
}

inline bool MATH::fp_similar			(float const number0, float const number1, float const epsilon)
{
	return			fp_zero(number0 - number1, epsilon);
}

template <typename T>
inline T MATH::_sqr						(T const& number)
{
	return			number*number;
}

template <typename T>
inline T MATH::_min						(T const& x, T const& y)
{
	if (x < y)
		return		x;
	
	return			y;
}

inline s8 MATH::_min					(s8 const x, s8 const y)
{
	return			(
		y + (
			(x - y) & (
				(x - y) >> (sizeof(s8) * 8 - 1)
			)
		)
	);
}

inline s16 MATH::_min					(s16 const x, s16 const y)
{
	return		(
		y + (
			(x - y) & (
				(x - y) >> (sizeof(s16) * 8 - 1)
			)
		)
	);
}

inline s32 MATH::_min					(s32 const x, s32 const y)
{
	return		(
		y + (
			(x - y) & (
				(x - y) >> (sizeof(s32) * 8 - 1)
			)
		)
	);
}

inline s64 MATH::_min					(s64 const& x, s64 const& y)
{
	return			(
		y + (
			(x - y) & (
				(x - y) >> (sizeof(s64) * 8 - 1)
			)
		)
	);
}

template <typename T>
inline T MATH::_max					(T const& x, T const& y)
{
	if (x > y)
		return		x;
	
	return			y;
}

inline s8 MATH::_max					(s8 const x, s8 const y)
{
	return			(
		x - (
			(x - y) & (
				(x - y) >> (sizeof(s8) * 8 - 1)
			)
		)
	);
}

inline s16 MATH::_max					(s16 const x, s16 const y)
{
	return			(
		x - (
			(x - y) & (
				(x - y) >> (sizeof(s16) * 8 - 1)
			)
		)
	);
}

inline s32 MATH::_max					(s32 const x, s32 const y)
{
	return			(
		x - (
			(x - y) & (
				(x - y) >> (sizeof(s32) * 8 - 1)
			)
		)
	);
}

inline s64 MATH::_max					(s64 const& x, s64 const& y)
{
	return			(
		x - (
			(x - y) & (
				(x - y) >> (sizeof(s64) * 8 - 1)
			)
		)
	);
}

inline MATH::sine_cosine::sine_cosine	(float const angle)
{
	_sincos			(angle,sine,cosine);
}

inline int MATH::i_floor				(float const number)
{
    int a			= *(int const*)(&number);
    int exponent	= (127 + 31) - ((a >> 23) & 0xff);
    int result		= (((u32)(a) << 8) | (u32(1) << 31)) >> exponent;
    exponent		+= 31-127;
    {
		int			imask = !((((1 << exponent) - 1) >> 8) & a);
        exponent	-=	(31-127)+32;
        exponent	>>=	31;
        a			>>=	31;
        result		-=	(imask&a);
        result		&=	exponent;
        result		^=	a;
    }
    return			result;
}

inline int MATH::i_ceil					(float const number)
{
    int a			= *(int const*)(&number);
    int exponent	= (127 + 31) - ((a >> 23) & 0xff);
    int result		= (((u32)(a) << 8) | (u32(1) << 31)) >> exponent;
    exponent		+= 31-127;
    {
		int			imask = !((((1 << exponent) - 1) >> 8) & a);
        exponent	-=	(31-127)+32;
        exponent	>>=	31;
        a			=	~((a-1)>>31);		
        result		-=	(imask&  a);
        result		&=	exponent;
        result		^=	a;
        result		=	-result;                 
    }
    return			result;
}

inline float MATH::deg2rad				(float const degrees)
{
	return			degrees*PI_DIV_180;
}

inline float MATH::rad2deg				(float const radians)
{
	return			radians*_180_DIV_PI;
}

template <class T>
inline T MATH::clampr					(T const& value, T const& low, T const& high)
{
	if (value <= low)
		return	low;

	if (value <= high)
		return	value;

	return		high;
}

template <class T>
inline void MATH::clamp					(T& result, T const& low, T const& high)
{
	result		= clampr(result,low,high);
};

#undef MATH

#pragma warning(pop)

#endif // #ifndef CS_CORE_MATH_FUNCTIONS_INLINE_H_INCLUDED