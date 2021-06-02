////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_HALF_INLINE_H_INCLUDED
#define MATH_HALF_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline half_pod::operator float( ) const
{
	union 
	{ 
		unsigned int s; 
		float result; 
	};

	s = (data & 0x8000) << 16; 
	unsigned int e = (data >> 10) & 0x1F; 
	unsigned int m = data & 0x03FF;

	if (e == 0) 
	{ 
		// +/- 0 
		if (m == 0) return result;

		// Denorm 
		while ((m & 0x0400) == 0) 
		{ 
			m += m; 
			e--; 
		} 
		e++; 
		m &= ~0x0400; 
	} 
	else if (e == 31) 
	{ 
		// INF / NAN 
		s |= 0x7F800000 | (m << 13); 
		return result; 
	}

	s |= ((e + 112) << 23) | (m << 13);

	return result; 	
}

inline half::half( )
{
	data = 0;
}

inline half::half( half_pod val )
{
	data = val.data;
}

inline half::half( type val )
{
	data = val;
}

inline half::half( float x )
{
	union 
	{ 
		float floatI; 
		unsigned int i; 
	}; 
	floatI = x;

	int e = ((i >> 23) & 0xFF) - 112; 
	int m =  i & 0x007FFFFF;

	data = (i >> 16) & 0x8000; 
	if (e <= 0) 
	{ 
		// Denorm 
		m = ((m | 0x00800000) >> (1 - e)) + 0x1000; 
		data |= (m >> 13); 
	} 
	else if (e == 143) 
	{ 
		data |= 0x7C00; 
		if (m != 0) 
		{ 
			// NAN 
			m >>= 13; 
			data |= m | (m == 0); 
		} 
	} 
	else 
	{ 
		m += 0x1000; 
		if (m & 0x00800000) 
		{ 
			// Mantissa overflow 
			m = 0; 
			e++; 
		} 
		if (e >= 31) 
		{ 
			// Exponent overflow 
			data |= 0x7C00; 
		} 
		else 
		{ 
			data |= (e << 10) | (m >> 13); 
		} 
	} 	
}



inline half3_pod::operator half3	( ) const
{
	return half3( x, y, z );
}


inline half3 operator * ( half3_pod const a, half3_pod const b)
{
	return half3( a.x * b.x, a.y * b.y, a.z * b.z );
}

inline half3 operator + ( half3_pod const a, half3_pod const b)
{
	return half3 ( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline half3 operator * ( half3_pod const a, float s )
{
	return half3 ( a.x * s, a.y * s, a.z * s );
}

inline half3_pod::operator float3 ( ) const
{
	return float3( x, y, z );
}

inline half	half3_pod::length		( ) const
{
	return sqrt( x*x + y*y + z*z );
}

inline half3::half3			( )
{
	half3_pod::x = half( 0.0f );
	half3_pod::y = half( 0.0f );
	half3_pod::z = half( 0.0f );
}

inline half3::half3			( half3 const& copy )
{
	half3_pod::x = half( copy.x );
	half3_pod::y = half( copy.y );
	half3_pod::z = half( copy.z );
}

inline half3::half3			( xray::math::float3 const& copy )
{
	half3_pod::x = half( copy.x );
	half3_pod::y = half( copy.y );
	half3_pod::z = half( copy.z );
}

inline half3::half3			( half _x, half _y, half _z )
{
	half3_pod::x = _x;
	half3_pod::y = _y;
	half3_pod::z = _z;
}

inline half3::operator float3	( ) const
{
	return float3( x, y, z );
}


inline void half3::operator += ( half3 const& other)
{
	*this = *this + other;
}

inline void half3::operator *= ( half3 const& other)
{
	*this = *this * other;
}

inline half half3::length( ) const
{
	return sqrt( x*x + y*y + z*z );
}

inline half4 operator * (half4_pod const a, half4_pod const b)
{
	return half4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

inline half4 operator + (half4_pod const a, half4_pod const b)
{
	return half4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

inline half4 operator * (half4_pod const a, float s)
{
	return half4(a.x*s, a.y*s, a.z*s, a.w*s);
}

inline half4::half4	( )
{
	x = half ( 0.0f );
	y = half ( 0.0f );
	z = half ( 0.0f );
	w = half ( 0.0f );
}

inline half4::half4 ( half4 const& copy )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
}

inline half4::half4 ( xray::math::float4 const& copy )
{
	x = half ( copy.x );
	y = half ( copy.y );
	z = half ( copy.z );
	w = half ( copy.w );
}

inline half4::half4 ( half _x, half _y, half _z, half _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline half4::operator xray::math::float4 ( ) const
{
	return xray::math::float4( x, y, z, w );
}

inline void half4::operator += ( half4 const& other)
{
	*this = *this + other;
}

inline void half4::operator *= ( half4 const& other)
{
	*this = *this * other;
}

inline half half4::length( ) const 
{
	return xray::math::sqrt( x*x + y*y + z*z  + w*w );
}

inline half3 half4::xyz( ) const 
{
	return half3( x, y, z );
}

} // namespace math
} // namespace xray

#endif // #ifndef MATH_HALF_INLINE_H_INCLUDED