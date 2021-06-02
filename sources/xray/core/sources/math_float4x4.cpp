////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::math::float4x4;
using xray::math::float3;

static float4x4 invert_impl								( float4x4 const& other, float const determinant )
{
	//bool const is_similar_determinant	= xray::math::is_similar( determinant, other.determinant( ), xray::math::epsilon_5 );
	//if ( !is_similar_determinant )
	//{
	//	float const d0 = other.determinant( );
	//	float const d1 = other.determinant( );
	//	float const d2 = other.determinant( );
	//	float const d3 = other.determinant( );
	//	LOG_ERROR("dets: %f\n %f\n %f\n %f\n %f", determinant, d0, d1, d2, d3);
	//	LOG_ERROR("i: %f %f %f %f", other.e00, other.e01, other.e02, other.e03);
	//	LOG_ERROR("j: %f %f %f %f", other.e10, other.e11, other.e12, other.e13);
	//	LOG_ERROR("k: %f %f %f %f", other.e20, other.e21, other.e22, other.e23);
	//	LOG_ERROR("c: %f %f %f %f", other.e30, other.e31, other.e32, other.e33);
	//	R_ASSERT		( 0 );
	//}
	R_ASSERT		( xray::math::is_relatively_similar( determinant, other.determinant4x3( ), xray::math::epsilon_5 ) );


	float const	inverted_determinant = 1.f / determinant;

	float4x4	result;
	result.e00 	=  inverted_determinant * ( other.e11*other.e22 - other.e12*other.e21 );
	result.e01 	= -inverted_determinant * ( other.e01*other.e22 - other.e02*other.e21 );
	result.e02 	=  inverted_determinant * ( other.e01*other.e12 - other.e02*other.e11 );
	result.e03 	=  0.f;
	
	result.e10 	= -inverted_determinant * ( other.e10*other.e22 - other.e12*other.e20 );
	result.e11 	=  inverted_determinant * ( other.e00*other.e22 - other.e02*other.e20 );
	result.e12 	= -inverted_determinant * ( other.e00*other.e12 - other.e02*other.e10 );
	result.e13 	=  0.f;
	
	result.e20 	=  inverted_determinant * ( other.e10*other.e21 - other.e11*other.e20 );
	result.e21 	= -inverted_determinant * ( other.e00*other.e21 - other.e01*other.e20 );
	result.e22 	=  inverted_determinant * ( other.e00*other.e11 - other.e01*other.e10 );
	result.e23 	=  0.f;
	
	result.e30 	= -( other.e30*result.e00 + other.e31*result.e10 + other.e32*result.e20 );
	result.e31 	= -( other.e30*result.e01 + other.e31*result.e11 + other.e32*result.e21 );
	result.e32 	= -( other.e30*result.e02 + other.e31*result.e12 + other.e32*result.e22 );
	result.e33 	=  1.f;

	R_ASSERT	( result.valid( ) );

	return		( result );
}

float4x4 xray::math::invert4x3							( float4x4 const& other )
{
	R_ASSERT	( other.valid( ) );
	return		invert_impl( other, other.determinant4x3() );
}

bool xray::math::try_invert4x4			( float4x4 const& matrix_to_invert, float4x4& out_result )
{
	float const* const m				=	& matrix_to_invert.e00;
	float* const inv					=	&out_result.e00;

	inv[0] 	=   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
					+ m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] 	=  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
					- m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] 	=   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
					+ m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12]	 = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
				- m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];

	float const determinant				=	(m[0] * inv[0]) + (m[1] * inv[4]) + 
											(m[2] * inv[8]) + (m[3] * inv[12]);
	if ( determinant == 0 )
		return								false;

	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
				- m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
				+ m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
				- m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
				+ m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
				+ m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
				- m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
				+ m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
				- m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
				- m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
				+ m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
				- m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
				+ m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	out_result							*=	1.0f / determinant;
	return								true;
}

float4x4 xray::math::invert4x4			( float4x4 const& matrix_to_invert )
{
	float4x4 out_result;
	bool const inverted					=	try_invert4x4(matrix_to_invert, out_result);
	R_ASSERT								(inverted);
	return									out_result;
}

