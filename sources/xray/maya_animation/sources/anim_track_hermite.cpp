////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
//#include "anim_track_hermite.h"
//#include <xray/maya_animation/anim_track.h>
//#include <xray/animation/anim_track_common.h>
//using namespace xray::maya_animation;
//using namespace xray::animation;
//
//void xray::maya_animation::engineHermiteCreate (EtCurve *animCurve, float x[4], float y[4]);
//
//void	build( hermite_key_interval &ki, const EtKey &k0, const EtKey &k1 );
//
//static const float	one_third  = 1.f/3.f;
//
//void	build( hermite_key_interval &ki, const EtKey &k0, const EtKey &k1 )
//{
////constant value case	
//	//ki.x1 = temp_curve.fX1;
//	ki.x1 = k0.time;
//	ki.coeff = xray::math::float4(0,0,0,0);
//	if ((k0.outTanX == 0.0) &&	(k0.outTanY == 0.0)) 
//	{
//		ki.coeff[3]  = k0.value;
//		return;
//	}else 
//		if( (k0.outTanX == xray::math::float_max) &&	(k0.outTanY == xray::math::float_max) )
//	{
//		ki.coeff[3] = k1.value;
//		return;
//	}
//
//	
////general case
//
//	float x[4];
//	float y[4];
//	
//	x[0]					= k0.time;
//	y[0]					= k0.value;
//	x[1]					= x[0] + (k0.outTanX * one_third);
//	y[1]					= y[0] + (k0.outTanY * one_third);
//
//	//nextKey					= &(animCurve->keyListPtr[index]);
//	x[3] 					= k1.time;
//	y[3] 					= k1.value;
//	x[2] 					= x[3] - (k1.inTanX * one_third);
//	y[2] 					= y[3] - (k1.inTanY * one_third);
//	
//	EtCurve					temp_curve(g_allocator);
//
//	engineHermiteCreate		( &temp_curve, x, y);
//
//	//ki.x1 = temp_curve.fX1;
//	ki.coeff = xray::math::float4(
//	temp_curve.fCoeff[0],
//	temp_curve.fCoeff[1],
//	temp_curve.fCoeff[2],
//	temp_curve.fCoeff[3]
//	);
//
//}
//
//anim_curve_hermite::anim_curve_hermite( )
//:m_time_end		( 0 ), 
//m_last_interval	( 0 ), 
//b_static		( false )
//{
//		
//}
//
//void anim_curve_hermite::build( const EtKey *keys, u32 num_keys )
//{
//	ASSERT(num_keys>1);
//	m_time_end		= keys[ num_keys-1 ].time;
//	m_last_interval = 0;
//	m_keys.resize(num_keys-1);
//	for(u32 i = 0; i < num_keys-1; ++i )
//	{
//		//hermite_key_interval ki;
//		::build( m_keys[i], keys[i], keys[i+1] );
//		//m_keys.push_back( ki );
//	}	
//
//}
//void	anim_curve_hermite::build( const EtCurve *anim_curve )
//{
//	if(!anim_curve)
//		return;
//	//ASSERT(!anim_curve->isWeighted);
//	b_static = anim_curve->isStatic;
//	//ASSERT( !anim_curve->isStatic );
//	build( &anim_curve->keyList.front(), anim_curve->keyList.size() );
//}
//
//float	anim_curve_hermite::evaluate( float time )const
//{
//	const u32 sz = m_keys.size();
//
//	if(sz==0)
//		return 0 ;
//	
//	if( b_static )
//		return m_keys[0].coeff.w;
//
//	float start_time = m_keys[0].x1;
//	math::clamp( time, start_time, m_time_end );
//	
//
//	for( u32 i = 0, j = m_last_interval; i < sz; ++i, ++j )
//	{
//		
//		const hermite_key_interval &current = m_keys[j];
//		u32 last_interval = j;
//		float next_time;
//		if( j + 1 == sz )
//		{
//			j = u32(-1);
//			next_time = m_time_end;
//		} else
//		{
//			next_time =m_keys[j+1].x1 ;
//		}
//		
//		if( current.x1 <= time && time <= next_time )
//		{
//			m_last_interval = 	last_interval;
//			return ::evaluate( current, time );
//		}
//	}
//	
//	UNREACHABLE_CODE(return 0);
//}
//
//
//float anim_track_hermite::evaluate(  enum_channel_id id, float time ) const
//{
//	return m_channels[id].evaluate( time );
//}
//
//void anim_track_hermite::bone_pos( xray::float4x4	&pose, float time )const
//{
//	xray::float3 translation = xray::float3(0,0,0);
//	xray::float3 rotation = xray::float3(0,0,0);
//	xray::float3 scale = xray::float3(1,1,1);
//
//	translation.x = evaluate( channel_translation_x, time );
//	translation.y = evaluate( channel_translation_y, time );
//	translation.z = evaluate( channel_translation_z, time );
//
//	rotation.x = evaluate( channel_rotation_x, time );
//	rotation.y = evaluate( channel_rotation_y, time );
//	rotation.z = -evaluate( channel_rotation_z, time );
//
//
//
//	scale.x = evaluate( channel_scale_x, time );
//	scale.y = evaluate( channel_scale_y, time );
//	scale.z = evaluate( channel_scale_z, time );
//
//	pose =	create_scale( scale ) * create_rotation( rotation ); //*	create_translation( translation );
//	pose.c.xyz() = translation;
//}
//
//void anim_track_hermite::build( const anim_track &anim_curve )
//{
//	for(int i = channel_translation_x; i < channel_max; ++i )
//			m_channels[i].build( anim_curve.m_channels[i] );
//}
//
