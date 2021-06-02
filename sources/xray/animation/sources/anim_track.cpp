////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/anim_track.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(animation_anim_track)

using namespace xray::animation;
#define kMaxTan 5729577.9485111479f

namespace xray {
namespace animation {

EtCurve::EtCurve	( xray::memory::base_allocator* a )
:keyList(a)
{
	isWeighted		= false;
	isStatic		= false;
	preInfinity		= kInfinityConstant;
	postInfinity	= kInfinityConstant;
	reset			( );
}

EtCurve::~EtCurve	( )
{
}

void			EtCurve::reset						( )
{
	//initialize cache
	lastKey			= 0;
	lastIndex		= -1;
	lastInterval	= -1;
	isStep			= false;
	isStepNext		= false;
}

float			EtCurve::evaluate					( float time )
{
	return engineAnimEvaluate ( this, time );
}

void			EtCurve::insert						( float time, float value )
{
	// find
	xray::vectora<EtKey>::iterator it = std::lower_bound( keyList.begin(), keyList.end(), time );
	EtKey			k;
	k.time			= time;
	k.value			= value;
	k.inTanX		= -0.00001f;
	k.inTanY		= value;
	k.outTanX		= 0.00001f;
	k.outTanY		= value;
	k.type			= 0;

	keyList.insert	( it, k );
	reset			( );
}

void			EtCurve::remove						( u32 idx )
{
	keyList.erase(keyList.begin()+idx);
}

void			EtCurve::calc_smooth_tangents		( )
{
	EtKey* key		= 0;
	EtKey* prevKey	= 0;
	EtKey* nextKey	= 0;

	float		inTanX = xray::memory::uninitialized_value<float>(); 
	float		inTanY = xray::memory::uninitialized_value<float>(); 
	float		outTanX = xray::memory::uninitialized_value<float>(); 
	float		outTanY = xray::memory::uninitialized_value<float>();
	float		inTanXs, inTanYs, outTanXs, outTanYs;

	u32 key_count = keyList.size();

	for( u32 i=0; i<key_count; ++i)
	{	
		key				= &(keyList[i]);

		if(i==0)
			prevKey = NULL;
		else
			prevKey		= &(keyList[i-1]);

		if(i==(key_count-1) )
			nextKey = NULL;
		else
			nextKey		= &(keyList[i+1]);

		if( (prevKey == 0) && (nextKey != 0) ) 
		{
			outTanXs	= nextKey->time - key->time;
			outTanYs	= nextKey->value - key->value;
			inTanXs		= outTanXs;
			inTanYs		= outTanYs;
		}else 
		if ((prevKey != 0) && (nextKey == 0)) 
		{
			outTanXs	= key->time - prevKey->time;
			outTanYs	= key->value - prevKey->value;
			inTanXs		= outTanXs;
			inTanYs		= outTanYs;
		}else 
		if( (prevKey != 0) && (nextKey != 0) )
		{
			/* There is a CV before and after this one*/
			/* Find average of the adjacent in and out tangents. */

			float dx = nextKey->time - prevKey->time;
			if (dx < 0.0001) 
				outTanYs = kMaxTan;
			else 
				outTanYs = (nextKey->value - prevKey->value) / dx;

			outTanXs		= nextKey->time - key->time;
			inTanXs			= key->time - prevKey->time;
			inTanYs			= outTanYs * inTanXs;
			outTanYs		*= outTanXs;
		}else 
		{
			inTanXs			= 1.0;
			inTanYs			= 0.0;
			outTanXs		= 1.0;
			outTanYs		= 0.0;
		}

		inTanX = inTanXs;
		inTanY = inTanYs;
		outTanX = outTanXs;
		outTanY = outTanYs;

		if (inTanX < 0.0)
			inTanX = 0.0;
			
		float length			= sqrt ((inTanX * inTanX) + (inTanY * inTanY));
		if (length != 0.0f) 
		{	/* zero lengths can come from step tangents */
			inTanX /= length;
			inTanY /= length;
		}
		if ((inTanX == 0.0f) && (inTanY != 0.0f)) 
		{
			inTanX = 0.0001f;
			inTanY = (inTanY < 0.0f ? -1.0f : 1.0f) * (inTanX * kMaxTan);
		}
		if (outTanX < 0.0f) 
			outTanX = 0.0f;
			
		length				= sqrt ((outTanX * outTanX) + (outTanY * outTanY));
		if (length != 0.0f) 
		{	/* zero lengths can come from step tangents */
			outTanX /= length;
			outTanY /= length;
		}
		if ((outTanX == 0.0f) && (outTanY != 0.0f)) 
		{
			outTanX = 0.0001f;
			outTanY = (outTanY < 0.0f ? -1.0f : 1.0f) * (outTanX * kMaxTan);
		}

		key->inTanX 		= inTanX;
		key->inTanY 		= inTanY;
		key->outTanX		= outTanX;
		key->outTanY		= outTanY;

	}// for all keys
	reset();
}


anm_track::anm_track	( xray::memory::base_allocator* a )
:m_allocator( a )
{
	m_max_channels	= channel_max;
	m_channels		= XRAY_NEW_ARRAY_IMPL( m_allocator, EtCurve*, m_max_channels );
	memset			( m_channels, 0, m_max_channels*sizeof( m_channels[0] ) );
}

anm_track::anm_track	( xray::memory::base_allocator* a, int max_channels )
:m_allocator( a )
{
	m_max_channels	= max_channels;
	m_channels		= XRAY_NEW_ARRAY_IMPL( m_allocator, EtCurve*, m_max_channels );
	memset			( m_channels, 0, m_max_channels*sizeof( m_channels[0] ) );
}

anm_track::~anm_track	( )
{
	for( int i=0; i<m_max_channels; ++i )
	{
		if( m_channels[i] )
		{
			XRAY_DELETE_IMPL	( m_allocator, m_channels[i] );
			m_channels[i]		= 0;
		}
	}
	XRAY_DELETE_ARRAY_IMPL( m_allocator, m_channels );
}

void			anm_track::initialize_empty			( bool bposition, bool brotation, bool bscale )
{
	if(bposition)
	{
		m_channels[channel_translation_x]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_translation_y]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_translation_z]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
	}
	if(brotation)
	{
		m_channels[channel_rotation_x]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_rotation_y]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_rotation_z]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
	}
	if(bscale)
	{
		m_channels[channel_scale_x]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_scale_y]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		m_channels[channel_scale_z]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
	}

	for( int i = 9; i < m_max_channels; ++i )
		m_channels[i]	= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
}

