////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::math::float3_pod;
using xray::math::float3;

// change angles xyz representation to be close to to_angles; average_distance = average distance criteria 
float3 xray::math::to_close_xyz( float3_pod const& angles, float3_pod const& to_angles, float const average_distance )
{
#if 0
	float3 dist;
	//reduce distance to  pi,-pi 
	dist.x =	 angle_normalize_signed( angles.x - to_angles.x );
	dist.y =	 angle_normalize_signed( angles.y - to_angles.y );
	dist.z =	 angle_normalize_signed( angles.z - to_angles.z );
	
	float y_reverce_dist = angle_normalize_signed( -angles.y - to_angles.y );

	float y_dist = abs( y_reverce_dist ) + abs( dist.y );//.

	//float y_dist = angle_normalize_signed( abs( y_reverce_dist ) - abs( dist.y ) );//.

	float avr = ( abs( dist.x ) + abs( y_dist ) + abs( dist.z ) )/3.f;
	
	if( avr >= average_distance - math::epsilon_6  ) // than change triple representation
	{
		dist.x = angle_rotation_dist_hemi_flip( dist.x );
		dist.y = angle_rotation_dist_hemi_flip( y_reverce_dist );
		dist.z = angle_rotation_dist_hemi_flip( dist.z );
	}

	return		to_angles + dist;
#else // #if 0
	//reduce distance to  pi,-pi 
	float3 const distance			=
		float3 (
			angle_normalize_signed( angles.x - to_angles.x ),
			angle_normalize_signed( angles.y - to_angles.y ),
			angle_normalize_signed( angles.z - to_angles.z )
		);
	
	float const y_reverce_distance	= angle_normalize_signed( -angles.y - to_angles.y );
	float const y_distance			= abs( y_reverce_distance ) + abs( distance.y );//.
	float const average				= ( abs( distance.x ) + abs( y_distance ) + abs( distance.z ) )/3.f;
	if ( average < average_distance - math::epsilon_6  )
		return						to_angles + distance;

	// change triple representation
	return
		to_angles +
		float3(
			angle_rotation_dist_hemi_flip( distance.x ),
			angle_rotation_dist_hemi_flip( y_reverce_distance ),
			angle_rotation_dist_hemi_flip( distance.z )
		);
#endif // #if 0
}