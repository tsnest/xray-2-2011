////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FUNCTIONS_INLINE_H_INCLUDED
#define XRAY_MATH_FUNCTIONS_INLINE_H_INCLUDED

#include "math_float3.h"

namespace xray {
namespace math {

template < typename T >
inline typename T::type	dot_product	( T const& left, T const& right )
{
	R_ASSERT	( valid(left) );
	R_ASSERT	( valid(right) );
	return		( left.dot_product( right ) );
}

template < typename T >
inline T sqr			( T const& value )
{
	R_ASSERT	( valid(value) );
	return		( value * value );
}

// min
template < typename T >
inline T min			( T const& left, T const& right )
{
	R_ASSERT	( valid(left) );
	R_ASSERT	( valid(right) );
	return		( left < right ? left : right );
}

template < typename T >
inline T min			( T const& value1, T const& value2, T const& value3 )
{
	return		min(value1, min(value2, value3));
}

template < typename T >
inline T min			( T const& value1, T const& value2, T const& value3, T const& value4 )
{
	return		min(value1, min(value2, min(value3, value4)));
}

template < typename T >
inline T min_integral	( T const& left, T const& right )
{
	return		( right + ( ( left - right) & -( left < right ) ) );
}

inline s8 min			( s8 left, s8 right )
{
	return		( min_integral( left, right ) );
}

inline s16 min			( s16 left, s16 right )
{
	return		( min_integral( left, right ) );
}

inline s32 min			( s32 left, s32 right )
{
	return		( min_integral( left, right ) );
}

inline s64 min			( s64 left, s64 right )
{
	return		( min_integral( left, right ) );
}

inline u8 min			( u8 left, u8 right )
{
	return		( min_integral( left, right ) );
}

inline u16 min			( u16 left, u16 right )
{
	return		( min_integral( left, right ) );
}

inline u32 min			( u32 left, u32 right )
{
	return		( min_integral( left, right ) );
}

inline u64 min			( u64 left, u64 right )
{
	return		( min_integral( left, right ) );
}

// max
template < typename T >
inline T max			( T const& left, T const& right )
{
	return		( left > right ? left : right );
}

template < typename T >
inline T max			( T const& value1, T const& value2, T const& value3 )
{
	return		max(value1, max(value2, value3));
}

template < typename T >
inline T max			( T const& value1, T const& value2, T const& value3, T const& value4 )
{
	return		max(value1, max(value2, max(value3, value4)));
}

template < typename T >
inline T max_integral	( T const& left, T const& right )
{
	return		( left - ( ( left - right ) & -( left < right ) ) );
}

inline s8 max			( s8 left, s8 right )
{
	return		( max_integral( left, right ) );
}

inline s16 max			( s16 left, s16 right )
{
	return		( max_integral( left, right ) );
}

inline s32 max			( s32 left, s32 right )
{
	return		( max_integral( left, right ) );
}

inline s64 max			( s64 left, s64 right )
{
	return		( max_integral( left, right ) );
}

inline u8 max			( u8 left, u8 right )
{
	return		( max_integral( left, right ) );
}

inline u16 max			( u16 left, u16 right )
{
	return		( max_integral( left, right ) );
}

inline u32 max			( u32 left, u32 right )
{
	return		( max_integral( left, right ) );
}

inline u64 max			( u64 left, u64 right )
{
	return		( max_integral( left, right ) );
}

inline float abs		( float value )
{
	R_ASSERT	( valid(value) );
	u32			bit_value = (*(u32*)&value) & 0x7fffffff;
	return		( *( float* )&bit_value );
}

// http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
inline int abs			( int value )
{
	int const	mask = value >> ( sizeof(int) * 8 - 1 );
	return		( (value + mask) ^ mask );
}

inline s64 abs			( s64 value )
{
	int const	mask = value >> ( sizeof(s64) * 8 - 1 );
	return		( (value + mask) ^ mask );
}

template < typename T >
inline bool is_similar		( T const& left, T const& right, float const epsilon )
{
	return		( abs( left - right) < epsilon );
}

template < typename T >
inline bool is_similar		( T const& left, T const& right, typename T::type const& epsilon )
{
	return		( left.is_similar( right, epsilon ) );
}

inline bool	is_relatively_similar	( float left, float right, float epsilon )
{
	float const denominator	= max( abs(left), abs(right) );
	if ( denominator != 0.f )
		return	(abs( left - right) / denominator) < epsilon;

	return		is_similar( left, right, epsilon );
}

template < typename T >
inline bool is_zero		( T const& value, T const& epsilon )
{
	return		( is_similar( value, T(0), epsilon ) );
}

inline bool is_relatively_zero	( float bigger_value, float smaller_value, float const epsilon )
{
	bigger_value			= abs( bigger_value );
	smaller_value			= abs( smaller_value );
	if ( bigger_value < smaller_value )
		return				false;

	if ( bigger_value != 0.f )
		return				smaller_value/bigger_value < epsilon;

	return					true;
}


inline float cos		( float const angle )
{
	return		( cosf( angle ) );
}

inline float sin		( float const angle )
{
	return		( sinf( angle ) );
}

inline float tan		( float const angle )
{
	return		( tanf( angle ) );
}

inline float acos		( float const value )
{
	return		( acosf( value ) );
}

inline float asin		( float const value )
{
	return		( asinf( value ) );
}

inline float atan		( float const value )
{
	return		( atanf( value ) );
}

inline float atan2		( float const x, float const y )
{
	return		( atan2f( x, y ) );
}

inline float sqrt		( float const value )
{
	return		( sqrtf( value ) );
}

inline float log		( float const value )
{
	return		( logf( value ) );
}

inline float exp		( float power )
{
	return		( expf( power ));
}

inline float pow		( float base, float power )
{
	return		( powf( base, power ) );
}

inline float pow_impl	( float const base, u32 const power )
{
	float		result = base;
	for (u32 i=1; i<power; ++i)
		result	*= base;

	return		result;
}

inline float pow		( float const base, int const power )
{
	R_ASSERT	( !is_zero(base) );

	if ( !power )
		return	1.f;

	if ( power < 0 )
		return	pow_impl( 1.f/base, u32(-power) );

	return		pow_impl( base, u32(power) );
}

inline float pow		( float const base, u32 const power )
{
	R_ASSERT	( !is_zero(base) );

	if ( !power )
		return	1.f;

	return		pow_impl( base, power );
}

inline u32 pow			( u32 const base, u32 const power )
{
	if ( !power )
		return	1;

	u32 result	= base;
	for (u32 i=1; i<power; ++i)
		result	*= base;

	return		result;
}

inline char sign		( float value )
{
//	return		( (horrible_cast<float,int>(value).second & 0x80000000) | 0x3f800000 );
	if ( value > 0.f )
		return	1;

	if ( value < 0.f )
		return	-1;

	return		0;
}

inline bool	is_infinity	( float const value )
{
	int const	value_bits = horrible_cast<float,int>(value).second;

	// +infinity, -infinity
	if ( (value_bits & 0x7f800000) == 0x7f800000 )
		return	true;

	return		false;
}

inline bool	is_denormal	( float const value )
{
	int const	value_bits = horrible_cast<float,int>(value).second;

	// +denormal, -denormal
	if ( ( ( (value_bits & 0x7f800000) == 0 ) ) && ( ( value_bits & 0x007fffff ) != 0) )
		return	true;

	return		false;
}

inline bool	is_quiet_not_a_number	( float const value )
{
	int const	value_bits = horrible_cast<float,int>(value).second;

	// +QNaN, -QNaN
	if ( ( ( (value_bits & 0x7f800000) == 0x7f800000 ) ) && ( ( value_bits & 0x00400000 ) != 0) )
		return	true;

	return		false;
}

inline bool	is_signalling_not_a_number	( float const value )
{
	int const	value_bits = horrible_cast<float,int>(value).second;

	// +SNaN, -SNaN
	if ( ( ( (value_bits & 0x7f800000) == 0x7f800000 ) ) && ( ( value_bits & 0x00400000 ) == 0)  && ( ( value_bits & 0x003fffff ) != 0) )
		return	true;

	return		false;
}

inline bool	is_not_a_number	( float const value )
{
	return		is_signalling_not_a_number( value ) || is_quiet_not_a_number( value );
}

#ifndef MASTER_GOLD

inline bool valid		( float const& value )
{
	if( is_not_a_number( value ) )
		return false;

	//if( is_infinity( value ) )
	//	return false;

	if( is_denormal( value ) )
	{
		const_cast<float&>(value) = 0;
		return	true;
	}

	return		true;
}

#endif

inline int floor		( float const value )
{
	R_ASSERT		( valid( value ) );
	int value_raw	= *(int const * )( &value );
	value_raw		= value_raw ^ ((value_raw ^ (value_raw & (value_raw - 1))) & 0x80000000);
	int exponent	= ( 127 + 31 ) - ( ( value_raw >> 23 ) & 0xff );
	int result		= (((u32)(value_raw) << 8 ) | ( 1U << 31 ) ) >> exponent;
	exponent		+= 31 - 127;
	int imask		= (!(((( (1<<(exponent)))-1)>>8) & value_raw ) );
	exponent		-= (31 - 127) + 32;
	exponent		>>= 31;
	value_raw		>>=	31;
	result			-= imask & value_raw;
	result			&= exponent;
	result			^= value_raw;
	return			result;
}

inline int ceil			( float const value )
{
	R_ASSERT		( valid( value ) );
	int value_raw	= *( int const* )( &value );
	value_raw		= value_raw ^ ((value_raw ^ (value_raw & (value_raw - 1))) & 0x80000000);
	int exponent	= ( 127 + 31 ) - ( (value_raw >> 23) & 0xff );
	int result		= (((u32)(value_raw) << 8) | (1U << 31)) >> exponent;
	exponent		+= 31 - 127;
	int imask		= (!(((( (1 << (exponent))) - 1) >> 8) & value_raw ) );
	exponent		-= (31 - 127) + 32;
	exponent		>>=	31;
	value_raw		= ~((value_raw - 1) >> 31);		/* change sign */
	result			-= imask & value_raw;
	result			&= exponent;
	result			^= value_raw;
	return			-result;
}

inline sine_cosine::sine_cosine	( float const angle ) :
	sine			( sin( angle ) ),
	cosine			( cos( angle ) )
{
}

template < typename T >
inline T type_epsilon	( )
{
	return			( std::numeric_limits< T >::epsilon( ) );
}

inline bool negative	( float value )
{
	return			( !!( horrible_cast<float,int>(value).second & 0x80000000 ) );
}

inline float deg2rad	( float const value )
{
	return			( ( value/180.f ) * math::pi );
}

inline float rad2deg	( float const value )
{
	return			( ( value/math::pi ) * 180.f );
}

template < typename T >
inline T clamp_r		( T const value, T const min, T const max )
{
	ASSERT			( min <= max );

	if ( value <= min )
		return		( min );

	if ( value <= max )
		return		( value );

	return			( max );
}

template < typename T >
inline void clamp		( T& value_and_result, T const min, T const max )
{
	ASSERT			( min <= max );
	value_and_result= clamp_r( value_and_result, min, max );
}

inline float angle_normalize_always	( float const angle )
{
	float			division = angle/math::pi_x2;
	int				round = ( division > 0 ) ? floor( division ) : ceil( division );
	float			fraction = division - round;
	if ( fraction < 0 )
		fraction	+=	1.f;

	return			( fraction*math::pi_x2 );
}

inline float angle_normalize		( float const angle )
{
	if ( ( angle >= 0 ) && ( angle <= math::pi_x2 ))
		return		( angle );

	return			( angle_normalize_always( angle ) );
}

inline float angle_normalize_signed	( float const angle )
{
	if ( ( angle >= -math::pi ) && ( angle <= math::pi ) )
		return		( angle );

	float result	= angle_normalize_always( angle );
	if ( result > math::pi )
		result		-= math::pi_x2;

	return			( result );
}

inline float angle_rotation_dist_hemi_flip( float angle_dist )
 {
	if( angle_dist > 0.f )
		return angle_dist - pi;
	else
		return angle_dist + pi;
}

template <class Ordinal>
Ordinal   align_down (Ordinal value, Ordinal align_on)
{
	return value - (value % align_on);
}

template <class Ordinal>
Ordinal   align_up (Ordinal value, Ordinal align_on)
{
	Ordinal mod	=	(value % align_on);
	return !mod ?	value : value + align_on-mod ;
}

} // namespace math
} // namespace xray

#if defined(_MSC_VER)
#	pragma deprecated(cosf,sinf,tanf,acosf,asinf,atanf,atan2f,sqrtf,logf)
#endif // #if defined(_MSC_VER)

#endif // #ifndef XRAY_MATH_FUNCTIONS_INLINE_H_INCLUDED