////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/maya_animation/anim_track.h>
#include <string.h>
#include "stdlib.h"

using namespace xray::maya_animation;
using namespace xray::math;
using namespace xray::animation;

float EtCurve::evaluate(float time)
{
	float res = engineAnimEvaluate ( this, time );
	return res;
}

void EtCurve::create_keys( int count )
{
	keyListPtr	= (EtKey*)MALLOC( sizeof(EtKey) * count, "EtCurve keys" );
}

EtCurve::EtCurve():keyListPtr( 0 )
{

}

EtCurve::~EtCurve()
{
	FREE(keyListPtr);
}

anim_track::anim_track()
{
	memset( &m_channels, 0, channel_max*sizeof(m_channels[0]) );
}

anim_track::~anim_track()
{
	for( int i=0; i<channel_max; ++i )
	{
		if( m_channels[i] )
		{
			DELETE( m_channels[i] );
			m_channels[i]	= 0;
		}
	}
}


enum_channel_id	anim_track::get_channel_id( const char* name )
{
	if(0==strings::compare_insensitive("translateX", name))
		return channel_translation_x;
	else
	if(0==strings::compare_insensitive("translateY", name))
		return channel_translation_y;
	else
	if(0==strings::compare_insensitive("translateZ", name))
		return channel_translation_z;
	else
	if(0==strings::compare_insensitive("rotateX", name))
		return channel_rotation_x;
	else
	if(0==strings::compare_insensitive("rotateY", name))
		return channel_rotation_y;
	else
	if(0==strings::compare_insensitive("rotateZ", name))
		return channel_rotation_z;
	else
	if(0==strings::compare_insensitive("scaleX", name))
		return channel_scale_x;
	else
	if(0==strings::compare_insensitive("scaleY", name))
		return channel_scale_y;
	else
	if(0==strings::compare_insensitive("scaleZ", name))
		return channel_scale_z;
	else
		return channel_unknown;
}

EtCurve* anim_track::get_channel( enum_channel_id id, bool b_create )
{
	if(!m_channels[id] && b_create)
		m_channels[id] = NEW (EtCurve);
	
	return m_channels[id];
}

EtCurve* anim_track::get_channel(const char* name, bool b_create)
{
	
	enum_channel_id id	= get_channel_id(name);
	return get_channel	(id, b_create);
}

float	anim_track::evaluate( enum_channel_id id, float time )
{
	if( !m_channels[id] )
		return 0;

	//timing::timer t;
	//t.start();
	float ret = m_channels[id]->evaluate( time );
	//++m_num_evaluate;
	//m_evaluate_time += t.get_elapsed_sec();
	return ret;
}


void	anim_track::evaluate( float4x4	&pose, float time)
{
	xray::float3 translation = xray::float3(0,0,0);
	xray::float3 rotation = xray::float3(0,0,0);
	xray::float3 scale = xray::float3(1,1,1);

	translation.x = anim_track::evaluate( channel_translation_x, time );
	translation.y = anim_track::evaluate( channel_translation_y, time );
	translation.z = anim_track::evaluate( channel_translation_z, time );

	rotation.x = anim_track::evaluate( channel_rotation_x, time );
	rotation.y = anim_track::evaluate( channel_rotation_y, time );
	rotation.z = -anim_track::evaluate( channel_rotation_z, time );

	scale.x = evaluate( channel_scale_x, time );
	scale.y = evaluate( channel_scale_y, time );
	scale.z = evaluate( channel_scale_z, time );

	pose =	create_scale( scale ) * create_rotation_INCORRECT( rotation ); //*	create_translation( translation );
	pose.c.xyz() = translation;
}


float				min_time(EtCurve *animCurve)
{
	ASSERT(animCurve->numKeys>0);
	return  animCurve->keyListPtr[0].time;
}

float				max_time(EtCurve *animCurve)
{
	ASSERT(animCurve->numKeys>0);
	return  animCurve->keyListPtr[animCurve->numKeys-1].time;
}


float	anim_track::min_time					()
{
	float ret = math::infinity;
	for(u8 i = channel_translation_x; i < channel_max; ++i )
	{
		if(!m_channels[i])
			continue;
		float v = ::min_time( m_channels[i] );
		ret = math::min( ret, v );
	}
	return ret;
}

float	anim_track::max_time					()
{
	float ret = -math::infinity;
	for(u8 i = channel_translation_x; i < channel_max; ++i )
	{
		if(!m_channels[i])
			continue;
		float v = ::max_time( m_channels[i] );
		ret = math::max( ret, v );
	}
	return ret;
}

void	anim_track::reset_profile()
{
 	m_num_evaluate = 0 ;
	m_evaluate_time = 0;
}