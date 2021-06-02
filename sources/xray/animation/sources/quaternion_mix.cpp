////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "quaternion_mix.h"

#include "eigen_matrix.h"

#include <xray/animation/anim_track_common.h>


namespace xray {
namespace animation {

float sum_weights( const buffer_vector< float > &weights )
{
	buffer_vector< float >::const_iterator 
		i = weights.begin(),
		e = weights.end();
	float  ret = 0.f;
	for( ; i!=e; ++i )
		ret += *i;
	return ret;
}



void get_frames_and_weights( buffer_vector< frame > &frames, 
							 buffer_vector< float > &weights, 
							 const animation_vector_type &blends, u32 bone, float time )
{
	
	const u32 count = blends.size();

	ASSERT( count <= blends.capacity() );
	ASSERT( count <= frames.capacity() );

	frames.resize( count );
	weights.resize( count );

	for( u32 i = 0; i < count; ++i )
	{
		blends[i].get_frame( bone, time, frames[i] );
		weights[i] = blends[i].factor();
	}

	ASSERT( math::similar( sum_weights( weights ), 1.f ) );
}

void get_quaternions( buffer_vector< math::quaternion > &quaternions, const buffer_vector< frame > &frames )
{
	ASSERT( frames.size() == quaternions.capacity() );
	
	const u32 count = frames.size();

	quaternions.resize( count );

	for( u32 i = 0; i < count; ++i )
		quaternions[i]  = math::quaternion( frames[i].rotation );

}

float outer_product_matrix_element( u32 i, u32 j, const buffer_vector< math::quaternion > &quaternions )
{
	
	ASSERT( i < 4 );
	ASSERT( j < 4 );

	const u32 count	= quaternions.size( );

	float res = 0.f;
	for( u32 k = 0; k < count; ++k )
		res += quaternions[k].vector.elements[i] * quaternions[k].vector.elements[j];
	return res;

}

float4x4 outer_product_matrix( const buffer_vector< math::quaternion > &quaternions )
{
	float4x4 res;

	for( u32 i = 0; i < 4; ++i )
		for( u32 j = 0; j < 4; ++j )
			res.elements[i][j] = outer_product_matrix_element( i, j, quaternions );

	return res;
}

math::quaternion get_mean_quaternion( const buffer_vector< math::quaternion > &quaternions )
{
	
	float4x4 qtq = outer_product_matrix( quaternions );
	
	math::Eigen	eigen_matrix	( qtq );
	eigen_matrix.DecrSortEigenStuffN();

	return math::quaternion ( eigen_matrix.GetEigenvector( 0 ) );
}

//inline void hemi_flip( math::quaternion &q, const math::quaternion &to )
//{
//	q = math::hemi_flip( q, to );
//}

//inline bool is_unit( const math::quaternion &q )
//{
//	return math::similar( abs( q.w ), 1.f, math::epsilon_6 );
//}

math::quaternion tangent_space_weighted_sum( const math::quaternion &reference, const  buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
{

		ASSERT( weights.size() == quaternions.size() );
		//ASSERT( math::similar( reference.magnitude(), 1.f, math::epsilon_3 ) );
		
		const math::quaternion inverce_reference = math::quaternion( reference ).conjugate();
		
		math::quaternion weighted_sum( float4( 0, 0, 0, 0 ) );

		const u32 count = quaternions.size();

		for( u32 i = 0; i < count; ++i )
		{
			math::quaternion q = quaternions[i];

			math::hemi_flip( q, reference  );
			
			math::quaternion relative = inverce_reference *  q;

			weighted_sum += math::quaternion( weights[i] * math::logn( relative ).vector );
		}
		return weighted_sum;
}


struct qslim
{
	
	static math::quaternion mix( const  buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
	{
		ASSERT( weights.size() == quaternions.size() );

		const math::quaternion reference = get_mean_quaternion( quaternions );
		
		math::quaternion weighted_sum = tangent_space_weighted_sum( reference, quaternions, weights );

		return reference * math::exp( weighted_sum );

	}

}; // struct qslim

static const math::quaternion unit_quaternion = math::quaternion( float4( 0, 0, 0, 1 ) );
math::quaternion weighted_mean( const  buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
{
	
	const math::quaternion mean = get_mean_quaternion( quaternions ); // use it to flip into same hemi

	ASSERT( quaternions.size() == weights.size() );

	const u32 count = quaternions.size();

	math::quaternion sum = math::quaternion( float4( 0, 0, 0, 0 ) );

	for( u32 i = 0; i < count; ++i )
	{
		math::quaternion q = quaternions[i];
		math::hemi_flip( q, mean  );
		sum += math::quaternion( weights[i] * math::logn( q ).vector );
	}
	
	const float magnitude = sum.magnitude();
	
	if( math::similar( magnitude, 0.f ) )
	{
		math::quaternion ret = unit_quaternion; 
		math::hemi_flip ( ret, mean  ); 
		return ret;
	}
	const float recip_magnitude = 1.f/magnitude;

	sum  = math::quaternion(  recip_magnitude * sum.vector );

	math::quaternion ret = math::exp( sum );
	
	ret.normalize();

	return ret;

}

void hemispherize( buffer_vector< math::quaternion > &quaternions, const math::quaternion &to )
{
	const u32 count = quaternions.size();
	for( u32 i = 0; i < count; ++i )
		math::hemi_flip( quaternions[i], to  );
}

void sasquatch_slerp_test( const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights );
void cmp_to_slerp( const math::quaternion &q, const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights );

struct sasquatch
{
	
	static math::quaternion mix( const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
	{
		
		const float step_delta		= 1.175f; 
		const u32	max_iterations	= 1000;

		//const math::quaternion q0 = qslim::mix( quaternions, weights );
		const math::quaternion q0 = weighted_mean( quaternions, weights );
		
		math::quaternion q = q0;
		u32 ineration = 1;

		do{
			math::quaternion omega = tangent_space_weighted_sum( q, quaternions, weights );
			omega.w = 0;

			math::quaternion r = exp ( math::quaternion( step_delta * omega.vector ) );

			q = q * r ;
			
			q.normalize( );

			++ineration;

			if( omega.magnitude() < math::epsilon_7 )
				break;

		} while(  ineration < max_iterations );
		
#ifdef DEBUG
		if( quaternions.size() == 2 )
			cmp_to_slerp( q, quaternions, weights );
#endif

		return q;

	}
	
}; // struct sasquatch

void cmp_to_slerp( const math::quaternion &q, const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
{
		ASSERT( quaternions.size()==2 );
		math::quaternion q00 = quaternions[0]; q00.normalize( );
		math::quaternion q01 = quaternions[1]; q01.normalize( );
		
		math::hemi_flip( q01, q00 );
		math::quaternion slerp = math::slerp( q00, q01,		1.f - weights[0] );
		math::quaternion slerp1 = math::slerp( q01, q00,	weights[0] );

		math::quaternion test = slerp;

		test.normalize();
		math::hemi_flip( test, q );

		const float epsilon  =  math::epsilon_6;// 0.0001f;
		ASSERT_U( math::similar( test.vector.x, q.vector.x, epsilon ) );
		ASSERT_U( math::similar( test.vector.y, q.vector.y, epsilon ) );
		ASSERT_U( math::similar( test.vector.z, q.vector.z, epsilon ) );
		ASSERT_U( math::similar( test.vector.w, q.vector.w, epsilon ) );
}

void sasquatch_slerp_test( const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
{
		ASSERT( quaternions.size()==2 );

		math::quaternion q = sasquatch::mix( quaternions, weights );

		cmp_to_slerp( q, quaternions, weights );

}

struct multilinear
{
	static math::quaternion mix( const buffer_vector< math::quaternion > &quaternions, const buffer_vector< float > &weights )
	{
		
		ASSERT( quaternions.size() == weights.size() );
		
		const u32 number = quaternions.size();

		ASSERT( number != 0 );

		math::quaternion res = quaternions[0];

		float factor = weights[0];

		for( u32 i = 1; i < number; ++i )
		{
			math::quaternion q = quaternions[i];

			math::hemi_flip( q, res );

			float q_factor = weights[i];

			float new_factor = factor + q_factor;

			res = slerp( res, q, q_factor/new_factor );

			factor = new_factor;
		}
		
		return res;
	}

}; // multilinear


void mixed_scale_and_translation( float3 &scale, float3 &translation, 
								  const buffer_vector< float > &weights,
								  const buffer_vector< frame > &frames )
{
	ASSERT( weights.size() == frames.size() );

	const u32 count = frames.size();
	
	scale = float3( 0, 0, 0 );
	translation = float3( 0, 0, 0 );

	for( u32 i = 0; i < count; ++i )
	{
		scale			+= weights[i]*frames[i].scale;
		translation		+= weights[i]*frames[i].translation;
	}

}

template< class Method >
float4x4 quaternion_mix( const animation_vector_type &blends, u32 bone, float time )
{
	const u32 count = blends.size();

	buffer_vector< frame >				frames		( ALLOCA( count * sizeof(frame) ), count );
	buffer_vector< math::quaternion >	quaternions ( ALLOCA( count * sizeof( math::quaternion ) ), count );
	buffer_vector< float >				weights		( ALLOCA( count * sizeof( float ) ), count );

	get_frames_and_weights( frames, weights, blends, bone, time );

	get_quaternions( quaternions, frames );

	float3 scale, translation;
	mixed_scale_and_translation( scale, translation, weights, frames  );

	math::quaternion mixed_quat = Method::mix( quaternions, weights );

	float4x4 res = create_matrix( mixed_quat, translation );

	return res;
}

float4x4 qslim_mix( const animation_vector_type &blends, u32 bone, float time )
{
	return quaternion_mix < qslim > ( blends, bone, time );
}

float4x4 sasquatch_mix( const animation_vector_type &blends, u32 bone, float time )
{
	return quaternion_mix < sasquatch > ( blends, bone, time );
}

float4x4 multilinear_mix( const animation_vector_type &blends, u32 bone, float time )
{
	return quaternion_mix < multilinear > ( blends, bone, time );
}

}  // namespace animation
}  // namespace xray