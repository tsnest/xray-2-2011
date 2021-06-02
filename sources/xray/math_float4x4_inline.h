////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT4X4_INLINE_H_INCLUDED
#define XRAY_MATH_FLOAT4X4_INLINE_H_INCLUDED
#include	"math_quaternion.h"
namespace xray {
namespace math {

inline float4x4::float4x4					( )
#ifdef DEBUG
:	i			( float4() ),
	j			( float4() ),
	k			( float4() ),
	c			( float4() )
#endif // #ifdef DEBUG
{
}

inline float4x4::float4x4					( float4 const& other_i, float4 const& other_j, float4 const& other_k, float4 const& other_c ) :
	i			( other_i ),
	j			( other_j ),
	k			( other_k ),
	c			( other_c )
{
}

inline float4x4& float4x4::identity			( )
{
	i			= float4( 1.f, 0.f, 0.f, 0.f );
	j			= float4( 0.f, 1.f, 0.f, 0.f );
	k			= float4( 0.f, 0.f, 1.f, 0.f );
	c			= float4( 0.f, 0.f, 0.f, 1.f );

	return		( *this );
}

inline float4x4& float4x4::operator *=		( float4x4::type value )
{
	i			*= value;
	j			*= value;
	k			*= value;
	c			*= value;

	return		( *this );
}

inline float4x4& float4x4::operator /=		( float4x4::type value )
{
	ASSERT		( !is_relatively_zero( e00, value ) );
	ASSERT		( !is_relatively_zero( e01, value ) );
	ASSERT		( !is_relatively_zero( e02, value ) );
	ASSERT		( !is_relatively_zero( e03, value ) );

	ASSERT		( !is_relatively_zero( e10, value ) );
	ASSERT		( !is_relatively_zero( e11, value ) );
	ASSERT		( !is_relatively_zero( e12, value ) );
	ASSERT		( !is_relatively_zero( e13, value ) );

	ASSERT		( !is_relatively_zero( e20, value ) );
	ASSERT		( !is_relatively_zero( e21, value ) );
	ASSERT		( !is_relatively_zero( e22, value ) );
	ASSERT		( !is_relatively_zero( e23, value ) );

	ASSERT		( !is_relatively_zero( e30, value ) );
	ASSERT		( !is_relatively_zero( e31, value ) );
	ASSERT		( !is_relatively_zero( e32, value ) );
	ASSERT		( !is_relatively_zero( e33, value ) );

	return		( *this *= (1.f / value) );
}

inline float3 float4x4::transform_position	( float3 const& vector ) const
{
	float3		result;

	result.x	= vector.x*e00 + vector.y*e10 +  vector.z*e20 + 1.f*e30;
	result.y	= vector.x*e01 + vector.y*e11 +  vector.z*e21 + 1.f*e31;
	result.z	= vector.x*e02 + vector.y*e12 +  vector.z*e22 + 1.f*e32;

	return		( result );
}

inline float3 float4x4::transform_direction	( float3 const& direction ) const
{
	float3		result;

	result.x	= direction.x*e00 + direction.y*e10 +  direction.z*e20;
	result.y	= direction.x*e01 + direction.y*e11 +  direction.z*e21;
	result.z	= direction.x*e02 + direction.y*e12 +  direction.z*e22;

	return		( result );
}

inline float3 float4x4::transform			( float3 const& vector ) const
{
	float4 const temp = transform( float4( vector, 1.f) );
	return		( float3( temp.x, temp.y, temp.z ) / temp.w );
}

inline float4 float4x4::transform			( float4 const& vector ) const
{
	float4		result;

	result.x	= vector.x*e00 + vector.y*e10 + vector.z*e20 + vector.w*e30;
	result.y	= vector.x*e01 + vector.y*e11 + vector.z*e21 + vector.w*e31;
	result.z	= vector.x*e02 + vector.y*e12 + vector.z*e22 + vector.w*e32;
	result.w	= vector.x*e03 + vector.y*e13 + vector.z*e23 + vector.w*e33;

	return		( result );
}

inline float float4x4::determinant4x3		( ) const
{
	return		(
		e00 * ( e11*e22 - e12*e21 ) -
		e01 * ( e10*e22 - e12*e20 ) +
		e02 * ( e10*e21 - e11*e20 )
	);
}

#ifndef MASTER_GOLD

inline bool float4x4::valid					( ) const
{
	return		( i.valid( ) && j.valid( ) && k.valid( ) && c.valid( ) );
}

#endif

inline float3 float4x4::get_scale			( ) const
{
	return		float3( i.xyz().length(), j.xyz().length(), k.xyz().length() );
}

inline void float4x4::set_scale				( float3 const& scale )
{
	i.xyz().set_length	( scale.x );
	j.xyz().set_length	( scale.y );
	k.xyz().set_length	( scale.z );
}

inline float4x4	operator *					( float4x4 const& left, float4x4 const& right )
{
	R_ASSERT	( left.i.w  == 0.f );
	R_ASSERT	( left.j.w  == 0.f );
	R_ASSERT	( left.k.w  == 0.f );
	R_ASSERT	( right.i.w == 0.f );
	R_ASSERT	( right.j.w == 0.f );
	R_ASSERT	( right.k.w == 0.f );

	return		( mul4x3( left, right ) );
}

inline float4x4 operator *					( float4x4 const& left, float4x4::type right )
{
	float4x4	result;

	result.i	= left.i * right;
	result.j	= left.j * right;
	result.k	= left.k * right;
	result.c	= left.c * right;

	return		( result );
}

inline float4x4 operator *					( float4x4::type left, float4x4 const& right )
{
	return		( right * left);
}

inline float4x4 operator /					( float4x4 const& left, float4x4::type right )
{
	ASSERT		( !is_relatively_zero( left.e00, right ) );
	ASSERT		( !is_relatively_zero( left.e01, right ) );
	ASSERT		( !is_relatively_zero( left.e02, right ) );
	ASSERT		( !is_relatively_zero( left.e03, right ) );

	ASSERT		( !is_relatively_zero( left.e10, right ) );
	ASSERT		( !is_relatively_zero( left.e11, right ) );
	ASSERT		( !is_relatively_zero( left.e12, right ) );
	ASSERT		( !is_relatively_zero( left.e13, right ) );

	ASSERT		( !is_relatively_zero( left.e20, right ) );
	ASSERT		( !is_relatively_zero( left.e21, right ) );
	ASSERT		( !is_relatively_zero( left.e22, right ) );
	ASSERT		( !is_relatively_zero( left.e23, right ) );

	ASSERT		( !is_relatively_zero( left.e30, right ) );
	ASSERT		( !is_relatively_zero( left.e31, right ) );
	ASSERT		( !is_relatively_zero( left.e32, right ) );
	ASSERT		( !is_relatively_zero( left.e33, right ) );

	return		( left * (1.f / right) );
}

inline float3 operator *					( float3 const& left, float4x4 const& right )
{
	return		( right.transform_position( left ) );
}

inline float4 operator *					( float4 const& left, float4x4 const& right )
{
	return		( right.transform( left ) );
}

inline float4x4 transpose					( float4x4 const& other )
{
	float4x4	result;

	result.i	= float4( other.e00, other.e10, other.e20, other.e30);
	result.j	= float4( other.e01, other.e11, other.e21, other.e31);
	result.k	= float4( other.e02, other.e12, other.e22, other.e32);
	result.c	= float4( other.e03, other.e13, other.e23, other.e33);

	return		( result );
}

inline float4x4 create_translation			( float3 const& position )
{
	float4x4	result;

	result.i	= float4( 1.f,			0.f,		0.f,		0.f );
	result.j	= float4( 0.f,			1.f,		0.f,		0.f );
	result.k	= float4( 0.f,			0.f,		1.f,		0.f );
	result.c	= float4( position.x,	position.y,	position.z,	1.f );

	return		( result );
}

inline float4x4 create_rotation ( float3 const& angles, xray::math::axis_rotation_order const order )
{
	ASSERT_U	( order == rotation_zxy);
	float4x4	result;
	sine_cosine	x(-angles.x), y(-angles.y), z(-angles.z);

	float		ysXzs = y.sine*z.sine;
	float		ysXzc = y.sine*z.cosine;
	float		ycXzs = y.cosine*z.sine;
	float		ycXzc = y.cosine*z.cosine;

	result.i	= float4(	ycXzc - x.sine*ysXzs,	-x.cosine*z.sine,	x.sine*ycXzs + ysXzc,	0.f);
	result.j	= float4(	x.sine*ysXzc + ycXzs,	x.cosine*z.cosine,	ysXzs - x.sine*ycXzc,	0.f);
	result.k	= float4(	-x.cosine*y.sine,		x.sine,				x.cosine*y.cosine,		0.f);
	result.c	= float4(	0.f,					0.f,				0.f,					1.f);

	return		( result );
}

inline float4x4 create_rotation				( float3 const& angles )
{
	float4x4	result;

	sine_cosine	x(angles.x), y(angles.y), z(angles.z);

	float		xsXyc = x.sine	* y.cosine;
	float		xcXyc = x.cosine*y.cosine;
	
	float		xcXzs = x.cosine*z.sine;
	float		xcXzc = x.cosine*z.cosine;
	
	float		xsXzs = x.sine*z.sine;
	float		xsXzc = x.sine*z.cosine;

	float		ysXzs = y.sine*z.sine;
	float		ysXzc = y.sine*z.cosine;

	float		ycXzs = y.cosine*z.sine;
	float		ycXzc = y.cosine*z.cosine;


	result.i	= float4(	ycXzc					 , -ycXzs					,	y.sine, 0.f );
	result.j	= float4(	y.sine	  * xsXzc + xcXzs, -x.sine  * ysXzs + xcXzc ,	-xsXyc, 0.f );
	result.k	= float4(	-x.cosine * ysXzc + xsXzs, x.cosine * ysXzs + xsXzc ,	xcXyc , 0.f );
	result.c	= float4(	0.f						 , 0.f					    ,	0.f	  , 1.f );

	return		( result );
}

inline float4x4 create_rotation_x			( float4x4::type angle )
{
	
	float4x4	result;

	sine_cosine	a( angle );

	result.i	= float4(	1.f,	0.f,		0.f,		0.f);
	result.j	= float4(	0.f,	a.cosine,	-a.sine,	0.f);
	result.k	= float4(	0.f,    a.sine,		a.cosine,	0.f);
	result.c	= float4(	0.f,	0.f,		0.f,		1.f);

	return		( result );
}

inline float4x4 create_rotation_y			( float4x4::type angle )
{

	float4x4	result;

	sine_cosine	a( angle );

	result.i	= float4(	a.cosine,	0.f,	a.sine,		0.f);
	result.j	= float4(	0.f,		1.f,	0.f,		0.f);
	result.k	= float4(	-a.sine,	0.f,	a.cosine,	0.f);
	result.c	= float4(	0.f,		0.f,	0.f,		1.f);

	return		( result ); 
}

inline float4x4 create_rotation_z			( float4x4::type angle )
{

	float4x4	result;

	sine_cosine	a( angle );

	result.i	= float4(	a.cosine,	-a.sine,	0.f,	0.f);
	result.j	= float4(	a.sine,		a.cosine,	0.f,	0.f);
	result.k	= float4(	0.f,		0.f,		1.f,	0.f);
	result.c	= float4(	0.f,		0.f,		0.f,	1.f);

	return		( result ); 
}

inline float4x4 create_rotation				( float3 const& direction, float3 const& normal )
{
	float4x4	result;

	result.i	= float4( (normal ^ direction).normalize( ), 0.f );
	result.j	= float4( normal, 0.f );
	result.k	= float4( direction, 0.f );
	result.c	= float4( 0.f, 0.f, 0.f, 1.f );

	return		( result ); 
}

inline float4x4 create_matrix				( quaternion const& q, float3 const& position )
{
	
	float4x4::type xx = q.x*q.x; float4x4::type yy = q.y*q.y; float4x4::type zz = q.z*q.z;
	float4x4::type xy = q.x*q.y; float4x4::type xz = q.x*q.z; float4x4::type yz = q.y*q.z;
	float4x4::type wx = q.w*q.x; float4x4::type wy = q.w*q.y; float4x4::type wz = q.w*q.z;
	
	float4x4	result; 
	result.e00 = 1 - 2 * ( yy + zz );	result.e01 =     2 * ( xy - wz );	result.e02 =     2 * ( xz + wy );	result.e03 = 0;
	result.e10 =     2 * ( xy + wz );	result.e11 = 1 - 2 * ( xx + zz );	result.e12 =     2 * ( yz - wx );	result.e13 = 0;
	result.e20 =     2 * ( xz - wy );	result.e21 =     2 * ( yz + wx );	result.e22 = 1 - 2 * ( xx + yy );	result.e23 = 0;

	result.e30 = position.x;				result.e31 = position.y;				result.e32 = position.z;						result.e33 = 1.f;
	return		( result ); 
}

inline float4x4 create_rotation			( quaternion const& q )
{
	return create_matrix( q, float3( 0, 0, 0 ) );
}

inline float4x4 create_scale			( float3 const& scale )
{
	float4x4	result;

	memset		( &result, 0, sizeof(result) );

//	result.i	= float4( scale.x,	0.f,		0.f,		0.f );
//	result.j	= float4( 0.f,		scale.y,	0.f,		0.f );
//	result.k	= float4( 0.f,		0.f,		scale.z,	0.f );
//	result.c	= float4( 0.f,		0.f,		0.f,		1.f );
	result.i.x	= scale.x;
	result.j.y	= scale.y;
	result.k.z	= scale.z;
	result.c.w	= 1.f;

	return		( result );
}

inline void get_rotational_and_scale( float4x4 const& source, float4x4 &rotational, float3 &scale )
{
	rotational = source;
	scale.x = source.i.xyz().length();
	ASSERT( scale.x  > 0.f );

	scale.y = source.j.xyz().length();
	ASSERT( scale.y  > 0.f );

	scale.z = source.k.xyz().length();
	ASSERT( scale.z  > 0.f );

	rotational.i.xyz() *= ( 1.f/scale.x );
	rotational.j.xyz() *= ( 1.f/scale.y );
	rotational.k.xyz() *= ( 1.f/scale.z );
	
}

inline float4x4& remove_scale( float4x4 const& source, float4x4& result )
{
	float3 dummy;
	get_rotational_and_scale( source, result, dummy );
	return result;
}

inline void remove_scale( float4x4 &in_out )
{
	remove_scale( in_out, in_out );
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_FLOAT4X4_INLINE_H_INCLUDED