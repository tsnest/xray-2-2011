////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HALF_INLINE_H_INCLUDED
#define HALF_INLINE_H_INCLUDED

namespace xray {
namespace particle {

inline half::half() : 
	data(0)
{
}

inline half::half( float x)
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

inline half::operator float( ) const
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


inline half3::half3			( )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

inline half3::half3			( half3 const& copy )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

inline half3::half3			( xray::math::float3 const& copy )
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

inline half3::half3			( half _x, half _y, half _z )
{
	x = _x;
	y = _y;
	z = _z;
}

inline half3::operator xray::math::float3 ( ) const
{
	return xray::math::float3(float(x), float(y), float(z));
}

inline void half3::operator += ( half3 const& other)
{
	*this = *this + other;
}

inline void half3::operator *= ( half3 const& other)
{
	*this = *this * other;
}

inline half half3::length() const
{
	return xray::math::sqrt( x*x + y*y + z*z );
}

inline half4::half4	( )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
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
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
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
	return xray::math::float4(float(x), float(y), float(z), float(w));
}

inline void half4::operator += ( half4 const& other)
{
	*this = *this + other;
}

inline void half4::operator *= ( half4 const& other)
{
	*this = *this * other;
}

inline half half4::length() const 
{
	return xray::math::sqrt( x*x + y*y + z*z  + w*w );
}

inline half3 half4::xyz() const 
{
	return half3(x,y,z);
}

} // namespace particle
} // namespace xray

#endif // #ifndef HALF_INLINE_H_INCLUDED