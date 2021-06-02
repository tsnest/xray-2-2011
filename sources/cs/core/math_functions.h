////////////////////////////////////////////////////////////////////////////
//	Module 		: math_functions.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : math functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATH_FUNCTIONS_H_INCLUDED
#define CS_CORE_MATH_FUNCTIONS_H_INCLUDED

#include <math.h>
#include <cs/core/math_constants.h>

namespace cs {

namespace core {

namespace math {

inline bool		fp_zero		(float const number, float const epsilon = EPS_7);
inline bool		fp_similar	(float const number0, float const number1, float const epsilon = EPS_5);

inline float	_abs		(float const number);
inline int		_abs		(int const   number);
inline float	_sqrt		(float const number);
inline float	_sin		(float const angle);
inline float	_cos		(float const angle);
inline float	_tan		(float const angle);
inline void		_sincos		(float const angle, float& sine, float& cosine);

template <typename T>
inline T		_sqr		(T const& number);

template <typename T>
inline T		_min		(T const& x, T const& y);
inline s8		_min		(s8 const x, s8 const y);
inline s16		_min		(s16 const x, s16 const y);
inline s32		_min		(s32 const x, s32 const y);
inline s64		_min		(s64 const& x, s64 const& y);

template <typename T>
inline T		_max		(T const& x, T const& y);
inline s8		_max		(s8 const x, s8 const y);
inline s16		_max		(s16 const x, s16 const y);
inline s32		_max		(s32 const x, s32 const y);
inline s64		_max		(s64 const& x, s64 const& y);

inline int		i_floor		(float const number);
inline int		i_ceil		(float const number);

inline float	deg2rad		(float const degrees);
inline float	rad2deg		(float const radians);

template <class T>
inline T		clampr		(T const& value, T const& low, T const& high);

template <class T>
inline void		clamp		(T& result, T const& low, T const& high);

struct sine_cosine {
public:
	float		sine;
	float		cosine;

public:
	inline		sine_cosine	(float const angle);
};

} // namespace math

} // namespace core

} // namespace cs

#include <cs/core/math_functions_inline.h>

#endif // #ifndef CS_CORE_MATH_FUNCTIONS_H_INCLUDED