enum_channel_id	anm_track::get_channel_id			( pcstr name )
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
	if(0==strings::compare_insensitive("time", name))
		return (enum_channel_id)m_time_channel_id;
	else
		return channel_unknown;
}

EtCurve*		anm_track::get_channel				( enum_channel_id id, bool b_create )
{
	if(!m_channels[id] && b_create)
		m_channels[id] = XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
	
	return m_channels[id];
}

EtCurve*		anm_track::get_channel				( pcstr name, bool b_create)
{
	enum_channel_id id	= get_channel_id(name);
	return get_channel	(id, b_create);
}

float			anm_track::evaluate					( enum_channel_id id, float channel_position )
{
	if( !m_channels[id] )
		return 0;

	//timing::timer t;
	//t.start();
	float ret = m_channels[id]->evaluate( channel_position );
	//++m_num_evaluate;
	//m_evaluate_time += t.get_elapsed_sec();
	return ret;
}

void			anm_track::evaluate					( float4x4& pose, float time, bool use_time, float& on_track_position )
{
	if( use_time )
		time				= anm_track::evaluate( (enum_channel_id)m_time_channel_id, time );

	on_track_position	= time;
	evaluate( pose, time );
}

void			anm_track::evaluate					( float4x4& pose, float time, bool use_time )
{
	if( use_time )
		time				= anm_track::evaluate( (enum_channel_id)m_time_channel_id, time );
	
	evaluate( pose, time );
}

void			anm_track::evaluate					( float4x4& pose, float on_track_position )
{
	float3 translation;
	float3 rotation;
	float3 scale;

	translation.x 		= anm_track::evaluate( channel_translation_x, on_track_position );
	translation.y 		= anm_track::evaluate( channel_translation_y, on_track_position );
	translation.z 		= anm_track::evaluate( channel_translation_z, on_track_position );

	rotation.x 			= anm_track::evaluate( channel_rotation_x, on_track_position );
	rotation.y 			= anm_track::evaluate( channel_rotation_y, on_track_position );
	rotation.z 			= anm_track::evaluate( channel_rotation_z, on_track_position );

	scale.x 			= evaluate( channel_scale_x, on_track_position );
	scale.y 			= evaluate( channel_scale_y, on_track_position );
	scale.z 			= evaluate( channel_scale_z, on_track_position );

	pose				= create_scale( scale ) * create_rotation( rotation ); //*	create_translation( translation );
	pose.c.xyz()		= translation;
}

void			anm_track::insert					( float on_track_position, float3 const& position, float3 const& rotation, float3 const& scale )
{
	m_channels[channel_translation_x]->insert( on_track_position, position.x );
	m_channels[channel_translation_y]->insert( on_track_position, position.y );
	m_channels[channel_translation_z]->insert( on_track_position, position.z );

	m_channels[channel_rotation_x]->insert( on_track_position, rotation.x );
	m_channels[channel_rotation_y]->insert( on_track_position, rotation.y );
	m_channels[channel_rotation_z]->insert( on_track_position, rotation.z );

	m_channels[channel_scale_x]->insert( on_track_position, scale.x );
	m_channels[channel_scale_y]->insert( on_track_position, scale.y );
	m_channels[channel_scale_z]->insert( on_track_position, scale.z );
}

void			anm_track::remove					( u32 idx )
{
	for(int i = channel_translation_x; i < channel_max; ++i )
	{
		EtCurve*	c =	get_channel	( (xray::animation::enum_channel_id)i, false );
		if(c)
			c->remove	( idx );
	}

}

void			anm_track::calc_smooth_tangents		( )
{
	for(int i = channel_translation_x; i < m_max_channels; ++i )
	{
		EtCurve*	c = m_channels[i];
		if( c )
			c->calc_smooth_tangents	( );
	}
}

float			curve_min_time						( EtCurve *animCurve )
{
	
	return  (animCurve->keyList.empty()) ? 0.0f : animCurve->keyList[0].time;
}

float			curve_max_time						( EtCurve *animCurve )
{
	return  (animCurve->keyList.empty()) ? 0.0f : animCurve->keyList.back().time;
}


float			anm_track::min_time					( )
{
	float ret			= math::infinity;
	
	for(u8 i = channel_translation_x; i < m_max_channels; ++i )
	{
		if(!m_channels[i])
			continue;

		float v			= curve_min_time( m_channels[i] );
		ret				= math::min( ret, v );
	}
	return				ret;
}

float			anm_track::max_time					( )
{
	float ret			= -math::infinity;

	for(u8 i = channel_translation_x; i < m_max_channels; ++i )
	{
		if(!m_channels[i])
			continue;

		float v			= curve_max_time( m_channels[i] );
		ret				= math::max( ret, v );
	}
	return				ret;
}

void			anm_track::reset_profile			( )
{
 	m_num_evaluate		= 0 ;
	m_evaluate_time		= 0;
}

} //namespace xray
} //namespace animation