bool float4x4::try_invert								( float4x4 const& other )
{
	float const determinant = other.determinant4x3();
	if ( is_zero( determinant, epsilon_7) )
	{
		float const epsilon = math::epsilon_7;
		if (
				math::is_relatively_zero( other.e11*other.e22 - other.e12*other.e21, determinant, epsilon ) ||
				math::is_relatively_zero( other.e01*other.e22 - other.e02*other.e21, determinant, epsilon ) ||
				math::is_relatively_zero( other.e01*other.e12 - other.e02*other.e11, determinant, epsilon ) ||
				math::is_relatively_zero( other.e10*other.e22 - other.e12*other.e20, determinant, epsilon ) ||
				math::is_relatively_zero( other.e00*other.e22 - other.e02*other.e20, determinant, epsilon ) ||
				math::is_relatively_zero( other.e00*other.e12 - other.e02*other.e10, determinant, epsilon ) ||
				math::is_relatively_zero( other.e10*other.e21 - other.e11*other.e20, determinant, epsilon ) ||
				math::is_relatively_zero( other.e00*other.e21 - other.e01*other.e20, determinant, epsilon ) ||
				math::is_relatively_zero( other.e00*other.e11 - other.e01*other.e10, determinant, epsilon )
			)
			return false;
	}
	*this		= invert_impl ( other, determinant );
	return		( true );
}

float3 float4x4::get_angles				( xray::math::axis_rotation_order const order ) const // zxy indeed
{
	ASSERT_U( order == rotation_zxy);
	ASSERT	( !math::is_zero( i.xyz().length( ) ), "vector 'i' is 0 !" );
	ASSERT	( !math::is_zero( j.xyz().length( ) ), "vector 'j' is 0 !" );
	ASSERT	( !math::is_zero( k.xyz().length( ) ), "vector 'k' is 0 !" );

	float	inv_scale_x	= 1/sqrt( i.xyz().squared_length( ) );
	float	inv_scale_y	= 1/sqrt( j.xyz().squared_length( ) );	
	float	inv_scale_z	= 1/sqrt( k.xyz().squared_length( ) );

	float const	horde = sqrt( sqr( j.y*inv_scale_y ) + sqr( i.y*inv_scale_x ) );//z->x ->y
    if ( horde > 16.f*type_epsilon<float>() )
		return	(
			float3(
				atan2( -k.y*inv_scale_z, horde ),
				atan2( k.x*inv_scale_z, k.z*inv_scale_z ),//y 3 
				atan2( i.y*inv_scale_x, j.y*inv_scale_y ) //z 1
			)
		);

	return		(
		float3(
			atan2( -k.y*inv_scale_z, horde ),
			atan2( -i.z*inv_scale_x, i.x*inv_scale_x ),
			0.f
		)
	);
}



float3 float4x4::get_angles_xyz				( ) const
{
	ASSERT( !math::is_zero( i.xyz().length( ) ), "vector 'i' is 0 !" );
	ASSERT( !math::is_zero( j.xyz().length( ) ), "vector 'j' is 0 !" );
	ASSERT( !math::is_zero( k.xyz().length( ) ), "vector 'k' is 0 !" );

	float	inv_scale_x	= 1/sqrt( i.xyz().squared_length( ) );
	float	inv_scale_y	= 1/sqrt( j.xyz().squared_length( ) );	
	float	inv_scale_z	= 1/sqrt( k.xyz().squared_length( ) );

	float const	horde = sqrt( sqr( j.z*inv_scale_y ) + sqr( k.z*inv_scale_z ) );
	return
		float3 (
			atan2( -j.z*inv_scale_y, k.z*inv_scale_z ), //x 1
			atan2( i.z*inv_scale_x, horde ),			//y 2
			atan2( -i.y, i.x )							//z 3
		);
}

