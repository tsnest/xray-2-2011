////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
//#include "anim_track_discrete.h"
//#include <xray/maya_animation/anim_track.h>
//
//using namespace xray::math;
//
//using xray::maya_animation::anim_track_discrete;
//using xray::maya_animation::anim_track;
//using xray::maya_animation::EtCurve;
//
//void anim_track_discrete::build( anim_track& ad )
//{
//	const float min_time =  ad.min_time();
//	const float max_time =  ad.max_time();
//	
//	const float interval = max_time - min_time;
//	if( !xray::math::valid( interval ) )
//		return;
//	const u32	frame_count		= u32(xray::math::floor( interval/time_delta ));
//	m_start_time = min_time;
//	
//	m_positions.resize(frame_count);m_rotations.resize(frame_count);m_scale.resize(frame_count);
//	for(u32 i =0; i < frame_count; ++i )
//	{
//		float time = min_time + i * time_delta;
//		float4x4 bone_pos;
//		ad.evaluate( bone_pos, time );
//		m_positions[i] = ( bone_pos.c.xyz() );
//		m_rotations[i] = ( quaternion (bone_pos) );
//		
//		m_scale[i] = ( float3(
//		xray::math::sqrt( bone_pos.e00 * bone_pos.e00 + bone_pos.e10 * bone_pos.e10 +  bone_pos.e20 * bone_pos.e20 ),
//		xray::math::sqrt( bone_pos.e01 * bone_pos.e01 + bone_pos.e11 * bone_pos.e11 +  bone_pos.e21 * bone_pos.e21 ),
//		xray::math::sqrt( bone_pos.e02 * bone_pos.e02 + bone_pos.e12 * bone_pos.e12 +  bone_pos.e22 * bone_pos.e22 )
//		));
//	}
//
//}
//
//void	anim_track_discrete::bone_pos( xray::float4x4 &pose, float time )
//{
//	const u32 count = m_rotations.size();
//	if( time < m_start_time )
//		time = m_start_time;
//	
//	if( count==0 )
//	{
//		pose.identity();
//		return;
//	}
//	float	frame_time	=	( ( time - m_start_time ) * anim_fps ); //anim_fps = 1/time_delta
//	u32		frame	=	xray::math::floor( frame_time );
//	float	delta	=	frame_time-float(frame);
//	
//	
//	frame = frame % ( count-2 );
//	
//	quaternion rotation = slerp( m_rotations[ frame ], m_rotations[ frame + 1 ],  delta );
//	
//	float	rdelta = 1.f - delta;
//	float3	scale		= m_scale[ frame ] * delta + m_scale[ frame + 1 ] * rdelta;
//	float3	position	= m_positions[ frame ] * delta + m_positions[ frame + 1 ] * rdelta;
//
//	pose = create_scale( scale ) * create_matrix( rotation, position ); 
//}
//
