////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_HALF_H_INCLUDED
#define MATH_HALF_H_INCLUDED

namespace xray {
namespace math {

struct half_pod
{
public:
	typedef u16			type;
public:
	operator float  ( ) const;
public:
	type				data;
};

class half : public half_pod
{ 
public:
	half			( );
	half			( half_pod val );
	half			( type val );
	half			( float x );
}; // class half

class half3;


class half3_pod
{
public:
	inline	half	get_x			( ) const						{ return x; }
	inline	half	get_y			( ) const						{ return y; }
	inline	half	get_z			( ) const						{ return z; }
	
	inline	void	set_x			( half_pod _x )					{ x = _x; }
	inline	void	set_y			( half_pod _y )					{ y = _y; }
	inline	void	set_z			( half_pod _z )					{ z = _z; }

	friend	half3	operator *		( half3_pod const a, half3_pod const b);
	friend	half3	operator +		( half3_pod const a, half3_pod const b);
	friend	half3	operator *		( half3_pod const a, float s );

					operator float3	( ) const;
					operator half3	( ) const;
	
			half	length			( ) const;
protected:
	half_pod	x;
	half_pod	y;
	half_pod	z;
};


class half3 : public half3_pod
{
public:
			half3					( );
			half3					( half3 const& copy );
			half3					( float3 const& copy );
			half3					( half _x, half _y, half _z );
	
	friend	half3	operator *		(half3 const a, half3 const b)	{return half3(a.x*b.x, a.y*b.y, a.z*b.z);}
	friend	half3	operator +		(half3 const a, half3 const b)  {return half3(a.x+b.x, a.y+b.y, a.z+b.z);}
	friend	half3	operator *		(half3 const a, float s)		{return half3(a.x*s, a.y*s, a.z*s);}
	
					operator float3	( ) const;
			void	operator +=		( half3 const& other);
			void	operator *=		( half3 const& other);
	
			half	length( )		const;

}; // class half3

class half4;

class half4_pod
{
public:
	friend	half4	operator *		(half4_pod const a, half4_pod const b);
	friend	half4	operator +		(half4_pod const a, half4_pod const b);
	friend	half4	operator *		(half4_pod const a, float s);

	inline	half	get_x			( ) const						{ return x; }
	inline	half	get_y			( ) const						{ return y; }
	inline	half	get_z			( ) const						{ return z; }
	inline	half	get_w			( ) const						{ return w; }
	
	inline	void	set_x			( half_pod _x )					{ x = _x; }
	inline	void	set_y			( half_pod _y )					{ y = _y; }
	inline	void	set_z			( half_pod _z )					{ z = _z; }
	inline	void	set_w			( half_pod _w )					{ w = _w; }
protected:
	half_pod	x;
	half_pod	y;
	half_pod	z;
	half_pod	w;
};

class half4 : public half4_pod
{
public:
					half4			( );
					half4			( half4 const& copy );
					half4			( float4 const& copy );
					half4			( half _x, half _y, half _z, half _w );

	
	friend	half4	operator *		(half4 const a, half4 const b)	{return half4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);}
	friend	half4	operator +		(half4 const a, half4 const b)  {return half4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);}
	friend	half4	operator *		(half4 const a, float s)		{return half4(a.x*s, a.y*s, a.z*s, a.w*s);}
	
					operator float4 ( ) const;
			void	operator +=		( half4 const& other);
			void	operator *=		( half4 const& other);
	
			half	length			( ) const ;
			half3	xyz( )			const;
}; // class half4

} // namespace math
} // namespace xray

#include <xray/math_half_inline.h>

#endif // #ifndef MATH_HALF_H_INCLUDED