float4x4 xray::math::mul4x3						( float4x4 const& left, float4x4 const& right )
{
	float4x4	result;

	result.e00 	= left.e00*right.e00 + left.e01*right.e10 + left.e02*right.e20;
	result.e01 	= left.e00*right.e01 + left.e01*right.e11 + left.e02*right.e21;
	result.e02 	= left.e00*right.e02 + left.e01*right.e12 + left.e02*right.e22;
	result.e03 	= left.e00*right.e03 + left.e01*right.e13 + left.e02*right.e23;

	result.e10 	= left.e10*right.e00 + left.e11*right.e10 + left.e12*right.e20;
	result.e11 	= left.e10*right.e01 + left.e11*right.e11 + left.e12*right.e21;
	result.e12 	= left.e10*right.e02 + left.e11*right.e12 + left.e12*right.e22;
	result.e13	= left.e10*right.e03 + left.e11*right.e13 + left.e12*right.e23;

	result.e20 	= left.e20*right.e00 + left.e21*right.e10 + left.e22*right.e20;
	result.e21 	= left.e20*right.e01 + left.e21*right.e11 + left.e22*right.e21;
	result.e22 	= left.e20*right.e02 + left.e21*right.e12 + left.e22*right.e22;
	result.e23 	= left.e30*right.e03 + left.e21*right.e13 + left.e22*right.e23;

	result.e30 	= left.e30*right.e00 + left.e31*right.e10 + left.e32*right.e20 + right.e30;
	result.e31 	= left.e30*right.e01 + left.e31*right.e11 + left.e32*right.e21 + right.e31;
	result.e32 	= left.e30*right.e02 + left.e31*right.e12 + left.e32*right.e22 + right.e32;
	result.e33 	= left.e30*right.e03 + left.e31*right.e13 + left.e32*right.e23 + right.e33;

	return		( result );
}

float4x4 xray::math::mul4x4						( float4x4 const& left, float4x4 const& right )
{
	float4x4	result;

	result.e00 	= left.e00*right.e00 + left.e01*right.e10 + left.e02*right.e20 + left.e03*right.e30;
	result.e01 	= left.e00*right.e01 + left.e01*right.e11 + left.e02*right.e21 + left.e03*right.e31;
	result.e02 	= left.e00*right.e02 + left.e01*right.e12 + left.e02*right.e22 + left.e03*right.e32;
	result.e03 	= left.e00*right.e03 + left.e01*right.e13 + left.e02*right.e23 + left.e03*right.e33;
																							 
	result.e10 	= left.e10*right.e00 + left.e11*right.e10 + left.e12*right.e20 + left.e13*right.e30;
	result.e11 	= left.e10*right.e01 + left.e11*right.e11 + left.e12*right.e21 + left.e13*right.e31;
	result.e12 	= left.e10*right.e02 + left.e11*right.e12 + left.e12*right.e22 + left.e13*right.e32;
	result.e13	= left.e10*right.e03 + left.e11*right.e13 + left.e12*right.e23 + left.e13*right.e33;
																							 
	result.e20 	= left.e20*right.e00 + left.e21*right.e10 + left.e22*right.e20 + left.e23*right.e30;
	result.e21 	= left.e20*right.e01 + left.e21*right.e11 + left.e22*right.e21 + left.e23*right.e31;
	result.e22 	= left.e20*right.e02 + left.e21*right.e12 + left.e22*right.e22 + left.e23*right.e32;
	result.e23 	= left.e30*right.e03 + left.e21*right.e13 + left.e22*right.e23 + left.e23*right.e33;
																							 
	result.e30 	= left.e30*right.e00 + left.e31*right.e10 + left.e32*right.e20 + left.e33*right.e30;
	result.e31 	= left.e30*right.e01 + left.e31*right.e11 + left.e32*right.e21 + left.e33*right.e31;
	result.e32 	= left.e30*right.e02 + left.e31*right.e12 + left.e32*right.e22 + left.e33*right.e32;
	result.e33 	= left.e30*right.e03 + left.e31*right.e13 + left.e32*right.e23 + left.e33*right.e33;

	return		( result );
}

float4x4 xray::math::create_perspective_projection	( float4x4::type vertical_field_of_view, float4x4::type aspect_ratio, float4x4::type near_plane, float4x4::type far_plane )
{
	ASSERT_CMP	( aspect_ratio, >=, epsilon_5 );

	float const tan_alpha_div_2	= tan( vertical_field_of_view/2.f );
	ASSERT		( abs( tan_alpha_div_2 ) > epsilon_5 );

	float const cotan_alpha_div_2	= 1.f/tan_alpha_div_2;

	ASSERT_CMP	( far_plane, >, near_plane + epsilon_5 );
	float const	A = far_plane/( far_plane - near_plane );
	
	float4x4	result;

	result.i	= float4 ( cotan_alpha_div_2/aspect_ratio,	0.f,				0.f,			0.f );
	result.j	= float4 ( 0.f,								cotan_alpha_div_2,	0.f,			0.f );
	result.k	= float4 ( 0.f,								0.f,				A,				1.f );
	result.c	= float4 ( 0.f,								0.f,				-near_plane*A,	0.f );

	return		( result ); 
}

