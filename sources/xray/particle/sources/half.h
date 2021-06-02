////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HALF_H_INCLUDED
#define HALF_H_INCLUDED

namespace xray {
namespace particle {

class half { 
public:
	half			( );
	half			( float x );
	operator float  ( ) const;
	u16				data;
}; // class half

class half3
{
public:
	half3 ( );
	half3 ( half3 const& copy );
	half3 ( xray::math::float3 const& copy );
	half3 ( half _x, half _y, half _z );
	
	friend half3 operator * (half3 const a, half3 const b)	{return half3(a.x*b.x, a.y*b.y, a.z*b.z);}
	friend half3 operator + (half3 const a, half3 const b)  {return half3(a.x+b.x, a.y+b.y, a.z+b.z);}
	friend half3 operator * (half3 const a, float s)		{return half3(a.x*s, a.y*s, a.z*s);}
	
	operator xray::math::float3 ( ) const;
	void operator += ( half3 const& other);
	void operator *= ( half3 const& other);
	
	half length() const;
	
protected:
	half			x;
	half			y;
	half			z;
}; // class half3


class half4
{
public:
	half4 ( );
	half4 ( half4 const& copy );
	half4 ( xray::math::float4 const& copy );
	half4 ( half _x, half _y, half _z, half _w );
	
	friend half4 operator * (half4 const a, half4 const b)	{return half4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);}
	friend half4 operator + (half4 const a, half4 const b)  {return half4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);}
	friend half4 operator * (half4 const a, float s)		{return half4(a.x*s, a.y*s, a.z*s, a.w*s);}
	
	operator xray::math::float4 ( ) const;
	void operator += ( half4 const& other);
	void operator *= ( half4 const& other);
	
	half length() const ;
	half3 xyz() const ;
	
protected:
	half			x;
	half			y;
	half			z;
	half			w;
}; // class half4

} // namespace particle
} // namespace xray

#include "half_inline.h"

#endif // #ifndef HALF_H_INCLUDED