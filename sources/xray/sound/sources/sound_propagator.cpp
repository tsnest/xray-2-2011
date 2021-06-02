////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_propagator.h"
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/sound_rms.h>
#include <xray/sound/sound_spl.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"


namespace xray {
namespace sound {

static inline sound_world* get_world	( sound_instance_proxy_internal const& proxy )
{
	return proxy.get_world_user().get_sound_world();
}

static u32 prop_id					= 0;

sound_propagator::sound_propagator	(	playback_mode mode,
										u32	playing_offset,
										u32 before_playing_offset,
										u32 after_playing_offset,
										sound_instance_proxy_internal& proxy,
										sound_propagator_emitter const& emitter,
										sound_producer const* const producer,
										sound_receiver const* const	ignorable_receiver ) :
	m_propagating_time					( 0 ),
	m_proxy								( proxy ),
	m_emitter							( emitter ),
	m_is_on_finish_playing_called		( false ),
	m_is_propagation_finished			( false ),
	m_playing_offset					( playing_offset ),
	m_playing_offset_original			( playing_offset ),
	m_sound_playing_time				( playing_offset ),
	m_end_propagation_distance			( emitter.get_sound_spl()->get_unheard_distance() ),
	m_producer							( producer ),
	m_ignorable_receiver				( ignorable_receiver ),
	m_next_for_proxies					( 0 ),
	m_sound_voice						( 0 ),
	m_tick_precision_for_listener		( 0 ),
	m_is_callbak_executer				( false ),
	m_is_pause_producing_sound_requested( false ),
	m_is_pause_propagating_requested	( false ),
	m_is_stop_producing_sound_requested	( false ),
	m_is_state_preserved_for_resume		( false ),
	m_is_sound_producing				( true ),
	m_mode								( mode ),
	m_listener_position					( proxy.get_sound_scene().get_listenet_position() ),
	m_before_playing_offset				( before_playing_offset ),
	m_after_playing_offset				( after_playing_offset ),
	m_perceived_loudness				( 0.0f ),
	m_attenuated_loudness				( 0.0f ),
	m_dbg_id							( prop_id++ )
{
	float3 position;
	if ( m_proxy.get_sound_type() == volumetric )
		position						= m_proxy.get_volumetric_position( m_proxy.get_sound_scene().get_listenet_position() );
	else
		position						= m_proxy.get_position( );

	m_sound_length						= (u32)(emitter.get_quality_for_resource( )->get_length_in_msec( )) + after_playing_offset + before_playing_offset - playing_offset;
	m_sound_length_original				= (u32)(emitter.get_quality_for_resource( )->get_length_in_msec( )) + after_playing_offset + before_playing_offset; 
	m_start_propagation_position		= position;
	m_end_propagation_time				= (u32)(emitter.get_sound_spl()->get_unheard_distance() / get_world(proxy)->get_speed_of_sound() + (u32)emitter.get_quality_for_resource( )->get_length_in_msec( )) + after_playing_offset + before_playing_offset - playing_offset;


	if ( m_mode == looped )
	{
		m_sound_length				= 0xffffffff;
		m_end_propagation_time		= 0xffffffff;// end_propagation_distance / get_world(proxy)->get_speed_of_sound() + sound_length;

//		m_sound_length				( (u32)(m_emitter.get_quality_for_resource( proxy, position )->get_length_in_msec( )) + after_playing_offset + before_playing_offset - playing_offset ),
		m_sound_length_original		= ( (u32)(m_emitter.get_quality_for_resource( )->get_length_in_msec( )) + after_playing_offset + before_playing_offset ); 

		m_playing_offset			= playing_offset % m_sound_length_original;
	}

	LOG_DEBUG						( "sound_propagator id: %d, address: 0x%8x", m_dbg_id, this );
	LOG_DEBUG						( "emitter address: 0x%8x", &emitter );
	LOG_DEBUG						( "sound_length: %d", m_sound_length );
	LOG_DEBUG						( "sound_length_original: %d", m_sound_length_original );
	//LOG_DEBUG						( "get_length_in_msec: %d", (u32)(proxy.get_sound_propagator_emitter( ).get_quality_for_resource( proxy )->get_length_in_msec( )));
	LOG_DEBUG						( "after_playing_offset: %d", after_playing_offset );
	LOG_DEBUG						( "before_playing_offset: %d", before_playing_offset );
	LOG_DEBUG						( "playing_offset: %d", playing_offset );
	LOG_DEBUG						( "playing_offset: %d", playing_offset );
}

sound_propagator::~sound_propagator		( )
{
	if ( m_sound_voice )
		detach_sound_voice				( );

	LOG_DEBUG						( "~sound_propagator" );
}

void sound_propagator::tick			( u32 time_delta_in_msec )
{
	if ( m_mode == once )
		tick_for_once_mode			( time_delta_in_msec );
	else if ( m_mode == looped )
		tick_for_looped_mode		( time_delta_in_msec );
}

void sound_propagator::tick_for_once_mode	( u32 time_delta_in_msec )
{
//	LOG_DEBUG								( "time_delta_in_msec: %d", time_delta_in_msec );
	if ( !m_is_propagation_finished && !m_is_pause_propagating_requested )
	{
		if ( !m_is_pause_producing_sound_requested && !m_is_stop_producing_sound_requested )
		{
			m_playing_offset		+= time_delta_in_msec;
		}

		m_propagating_time		+= time_delta_in_msec;

		if ( !m_is_on_finish_playing_called && m_playing_offset >= m_sound_length_original )
		{
			m_is_sound_producing		= false;
			on_finish_playing_sound		( );
		}

		if ( m_propagating_time > m_end_propagation_time )
			on_finish_propagating		( );
	}
}


void sound_propagator::tick_for_looped_mode	( u32 time_delta_in_msec )
{
	if ( !m_is_propagation_finished && !m_is_pause_propagating_requested )
	{
		if ( !m_is_pause_producing_sound_requested && !m_is_stop_producing_sound_requested )
			m_playing_offset		+= time_delta_in_msec;

		m_propagating_time			+= time_delta_in_msec;
//		LOG_DEBUG					( "m_playing_offset: %d", m_playing_offset );

		if ( !m_is_on_finish_playing_called && m_playing_offset >= m_sound_length_original )
		{
			m_is_sound_producing		= false;
			on_finish_playing_sound		( );
		}

		if ( m_propagating_time > m_end_propagation_time )
			on_finish_propagating		( );
	}
}

void sound_propagator::on_finish_propagating	( )
{
	LOG_DEBUG									( "on_finish_propagating" );
	m_is_propagation_finished					= true;
	if ( m_proxy.is_playing_once( ) && is_callback_executer( ) )
		execute_user_callback					( );
}

void sound_propagator::on_finish_playing_sound	( )
{
	LOG_DEBUG								( "sound_propagator::on_finish_playing_sound" );
	LOG_DEBUG								( "pos: %f, %f, %f", m_start_propagation_position[0], m_start_propagation_position[1], m_start_propagation_position[2] );

	m_is_on_finish_playing_called			= true;

	if ( m_mode == once )
	{
		if ( !m_proxy.is_playing_once( ) && is_callback_executer( ) && m_mode == once )
			execute_user_callback					( );
	}
	else
	{
//		sound_propagator* prop		= m_proxy.get_sound_scene().create_sound_propagator_for_looped( *this );
//		m_proxy.add_sound_propagator( *prop );
	}

}

void sound_propagator::stop_playing_sound	( )
{
	m_is_propagation_finished				= true;
}

void sound_propagator::execute_user_callback	( ) const
{
	m_proxy.set_callback_pending			( true );
	functor_response* response				= XRAY_NEW_IMPL ( m_proxy.get_world_user().get_channel().responses.owner_allocator(), functor_response )
		( boost::bind( &sound_instance_proxy_internal::execute_callback, boost::ref( m_proxy ) ) );
	m_proxy.get_world_user().add_response	( response );
}

float sound_propagator::get_propagation_outer_radius	( ) const
{

//	float r		= m_mode == once ? 
//		get_outer_radius_internal( ) :
//		m_propagating_time * get_world( m_proxy )->get_speed_of_sound( );

	return math::min ( get_outer_radius_internal( ), m_end_propagation_distance );
}

float sound_propagator::get_propagation_inner_radius( ) const
{
	if ( m_mode == looped && m_end_propagation_time	== 0xffffffff )
		return 0.0f;
	//if ( m_mode == once )
	//{
		float out_r				= get_outer_radius_internal( );
	//	LOG_DEBUG				( "out_r: %f", out_r );
		if ( math::is_zero( out_r ) )
			return 0.0f;

		u32 sound_played			= m_playing_offset_original + m_sound_length;
		//LOG_DEBUG					( "sound_played: %d", sound_played );
		u32 sound_length_without_offset	= m_sound_length_original - m_after_playing_offset;
		//LOG_DEBUG					( "sound_length_without_offset: %d", sound_length_without_offset );
		u32 diff					= 0;
		if ( sound_played >= sound_length_without_offset )
			diff			= sound_played - sound_length_without_offset;

		u32 before_playing_offset	= 0;
		if ( m_playing_offset_original < m_before_playing_offset )
			before_playing_offset		= m_before_playing_offset - m_playing_offset_original;
	
		//LOG_DEBUG			( "before_playing_offset: %d", before_playing_offset );

		//LOG_DEBUG			( "diff: %d", diff );
		float in_r			= out_r - get_world( m_proxy )->get_speed_of_sound( ) * ( m_sound_length - diff - before_playing_offset);
		//LOG_DEBUG			( "in_r: %f", in_r );
		return math::min ( math::max( in_r, .0f ), math::min( out_r, m_end_propagation_distance ));
	//}
	
	//if ( m_sound_length > m_propagating_time )
	//	return 0.0f;

	//return ( m_propagating_time - m_sound_length ) * get_world( m_proxy )->get_speed_of_sound( );
}

float sound_propagator::get_propagation_outer_radius_for_listener	( ) const
{
	return get_propagation_outer_radius				( );
//	if ( m_propagating_time < m_before_playing_offset )
//		return 0.0f;

//	return ( m_propagating_time - m_before_playing_offset ) * get_world( m_proxy )->get_speed_of_sound( );
//	float r = m_propagating_time * get_world( m_proxy )->get_speed_of_sound( );
//	return math::min ( r, m_end_propagation_distance );
}

float sound_propagator::get_propagation_inner_radius_for_listener	( ) const
{
	//if ( m_mode == once )
	//{
	//	if ( m_propagating_time < m_sound_length - m_after_playing_offset )
	//		return 0.0f;
	//	return ( m_propagating_time - m_sound_length + m_after_playing_offset ) * get_world( m_proxy )->get_speed_of_sound( );
	//}
	//else if ( m_mode == looped )
	//{
	//	if ( m_propagating_time <= m_sound_length - m_after_playing_offset )
	//		return 0.0f;
	//	return ( m_propagating_time - m_sound_length + m_after_playing_offset ) * get_world( m_proxy )->get_speed_of_sound( );
	//}

	return get_propagation_inner_radius				( );
}

void sound_propagator::attach_sound_voice	( u32 delta_time_from_last_tick )
{
	XRAY_UNREFERENCED_PARAMETER			( delta_time_from_last_tick );
	R_ASSERT							( m_sound_voice == 0 );

	float3 position;
	if ( m_proxy.get_sound_type() == volumetric )
		position				= m_proxy.get_volumetric_position( get_listener_position() );
	else
		position				= m_proxy.get_position( );

	float dist_to_list					= length( position - get_listener_position( ));
	float out_r							= get_outer_radius_internal( );

	s32 offset							= (s32)(( out_r - dist_to_list ) / get_world( m_proxy )->get_speed_of_sound( ));
	offset	= offset < 0 ? 0 : offset;

	if ( m_mode == looped )
		offset = offset % m_sound_length_original;

	if ( m_playing_offset_original > m_before_playing_offset )
		offset	+= ( m_playing_offset_original - m_before_playing_offset );

	LOG_DEBUG							( "offset: %d", offset );

	u32 new_end_propagation_time		= (u32)( dist_to_list / get_world( m_proxy )->get_speed_of_sound( ) + m_sound_length ); 
	if ( new_end_propagation_time > m_end_propagation_time )
		m_end_propagation_time			= new_end_propagation_time;


	m_sound_voice						= get_world( m_proxy )->create_sound_voice( m_proxy.get_sound_scene(), offset, m_before_playing_offset, m_after_playing_offset, &m_proxy, m_emitter, m_emitter.get_sound_spl( ) );
	if ( m_sound_voice == 0 )
		return;

	m_proxy.get_sound_scene( ).calculate_3d_sound( *m_sound_voice, get_world( m_proxy )->get_panning_lut( ) );

	if ( m_sound_voice )
		m_sound_voice->play					( m_mode );
}

void sound_propagator::detach_sound_voice	( )
{
	R_ASSERT								( m_sound_voice != 0 );
	m_sound_voice->stop						( );
	LOG_DEBUG								( "voice detached" );
	get_world( m_proxy )->delete_sound_voice( m_proxy.get_sound_scene(), m_sound_voice );
	m_sound_voice							= 0;
}


float sound_propagator::get_unheard_distance( ) const
{ 
	return m_emitter.get_sound_spl( )->get_unheard_distance( );
}

void sound_propagator::pause_produce_and_preserve_state_for_resume	( )
{
	R_ASSERT								( is_sound_producing( ) );
	R_ASSERT								( !m_is_propagation_finished );
	R_ASSERT								( !m_is_pause_producing_sound_requested );
	R_ASSERT								( !m_is_pause_propagating_requested );
	R_ASSERT								( !m_is_stop_producing_sound_requested );
	R_ASSERT								( !m_is_state_preserved_for_resume );

	m_is_pause_producing_sound_requested	= true;
	m_is_state_preserved_for_resume			= true;
	m_is_sound_producing					= false;

	m_sound_length							= m_propagating_time;
	m_end_propagation_time					= (u32)(m_end_propagation_distance / get_world(m_proxy)->get_speed_of_sound()) + m_sound_length;
	LOG_DEBUG								( "m_end_propagation_time: %d", m_end_propagation_time );
//	m_playing_offset						= m_propagating_time + m_playing_offset_original;
}

void sound_propagator::clear_preservation_state_for_resume	( )
{
	R_ASSERT								( m_is_pause_producing_sound_requested );
	R_ASSERT								( !m_is_pause_propagating_requested );
	R_ASSERT								( !m_is_stop_producing_sound_requested );
	R_ASSERT								( m_is_state_preserved_for_resume );

	m_is_state_preserved_for_resume			= false;
	//m_is_pause_producing_sound_requested	= false;
}

void sound_propagator::pause_propagation	( )
{
	R_ASSERT								( !m_is_pause_propagating_requested );
	R_ASSERT								( !m_is_stop_producing_sound_requested );
	R_ASSERT								( !m_is_propagation_finished );
	
	m_is_pause_propagating_requested		= true;

	if ( has_sound_voice( ) )
		detach_sound_voice					( );

}

void sound_propagator::resume_propagation	( )
{
	R_ASSERT								( m_is_pause_propagating_requested );
	R_ASSERT								( !m_is_propagation_finished );
	R_ASSERT								( !m_is_stop_producing_sound_requested );

	m_is_pause_propagating_requested		= false;
}

void sound_propagator::stop_produce_sound	( )
{
	R_ASSERT								( is_sound_producing( ) );
	R_ASSERT								( !m_is_stop_producing_sound_requested );

	m_is_pause_producing_sound_requested	= false;
	m_is_pause_propagating_requested		= false;
	m_is_stop_producing_sound_requested		= true;
	m_is_sound_producing					= false;
	m_is_state_preserved_for_resume			= false;

	m_sound_length							= m_propagating_time;
	m_end_propagation_time					= (u32)(m_end_propagation_distance / get_world(m_proxy)->get_speed_of_sound() + m_sound_length);
}

void sound_propagator::stop_propagate_sound	( )
{
	LOG_DEBUG								( "stop_propagate_sound	( )" );

	m_is_propagation_finished				= true;
	m_is_pause_producing_sound_requested	= false;
	m_is_pause_propagating_requested		= false;
	m_is_stop_producing_sound_requested		= false;
	m_is_state_preserved_for_resume			= false;

	if ( m_proxy.is_playing_once( ) && is_callback_executer( ) )
		execute_user_callback				( );

	if ( has_sound_voice( ) )
		detach_sound_voice					( );
}

float sound_propagator::get_sound_spl		( float distance ) const
{
	return m_emitter.get_sound_spl( )->get_spl( distance );
}

float sound_propagator::get_sound_rms_value	( ) const
{
//	LOG_DEBUG							("prop m_dbg_id: %d", m_dbg_id );
	float dist_to_list					= length( m_proxy.get_position( ) - get_listener_position( ));
//	LOG_DEBUG							( "dist_to_list: %f", dist_to_list );
	u32 time_to_list					= (u32)(dist_to_list / get_world( m_proxy )->get_speed_of_sound( ));
//	LOG_DEBUG							( "time_to_list: %d", time_to_list );
	float rms_val						= 0.0f;

	u32 prop_time						= m_propagating_time;


	if ( m_mode == once )
	{
		if ( prop_time < time_to_list )
			return 0.0f;

		u32 offset	= prop_time - time_to_list + m_playing_offset_original - m_before_playing_offset;
		if ( offset > m_sound_length_original - m_after_playing_offset - m_before_playing_offset )
			return 0.0f;

		sound_rms_pinned pinned_rms		( m_emitter.get_sound_rms( ) );
		rms_val							= pinned_rms->get_rms_by_time ( offset );
	}
	else
	{
		if ( prop_time < time_to_list )
			return 0.0f;

		prop_time %= m_sound_length_original;
		u32 offset	= prop_time - time_to_list + m_playing_offset_original - m_before_playing_offset;
		if ( offset > m_sound_length_original - m_after_playing_offset - m_before_playing_offset )
			return 0.0f;

		sound_rms_pinned pinned_rms		( m_emitter.get_sound_rms( ) );
		rms_val							= pinned_rms->get_rms_by_time ( offset );
	}

	////LOG_DEBUG							( "time_to_list: %f", time_to_list );
	////u32	time							= ( m_sound_playing_time + (u32)time_to_list ) % m_sound_length_original;
	////LOG_DEBUG							( "time: %d", time );
	////if ( time < m_before_playing_offset || time > m_sound_length_original - m_after_playing_offset )
	////	return 0.0f;

	//sound_rms_pinned pinned_rms			( m_sound_rms );
	//float rms_val						= pinned_rms->get_rms_by_time ( time - m_before_playing_offset );
	//LOG_DEBUG							( "rms val: %f", rms_val );
	return rms_val;
}

sound_rms_ptr const& sound_propagator::get_sound_rms	( ) const
{ 
	return m_emitter.get_sound_rms( ); 
}

float sound_propagator::get_outer_radius_internal	( ) const
{
	if ( m_mode == looped )
		return m_propagating_time * get_world( m_proxy )->get_speed_of_sound( );

	u32 heard_prop_time				= m_propagating_time;
	u32 before_playing_offset		= 0;		

	if ( m_playing_offset_original > m_sound_length_original - m_after_playing_offset )
		return 0.0f;

	if ( m_before_playing_offset > m_playing_offset_original )
	{
		before_playing_offset = m_before_playing_offset - m_playing_offset_original;
		if ( m_sound_length < before_playing_offset )
			return 0.0f;
	}

	if ( m_propagating_time < before_playing_offset )
		return 0.0f;

	heard_prop_time			-= before_playing_offset;

	return ( heard_prop_time ) * get_world( m_proxy )->get_speed_of_sound( );
}

void sound_propagator::serialize	( memory::writer& w ) const
{
	w.write_u64						( (u64)&m_emitter ); 

	w.write_float3					( m_start_propagation_position );
	w.write_float3					( m_listener_position );

	w.write_float					( m_end_propagation_distance );

	w.write_u32						( m_end_propagation_time );
	w.write_u32						( m_propagating_time );
	w.write_u32						( m_sound_length );
	w.write_u32						( m_sound_length_original );
	w.write_u32						( m_sound_playing_time );
	w.write_u32						( m_playing_offset );
	w.write_u32						( m_playing_offset_original );
	w.write_u32						( m_before_playing_offset );
	w.write_u32						( m_after_playing_offset );
	w.write_u32						( m_tick_precision_for_listener );

	w.write							( &m_mode, sizeof ( playback_mode ) );
	w.write							( &m_is_on_finish_playing_called, sizeof( bool ) );
	w.write							( &m_is_propagation_finished, sizeof( bool ) );
	w.write							( &m_is_callbak_executer, sizeof( bool ) );
	w.write							( &m_is_pause_producing_sound_requested, sizeof( bool ) );
	w.write							( &m_is_pause_propagating_requested, sizeof( bool ) );
	w.write							( &m_is_stop_producing_sound_requested, sizeof( bool ) );
	w.write							( &m_is_state_preserved_for_resume, sizeof( bool ) );
	w.write							( &m_is_sound_producing, sizeof( bool ) );
}

void sound_propagator::deserialize	( memory::reader& r )
{
	m_start_propagation_position	= r.r_float3( );
	m_listener_position				= r.r_float3( );


	m_end_propagation_distance		= r.r_float( );

	m_end_propagation_time			= r.r_u32( );
	m_propagating_time				= r.r_u32( );
	m_sound_length					= r.r_u32( );
	m_sound_length_original			= r.r_u32( );
	m_sound_playing_time			= r.r_u32( );
	m_playing_offset				= r.r_u32( );
	m_playing_offset_original		= r.r_u32( );
	m_before_playing_offset			= r.r_u32( );
	m_after_playing_offset			= r.r_u32( );
	m_tick_precision_for_listener	= r.r_u32( );

	r.r							( &m_mode, sizeof ( playback_mode ), sizeof ( playback_mode ));
	r.r							( &m_is_on_finish_playing_called, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_propagation_finished, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_callbak_executer, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_pause_producing_sound_requested, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_pause_propagating_requested, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_stop_producing_sound_requested, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_state_preserved_for_resume, sizeof( bool ), sizeof( bool ) );
	r.r							( &m_is_sound_producing, sizeof( bool ), sizeof( bool ) );
}


void sound_propagator::set_quality	( u32 quality )
{
	if ( m_sound_voice )
		m_sound_voice->set_quality( quality );
}

#ifndef MASTER_GOLD
void sound_propagator::dump_debug_snapshot	( configs::lua_config_value& val ) const
{
	encoded_sound_child_ptr const& res	= get_propagator_emitter( ).get_quality_for_resource( );
	val["filename"]						= res->reusable_request_name( ).c_str( );
	val["length"]						= get_sound_length_original( );
	val["current_playing_time"]			= ( get_playing_offset( ) % get_sound_length_original( ) );
}

void sound_propagator::fill_statistic		( propagator_statistic& statistic ) const
{
	encoded_sound_child_ptr const& res	= get_propagator_emitter( ).get_quality_for_resource( );
	statistic.m_filename				= res->reusable_request_name( ).c_str( );
	statistic.m_length					= get_sound_length_original( );
	statistic.m_current_playing_time	= ( get_playing_offset( ) % get_sound_length_original( ) );
	statistic.m_rms						= get_sound_rms( );
	statistic.m_playback_mode			= m_mode;
}
#endif // #ifndef MASTER_GOLD

} // namespace sound
} // namespace xray