float4x4 xray::math::create_orthographic_projection	( float4x4::type w, float4x4::type h, float4x4::type near_plane, float4x4::type far_plane )
{
	float const	A = 1.f/(far_plane - near_plane);

	float4x4	result;

	result.i	= float4( 2.f/w,	0.f,	0.f,			0.f );
	result.j	= float4( 0.f,		2.f/h,	0.f,			0.f );
	result.k	= float4( 0.f,		0.f,	A,				0.f );
	result.c	= float4( 0.f,		0.f,	-near_plane*A,	1.f );

	return		( result );
}

float4x4 xray::math::create_camera_direction		( float3 const& from, float3 const& view, float3 const& world_up )
{
	float3 const up = ( view*-(world_up | view ) + world_up ).normalize( );
	float3 const right = up ^ view;

	float4x4	result;

	result.i	= float4(	right.x,			up.x,			view.x,			0.f );
	result.j	= float4(	right.y,			up.y,			view.y,			0.f );
	result.k	= float4(	right.z,			up.z,			view.z,			0.f );
	result.c	= float4(	-(from | right),	-(from | up),	-(from | view), 1.f );

	return		( result );
}

float4x4 xray::math::create_camera_at				( float3 const& from, float3 const& at, float3 const& up )
{
	return		( create_camera_direction ( from, ( at - from ).normalize( ), up ) );
}

float4x4 xray::math::create_rotation				( float3 const& axis, float const angle )
{
	float4x4	result;

	sine_cosine	temp( angle );
	float const inv_cosine = 1.f - temp.cosine;
	float const sqr_x = sqr( axis.x );
	float const sqr_y = sqr( axis.y );
	float const sqr_z = sqr( axis.z );
	float const xy_inv_cosine = axis.x*axis.y*inv_cosine;
	float const yz_inv_cosine = axis.y*axis.z*inv_cosine;
	float const xz_inv_cosine = axis.x*axis.z*inv_cosine;
	float const xsine = axis.x*temp.sine;
	float const ysine = axis.y*temp.sine;
	float const zsine = axis.z*temp.sine;

	result.i.set( sqr_x + (1.f - sqr_x)*temp.cosine,	xy_inv_cosine - zsine,				xz_inv_cosine + ysine,				0.f );
	result.j.set( xy_inv_cosine + zsine,				sqr_y + (1.f - sqr_y)*temp.cosine,	yz_inv_cosine - xsine,				0.f );
	result.k.set( xz_inv_cosine - ysine,				yz_inv_cosine + xsine,				sqr_z + (1.f - sqr_z)*temp.cosine,	0.f );
	result.c.set( 0.f,									0.f,								0.f,								1.f );

	return		result;
}

bool xray::math::try_solve_linear_equations_system	(
		float3 const& first,
		float3 const& second,
		float3 const& third,
		float3 const& b,
		float3& result
	)
{
	float4x4 m	= float4x4().identity();
	m.i.xyz()	= first;
	m.j.xyz()	= second;
	m.k.xyz()	= third;
	
	float4x4 inverted;
	bool success = inverted.try_invert( m );
	if ( !success )
		return	false;
	
	result		= float3( inverted.i.xyz() | b, inverted.j.xyz() | b, inverted.k.xyz() | b );
	return		true;
}

bool xray::math::try_solve_linear_equations_system	(
		float4 const& first,
		float4 const& second,
		float4 const& third,
		float4 const& fourth,
		float4 const& b,
		float4& result
	)
{
	float4x4 m	= float4x4().identity();
	m.i			= first;
	m.j			= second;
	m.k			= third;
	m.c			= fourth;
	
	float4x4 inverted;
	bool success = try_invert4x4( inverted, m );
	if ( !success )
		return	false;
	
	result		= float4( inverted.i | b, inverted.j | b, inverted.k | b, inverted.c | b );
	return		true;
}