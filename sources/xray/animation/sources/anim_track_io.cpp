////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/anim_track.h>
#include <xray/fs_utils.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(animation_anim_track_io)

using namespace xray::animation;

using namespace xray::math;

pcstr channel_names [] ={
"translateX",
"translateY",
"translateZ",
"rotateX",
"rotateY",
"rotateZ",
"scaleX",
"scaleY",
"scaleZ",
"time"
};

#ifndef MASTER_GOLD

void anm_track::save( xray::configs::lua_config_value t_root )
{
	t_root["version"]	= 1;
	
	for( int ch_id = 0; ch_id < m_max_channels; ++ch_id )
	{
		EtCurve* c		= m_channels[ch_id];
		if( !c )			
			continue;

		pcstr ch_id_str	= channel_names[ch_id];
		c->save			( t_root["channels"][ch_id_str] );
	}
}

void anm_track::load( xray::configs::lua_config_value const& t_root )
{
	int ver				= t_root["version"];
	if(ver!=1)
		return;
	
	configs::lua_config_value t_channels	= t_root["channels"];

	configs::lua_config_iterator it			= t_root["channels"].begin();
	configs::lua_config_iterator it_e		= t_root["channels"].end();

	for( ;it!=it_e; ++it )
	{
		pcstr ch_id_str				= it.key();
		enum_channel_id ch_id		= get_channel_id( ch_id_str );
		R_ASSERT					(m_channels[ch_id]!=NULL);
		//m_channels[ch_id]			= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);

		EtCurve* c					= m_channels[ch_id];
		configs::lua_config_value t = *it;
		c->load						( t );
	}

	reset_profile			( );
}
#endif

void anm_track::load( xray::configs::binary_config_value const& t_root )
{
	int ver				= t_root["version"];
	if(ver!=1)
		return;
	
	configs::binary_config_value t_channels	= t_root["channels"];

	configs::binary_config_value::const_iterator it			= t_root["channels"].begin();
	configs::binary_config_value::const_iterator it_e		= t_root["channels"].end();

	for( ;it!=it_e; ++it )
	{
		pcstr ch_id_str			= it->key();
		enum_channel_id ch_id	= get_channel_id(ch_id_str);
		R_ASSERT(m_channels[ch_id]!=NULL);
		//m_channels[ch_id]		= XRAY_NEW_IMPL( m_allocator, EtCurve )(m_allocator);
		EtCurve* c				= m_channels[ch_id];
		configs::binary_config_value t = *it;
		c->load					( t );
	}

	reset_profile			( );
}

#ifndef MASTER_GOLD
void EtCurve::load( xray::configs::lua_config_value const& t )
{
	keyList.clear	( );
	reset			( );

	u32 numKeys		= t["keys"].size();
	isWeighted		= t["is_weighted"];
	isStatic		= t["is_static"];
	preInfinity		= (EtInfinityType)(int)t["pre_inf"];
	postInfinity	= (EtInfinityType)(int)t["post_inf"];

	keyList.resize	( numKeys );
	for(u32 i=0; i<numKeys; ++i )
	{
		EtKey& k	= keyList[i];
		k.load		( t["keys"][i] );
	}
}
#endif

void EtCurve::load( xray::configs::binary_config_value const& t )
{
	keyList.clear	( );
	reset			( );

	u32 numKeys		= t["keys"].size();
	isWeighted		= t["is_weighted"];
	isStatic		= t["is_static"];
	preInfinity		= (EtInfinityType)(int)t["pre_inf"];
	postInfinity	= (EtInfinityType)(int)t["post_inf"];

	keyList.resize	( numKeys );
	for(u32 i=0; i<numKeys; ++i )
	{
		EtKey& k	= keyList[i];
		k.load		( t["keys"][i] );
	}
}

#ifndef MASTER_GOLD
void EtCurve::save( xray::configs::lua_config_value t )
{
	t["is_weighted"]	= isWeighted;
	t["is_static"]		= isStatic;
	t["pre_inf"]		= (int)preInfinity;
	t["post_inf"]		= (int)postInfinity;

	for(u32 i=0; i<keyList.size(); ++i )
	{
		EtKey& k	= keyList[i];
		k.save		( t["keys"][i] );
	}
}

void EtKey::load( xray::configs::lua_config_value const& t )
{
	time					= t["time"];
	value					= t["value"];										/* key value (in internal units)*/
	math::float2 in_tan		= t["in_tan"];
	inTanX					= in_tan.x;											/* key in-tangent x value		*/
	inTanY					= in_tan.y;											/* key in-tangent y value		*/

	math::float2 out_tan	= t["out_tan"];
	outTanX					= out_tan.x;										/* key out-tangent x value		*/
	outTanY					= out_tan.y;										/* key out-tangent y value		*/
	type					= (u8)( t.value_exists("type") ? t["type"] : 0 );	/* key type						*/
}
#endif

void EtKey::load( xray::configs::binary_config_value const& t )
{
	time					= t["time"];
	value					= t["value"];										/* key value (in internal units)*/
	math::float2 in_tan		= t["in_tan"];
	inTanX					= in_tan.x;											/* key in-tangent x value		*/
	inTanY					= in_tan.y;											/* key in-tangent y value		*/

	math::float2 out_tan	= t["out_tan"];
	outTanX					= out_tan.x;										/* key out-tangent x value		*/
	outTanY					= out_tan.y;										/* key out-tangent y value		*/
	type					= (u8)( t.value_exists("type") ? t["type"] : 0 );	/* key type						*/
}

#ifndef MASTER_GOLD
void EtKey::save( xray::configs::lua_config_value t )
{
	t["time"]				= time;
	t["value"]				= value;
	t["in_tan"]				= math::float2(inTanX, inTanY);
	t["out_tan"]			= math::float2(outTanX, outTanY);
	t["type"]				= type;
}
#endif