////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_voice.h"
#include <xray/sound/sound_instance_proxy.h>
#include <xray/sound/encoded_sound_interface.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/sound_rms.h>
#include <xray/sound/world_user.h>
#include <xray/sound/single_sound.h>
#include "sound_world.h"
#include "voice_bridge.h"
#include "sound_buffer.h"
#include "sound_voice.h"

namespace xray {
namespace sound {

enum
{
	max_queued_buffers = 3,
};

static u32 time_in_msec_to_pcm	( encoded_sound_ptr const& encoded_sound, u32 time )
{
	return	( time * (encoded_sound->get_samples_per_sec( ))) / 1000;
}

static u32 sound_voice_id		= 0;

sound_voice::sound_voice	(	s32 playing_offset_in_msec,
								u32 before_playing_quiet,
								u32 after_playing_quiet,
								sound_instance_proxy_internal& proxy,
								sound_propagator_emitter const& emitter,
								sound_spl_ptr const& spl ) :
	m_proxy					( proxy ),
	m_emitter				( emitter ),
	m_sound_spl				( spl ),
	m_is_playing			( false ),
	m_is_quality_changing_requested	( false ),
	m_is_conversion_buffer_ready	( false ),
	m_stream_cursor_pcm		( 0 ),
	m_buffer_playing_offset	( 0 ),
	m_next_for_delete		( 0 ),
	m_next_for_active		( 0 ),
	m_playing_offset		( playing_offset_in_msec ),
	m_buffers_queued		( 0 ),
	m_mode					( once ),
	m_before_playing_quiet	( before_playing_quiet ),
	m_after_playing_quiet	( after_playing_quiet ),
	m_dbg_id				( sound_voice_id++ ),
	m_conversion_buffer		( 0 ),
	m_is_conversion_buffer_submited( false ),
	m_conv_state			( conversion_not_requested ),
	m_current_quality		( 2 ),
	m_target_sound_quality	( 0 ),
	m_world_user			( proxy.get_world_user( ) )
{
	m_current_sound_quality	= emitter.dbg_get_encoded_sound( 2 );

	m_channels_type			= m_current_sound_quality->get_channels_type	( );
	u32 samples_per_sec	= m_current_sound_quality->get_samples_per_sec( );

	LOG_DEBUG			( "sound_voice id: %d", m_dbg_id );			
	LOG_DEBUG			( "sound_length in msec %d:", m_current_sound_quality->get_length_in_msec() );
	
	m_voice				= m_world_user.get_sound_world()->get_voice( this, proxy.get_sound_scene().get_submix_voice(), m_channels_type, samples_per_sec );
}

sound_voice::~sound_voice	( )
{
	LOG_DEBUG						("sound_voice::~sound_voice(): %d", m_dbg_id );
//	m_voice->stop					( );
//	m_voice->flush_source_buffers	( );
	R_ASSERT						( m_buffers_queued == 0 );
	m_world_user.get_sound_world()->free_voice( m_voice );
}


void sound_voice::play		( playback_mode mode )
{
	LOG_DEBUG				("sound_voice::play start");

	m_mode					= mode;

	if ( m_mode == once )
	{
		if ( m_playing_offset > 10  )
		{
			m_stream_cursor_pcm				= find_nearest_adjective_pcm_offset ( );
			LOG_DEBUG						( "m_stream_cursor_pcm: %d", m_stream_cursor_pcm );
			m_buffer_playing_offset			= time_in_msec_to_pcm( m_current_sound_quality, m_playing_offset ) - find_nearest_adjective_pcm_offset( );
			LOG_DEBUG						( "m_buffer_playing_offset: %d", m_buffer_playing_offset );
		}
		else
		{
			m_stream_cursor_pcm				= 0;
			m_buffer_playing_offset			= 0;
		}
	}
	else if ( m_mode == looped )
	{
		if ( m_playing_offset < (s32)m_before_playing_quiet )
		{
			submit_muted_buffers		( m_before_playing_quiet - m_playing_offset );
		}
		else if ( m_playing_offset < m_before_playing_quiet + m_current_sound_quality->get_length_in_msec() )
		{
			m_playing_offset				-= m_before_playing_quiet;
			m_stream_cursor_pcm				= find_nearest_adjective_pcm_offset ( );
			m_buffer_playing_offset			= time_in_msec_to_pcm( m_current_sound_quality, m_playing_offset ) - find_nearest_adjective_pcm_offset( );
		}
		else
		{
			submit_muted_buffers		( m_before_playing_quiet * 2 + m_after_playing_quiet + (u32)m_current_sound_quality->get_length_in_msec() - m_playing_offset );
		}
	}

	LOG_DEBUG			( "sound_voice() m_stream_cursor_pcm: %d", m_stream_cursor_pcm );
	LOG_DEBUG			( "sound_voice() m_buffer_playing_offset: %d", m_buffer_playing_offset );

	m_is_playing								= true;
	refill_buffers								( );
	m_voice->start								( );
	LOG_DEBUG									("sound_voice::play end");
}

void sound_voice::stop		( )
{
	LOG_DEBUG						("sound_voice::stop start");
	m_is_playing					= false;
	m_voice->stop					( );
	m_voice->flush_source_buffers	( );

	LOG_DEBUG						("sound_voice::stop end");
}

void sound_voice::refill_buffers	( )
{
//	LOG_DEBUG						( "sound_voice::refill_buffers" );
	//u32 buffers_queued				= m_voice->buffers_queued( );
	u64 pcm_total					= m_current_sound_quality->get_length_in_pcm();

	while ( ( m_buffers_queued  < max_queued_buffers ) && ( m_stream_cursor_pcm < pcm_total ) )
	{
//		LOG_DEBUG							( "pcm_total: %d", pcm_total );
//		LOG_DEBUG							( "m_stream_cursor_pcm: %d", m_stream_cursor_pcm );
//		LOG_DEBUG							( "m_buffer_playing_offset: %d", m_buffer_playing_offset );
//		LOG_DEBUG							( "buffers_queued: %d", buffers_queued );

		sound_buffer* buffer = m_world_user.get_sound_world()->get_sound_buffer( m_current_sound_quality, m_stream_cursor_pcm, m_stream_cursor_pcm );
		m_voice->submit_source_buffer	( buffer, m_buffer_playing_offset );
		if ( m_buffer_playing_offset )
			 m_buffer_playing_offset = 0;
		if ( m_mode == looped && pcm_total == m_stream_cursor_pcm )
		{
			submit_muted_buffers		( m_before_playing_quiet + m_after_playing_quiet );
			m_stream_cursor_pcm			= 0;
		}

//		++buffers_queued;
		threading::interlocked_increment( m_buffers_queued );
//		++m_buffers_queued;
	}

}

//void sound_voice::sumbmit_muted_sound_buffers_to_start	( )
//{
//	LOG_DEBUG							( "sumbmit_muted_sound_buffers_to_start( )" );
//	R_ASSERT							( m_playing_offset < 0 );
//	u32 playing_offset					= m_playing_offset * -1;
//	u32 samples_in_buffer				= sound_buffer_size / m_encoded_sound->get_bytes_per_sample				( );
//	u32 buffer_length_in_msec			= ( samples_in_buffer * 1000 ) / m_encoded_sound->get_samples_per_sec	( );
//	u32 mute_buffers_count				=  playing_offset / buffer_length_in_msec;
//
//	u32 mute_buffer_playing_offset		= 0;
//	if ( mute_buffers_count == 0 )
//		mute_buffer_playing_offset		= buffer_length_in_msec - playing_offset;
//	else
//		mute_buffer_playing_offset		= buffer_length_in_msec * mute_buffers_count - playing_offset;
//
//	if ( mute_buffer_playing_offset != 0 )
//		++mute_buffers_count;
//
//	for ( u32 i = 0; i < mute_buffers_count; ++i )
//	{
//		sound_buffer* mute_buff			= m_proxy.get_world_user().get_sound_world()->get_mute_sound_buffer( m_encoded_sound );
//		u32 buf_offset					= i == 0 ? mute_buffer_playing_offset : 0;
//		m_voice->submit_source_buffer	( mute_buff, buf_offset );
//	}
//}

void sound_voice::sumbmit_muted_sound_buffers_on_first_playing	( )
{
	//LOG_DEBUG							( "sumbmit_muted_sound_buffers_to_start( )" );
	//s32 playing_offset					= -1 * m_playing_offset;
	//if ( playing_offset > 0 )
	//{
	//	u32 samples_in_buffer				= sound_buffer_size / m_emitter.get_quality_for_resource( m_proxy, m_position )->get_bytes_per_sample				( );
	//	u32 buffer_length_in_msec			= ( samples_in_buffer * 1000 ) / m_emitter.get_quality_for_resource( m_proxy, m_position )->get_samples_per_sec	( );
	//	u32 mute_buffers_count				=  playing_offset / buffer_length_in_msec;

	//	u32 mute_buffer_playing_offset		= 0;
	//	if ( mute_buffers_count == 0 )
	//		mute_buffer_playing_offset		= buffer_length_in_msec - playing_offset;
	//	else
	//		mute_buffer_playing_offset		= ( buffer_length_in_msec * mute_buffers_count ) % playing_offset;

	//	if ( mute_buffer_playing_offset != 0 )
	//		++mute_buffers_count;

	//	for ( u32 i = 0; i < mute_buffers_count; ++i )
	//	{
	//		sound_buffer* mute_buff			= m_proxy.get_world_user().get_sound_world()->get_mute_sound_buffer( m_emitter.get_quality_for_resource( m_proxy, m_position ) );
	//		u32 buf_offset					= i == 0 ? mute_buffer_playing_offset : 0;
	//		m_voice->submit_source_buffer	( mute_buff, buf_offset );
	//	}
	//}
	//else
	//{
	//	m_stream_cursor_pcm						= find_nearest_adjective_pcm_offset ( );
	//	m_buffer_playing_offset					= time_in_msec_to_pcm( m_emitter.get_quality_for_resource( m_proxy, m_position ), m_playing_offset ) - find_nearest_adjective_pcm_offset( );
	//}
}

bool sound_voice::can_be_deleted	( ) const
{
	return m_buffers_queued == 0; 
}

void sound_voice::set_output_matrix	( float const* level_matrix )
{
	R_ASSERT						( m_voice != 0 );
	if ( m_channels_type == mono )
		m_voice->set_output_matrix	( level_matrix );
	else if ( m_channels_type == stereo )
		m_voice->set_channel_volumes( m_channels_type, level_matrix );
	else
		NOT_IMPLEMENTED				( );
}

u32 sound_voice::find_nearest_adjective_pcm_offset	( ) const
{
	u32 pcm_cursor					= time_in_msec_to_pcm( m_current_sound_quality, m_playing_offset );
	LOG_DEBUG						( "find_nearest_adjective_pcm_offset" );
	LOG_DEBUG						( "pcm_cursor: %d", pcm_cursor );
	LOG_DEBUG						( "m_playing_offset: %d", m_playing_offset );
	return ( pcm_cursor / ( sound_buffer_size / m_current_sound_quality->get_bytes_per_sample( ))) * ( sound_buffer_size / m_current_sound_quality->get_bytes_per_sample( ) );
}

void sound_voice::on_finish_playing	( )
{

}

sound_scene const& sound_voice::get_sound_scene	( ) const
{
	return m_proxy.get_sound_scene( );
}

void sound_voice::set_quality	( u32 quality )
{
	LOG_DEBUG		( "set_quality requested" );

	if ( quality == m_current_quality )
		return;

	if ( m_conv_state != conversion_not_requested )
		return;

	if ( m_voice->buffers_queued( ) < 2 )
		return;

	if ( m_emitter.dbg_get_encoded_sound( quality ).c_ptr() == 0 )
	{
		LOG_ERROR		( "qality not yet loaded!" );
		return;
	}

	m_target_sound_quality		= m_emitter.dbg_get_encoded_sound( quality );
	m_current_quality			= quality;

	threading::interlocked_exchange( m_conv_state, conversion_requested );
}

void sound_voice::submit_muted_buffers	( u32 msec )
{
	if ( msec == 0 )
		return;

	u32 const bytes_per_sample	= m_current_sound_quality->get_bytes_per_sample	( );
	u32 const samples_per_sec	= m_current_sound_quality->get_samples_per_sec( ) ;
	u32 const samples_in_buffer	= sound_buffer_size / bytes_per_sample;
	u8 channels_count			= 0;
	switch ( m_current_sound_quality->get_channels_type( ) )
	{
	case mono:		channels_count	= 1; break;
	case stereo:	channels_count	= 2; break;
	default:		NODEFAULT		( );
	}
	u32 buffer_length_in_msec	= (( samples_in_buffer * 1000 ) / samples_per_sec ) / channels_count;
	u32 mute_buffers_count		= msec / buffer_length_in_msec;

	
	u32 mute_buffer_playing_offset		= 0;
	if ( mute_buffers_count == 0 )
		mute_buffer_playing_offset		= buffer_length_in_msec - msec;
	else
		mute_buffer_playing_offset		= ( buffer_length_in_msec * mute_buffers_count ) % msec;

	if ( mute_buffer_playing_offset != 0 )
		++mute_buffers_count;

	for ( u32 i = 0; i < mute_buffers_count; ++i )
	{
		sound_buffer* mute_buff			= m_proxy.get_world_user().get_sound_world()->get_mute_sound_buffer( m_current_sound_quality );
		u32 buf_offset					= i == 0 ? mute_buffer_playing_offset : 0;
		m_voice->submit_source_buffer	( mute_buff, buf_offset );
		threading::interlocked_increment( m_buffers_queued );		
	}
}

void sound_voice::set_low_pass_filter_params( float coeff )
{
	m_voice->set_low_pass_filter_params	( coeff );
}

} // namespace sound
} // namespace xray
