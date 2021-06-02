////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_randoms_generator.h>
#include <xray/core_test_suite.h>
#include <xray/testing_macro.h>

XRAY_DECLARE_LINKAGE_ID( math_tests )

namespace xray {
namespace math {

random32 rnd;

inline float n_pi_d2_random()
{
	//const u32 range = u32(-1)/2;
	const u32 range = 100;
	return math::pi_d2 * ( float( rnd.random( range ) ) - float(  range/2 )  );
}

inline float pi_d2_random()
{
	const u32 range = u32(-1)/2;
	
	return math::pi_d2 * float( rnd.random( range ) ) / float( range );
}

inline float random_angle()
{
	return n_pi_d2_random() + pi_d2_random();
}


inline bool is_identity( const float4x4 &m, float epsilon )
{
	for( u32 i = 0; i < 4; ++i )
		for( u32 j = 0; j < 4; ++j )
			if(  
				( i != j && !math::is_similar( math::abs( m.elements[i][j] ), 0.f, epsilon ) )||
				( i == j && !math::is_similar( math::abs( m.elements[i][j] ), 1.f, epsilon ) )
			) return false;
	return true;
}

inline bool compare( const float4x4 &left, const float4x4 &right, float epsilon  )
{
	float4x4 inv_left;  inv_left.try_invert ( left );
	float4x4 inv_right; inv_right.try_invert( right );

	float4x4 test_identity =  right * inv_left;
	float4x4 test_identity1 = left  * inv_right;

	return is_identity( test_identity, epsilon ) && is_identity( test_identity1, epsilon );
}

static const float matrix_cmp_tolerance = 0.0001f;//0.001f;
static bool matrix_similar( const float4x4 &left, const float4x4 &right )
{
	
	return compare( left, right, matrix_cmp_tolerance ) &&  compare( right, left, matrix_cmp_tolerance );

}

////////////////////////////////test_matrix_to_quaternion////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_matrix_to_quaternion( const float3 &angles )
{
	float4x4 rotation = create_rotation( angles );

	quaternion test_quaternion0 ( rotation );

	float4x4 test_rotation = create_rotation( test_quaternion0 );

	TEST_ASSERT( matrix_similar( rotation, test_rotation ) );

	quaternion  test_quaternion1( angles );

	ASSERT_U( test_quaternion1.similar( test_quaternion0 ) );

}



static void test_matrix_to_quaternion( )
{	
	test_matrix_to_quaternion( float3( random_angle(), random_angle(), random_angle() ) );
	test_matrix_to_quaternion( float3(  n_pi_d2_random(),  n_pi_d2_random(),  n_pi_d2_random() ) );
}


////////////////////////////////test_angles_to_quaternion////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void test_angles_to_quaternion( const float3 &angles )
{
	
	quaternion test_quaternion0 ( angles );

	float4x4 rotation = create_rotation( angles );

	
	quaternion  test_quaternion1( rotation );

	TEST_ASSERT( test_quaternion1.similar( test_quaternion0 ) );
}

static void test_angles_to_quaternion( )
{
	test_angles_to_quaternion( float3( random_angle(), random_angle(), random_angle() ) );
	test_angles_to_quaternion( float3(  n_pi_d2_random(),  n_pi_d2_random(),  n_pi_d2_random() ) );
}


static void axis_angle_from_random_angles( float3 &axis, float& angle, const float3 &angles )
{
	sine_cosine	x( angles.x );
	sine_cosine	y( angles.y );


	axis  = float3( x.cosine * y.sine , x.sine * y.sine, y.cosine );
	angle = angles.z;
}

static void angle_close( float &angle, float to_angle )
{
	angle = to_angle + angle_normalize_signed( angle - to_angle );
}

static bool angle_similar(  float angle, float to_angle, float epsilon = epsilon_5 )
{
	angle_close( angle, to_angle );
	return is_similar( angle, to_angle, epsilon );
}

static void align( float3 &axis, float &angle, const float3 &to_axis, float to_angle )
{
	if( ( axis | to_axis ) < 0.f )
	{
		axis =  -axis;
		angle = -angle;
	}
	angle_close( angle, to_angle );
}

static bool is_similar( const float3 &axis,  float angle, const  float3 &to_axis, float to_angle )
{
	
	if( angle_similar( angle, 0 ) && angle_similar( to_angle, 0 )  )
		return true;
	
	float3 test_axis = axis;
	align( test_axis, angle, to_axis, to_angle );
	
	return angle_similar( angle, to_angle ) && is_similar( test_axis, to_axis ); 


}


////////////////////////////////test_axis_angle_to_quaternion////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void test_axis_angle_to_quaternion( const float3 &angles )
{
	
	float3 axis = float3().set(0,0,0);
	float  angle = 0;

	axis_angle_from_random_angles( axis, angle, angles );

	quaternion test_quaternion0 (  axis, angle );

	float3 test_axis = float3().set(0,0,0);
	float  test_angle = 0;
	test_quaternion0.get_axis_and_angle( test_axis, test_angle );

	TEST_ASSERT ( is_similar( axis, angle, test_axis, test_angle ) );

	float4x4 rotation = create_rotation(  axis, angle );

	
	quaternion  test_quaternion1( rotation );
	const float epsilon_4 = 0.0001f;

	//find out where precision can be lost (epsilon_4!)  

	TEST_ASSERT( test_quaternion1.similar( test_quaternion0, epsilon_4 ) );
}

static void test_axis_angle_to_quaternion( )
{
	test_axis_angle_to_quaternion( float3( random_angle(), random_angle(), random_angle() ) );
	test_axis_angle_to_quaternion( float3(  n_pi_d2_random(),  n_pi_d2_random(),  n_pi_d2_random() ) );
}

////////////////////////////////test_axis_angle_to_matrix////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_axis_angle_to_matrix_0( float angle )
{
	
	float4x4 rotation_x			=			create_rotation_x		( angle );

	float4x4 test_rotation_x	=			create_rotation			( float3( 1, 0 ,0 ), angle );
	TEST_ASSERT( matrix_similar( rotation_x, test_rotation_x ) );

	float4x4 rotation_y			=			create_rotation_y		( angle );
	float4x4 test_rotation_y	=			create_rotation			( float3( 0, 1 ,0 ), angle );
	TEST_ASSERT( matrix_similar( rotation_y, test_rotation_y ) );

	float4x4 rotation_z			=			create_rotation_z		( angle );
	float4x4 test_rotation_z	=			create_rotation			( float3( 0, 0 ,1 ), angle );
	TEST_ASSERT( matrix_similar( rotation_z, test_rotation_z ) );
	

}

static void test_axis_angle_to_matrix( const float3 &angles )
{

	float3 axis = float3().set(0,0,0);
	float  angle = 0;

	axis_angle_from_random_angles( axis, angle, angles );

	float4x4 rotation = create_rotation(  axis, angle );

	if( angle_similar( angle, 0 ) )
	{
		TEST_ASSERT( matrix_similar( float4x4().identity(), rotation ) );
		return;
	}

	math::quaternion q( rotation );

	float3 test_axis ; float test_angle;
	q.get_axis_and_angle( test_axis, test_angle );
	
	TEST_ASSERT( is_similar( axis, angle, test_axis, test_angle ) );

///////////////////////////////////////////////////////////////
	//math::quaternion q( axis, angle );
/////////////////////////////////////////////////////
	//float4x4

}

static void test_axis_angle_to_matrix( )
{
	test_axis_angle_to_matrix( float3( random_angle(), random_angle(), random_angle() ) );
	test_axis_angle_to_matrix( float3(  n_pi_d2_random(),  n_pi_d2_random(),  n_pi_d2_random() ) );
	test_axis_angle_to_matrix_0( n_pi_d2_random() );
	test_axis_angle_to_matrix_0(  random_angle() );
	
}

////////////////////////////////test_angles_to_matrix////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_angles_to_matrix( const float3 &angles )
{
	
	
	float4x4 rotation_x		=			create_rotation_x		( angles.x );
	float4x4 rotation_y		=			create_rotation_y		( angles.y );
	float4x4 rotation_z		=			create_rotation_z		( angles.z );

	float4x4 test_rotation	=			rotation_x  * rotation_y * rotation_z;

	float4x4 rotation		=			create_rotation		( angles );
	
	TEST_ASSERT( matrix_similar( rotation, test_rotation ) );

////////////////////////////////////////////////////////////////////////////////////

	float3 test_angles = rotation.get_angles_xyz();
	
	test_angles = to_close_xyz( test_angles, angles, pi_d2 );
	TEST_ASSERT( math::is_similar( test_angles, angles ) );


	
	float4x4 m = create_rotation( angles );
	

	test_angles = m.get_angles_xyz();
	test_angles = to_close_xyz( test_angles, angles, pi_d2 );
	TEST_ASSERT( math::is_similar( angles, test_angles ) );

	test_rotation = create_rotation		( test_angles );
	
	TEST_ASSERT( matrix_similar( rotation, test_rotation ) );


	
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/*
	rotation			=			rotation_z*rotation_x*rotation_y;
	test_rotation		=			math::create_rotation_INCORRECT( angles );

	TEST_ASSERT( compare( rotation, test_rotation, 0.01f ) );
	TEST_ASSERT( compare( test_rotation, rotation, 0.01f ) );

	
	test_angles = angles;
	test_angles.x = -math::pi/2;
	test_angles = create_rotation_INCORRECT		( test_angles ).get_angles_xyz_INCORRECT();
//////////////////////////////////////////////////////////////////////////////////////////
	//float3 cmp0 = angles, cmp1 = test_angles;
	//std::swap( cmp0.x, cmp0.y );
	//std::swap( cmp1.x, cmp1.y );
	//cmp1 = math::to_close_xyz( cmp1, cmp0, pi_d2  );
	//TEST_ASSERT( math::is_similar( cmp1, cmp0, pi_d2 ) );
///////////////////////////////////////////////////////////////////////////////////////
	test_rotation		=			math::create_rotation_INCORRECT( test_angles );
	TEST_ASSERT( matrix_similar( rotation, test_rotation ) );

*/	



}

static void all_test_angles_to_matrix( const float3 &angles )
{
	
	test_angles_to_matrix( angles );

	float3 t = angles;
	t.y = -pi/2;

	test_angles_to_matrix( t );

	t = angles;
	t.y = math::pi/2;

	test_angles_to_matrix( t );
}

static void test_angles_to_matrix()
{
	
	all_test_angles_to_matrix( float3( random_angle(), random_angle(), random_angle() ) );
	all_test_angles_to_matrix( float3(  n_pi_d2_random(),  n_pi_d2_random(),  n_pi_d2_random() ) );

}

static void all_tests( )
{
	test_angles_to_matrix();
	test_axis_angle_to_matrix( );
	test_matrix_to_quaternion( );
	test_angles_to_quaternion( );
	test_axis_angle_to_quaternion( );
}

struct math_tests
{
	static const u32	iterations_number = 100;//10000000

	void test (core_test_suite const * const)
	{
		rnd.seed( (u32)( timing::get_clocks() & u32(-1)) );
		
		for( u32 i = 0; i< iterations_number ; ++ i )
			all_tests( );
		
	}
} ;

//REGISTER_TEST_CLASS( math_tests, core_test_suite );




} // namespace math
} // namespace xray