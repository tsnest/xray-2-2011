////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_voice.h"
#include <xray/sound/world_user.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/sound_rms.h>
#include "functor_command.h"
#include "sound_world.h"
#include "voice_bridge.h"
#include "sound_buffer.h"

namespace xray {
namespace sound {

// Called just before this voice's processing pass begins.
void sound_voice::on_voice_processing_pass_start	( u32 bytes_required )
{
	XRAY_UNREFERENCED_PARAMETERS( bytes_required );
	//LOG_DEBUG("OnVoiceProcessingPassStart: %d", bytes_required);
}

// Called just after this voice's processing pass ends.
void sound_voice::on_voice_processing_pass_end	( )
{
	//LOG_DEBUG("OnVoiceProcessingPassEnd");
}

// Called when this voice has just finished playing a buffer stream
void sound_voice::on_stream_end			( )
{
	//m_proxy.get_world_user().get_sound_world()->add_xaudio_order(
	//	MT_NEW ( functor_order ) (
	//		boost::bind( &sound_voice::on_stream_end_impl, this )
	//	)
	//);
	LOG_DEBUG( "on_stream_end xaudio order" );
}

// Called when this voice is about to start processing a new buffer.
void sound_voice::on_buffer_start		( pvoid buffer_context )
{
	if ( m_conv_state == conversion_requested )
	{
		LOG_DEBUG							( "on_buffer_start conv start" );
		threading::interlocked_exchange		( m_conv_state, conversion_buffer_preparing );

		m_voice->flush_source_buffers							( );

		m_proxy.get_world_user().get_sound_world()->add_xaudio_order(
				MT_NEW ( functor_order ) (
				boost::bind( &sound_voice::on_buffer_start_impl, this, buffer_context )
			));
	}
	else if ( m_conv_state == conversion_buffer_submited )
		threading::interlocked_exchange	( m_conv_state, conversion_not_requested );

//	XRAY_UNREFERENCED_PARAMETERS( buffer_context );
	//LOG_DEBUG("OnBufferStart: %x", buffer_context);
}

// Called when this voice has just finished processing a buffer.
// The buffer can now be reused or destroyed.
void sound_voice::on_buffer_end			( pvoid buffer_context )
{
//	threading::yield( 3000 );

//	LOG_DEBUG						( "on_buffer_end" );


	if ( m_conv_state == conversion_buffer_ready && m_voice->buffers_queued( ) == 0 )
	{
		m_voice->set_sample_rate		( m_target_sound_quality->get_samples_per_sec( ));
		
		m_voice->submit_source_buffer	( m_conversion_buffer );
		threading::interlocked_increment( m_buffers_queued );
		threading::interlocked_exchange	( m_conv_state, conversion_buffer_submited );

		m_current_sound_quality			= m_target_sound_quality;
		m_target_sound_quality			= 0;
	}

	m_world_user.get_sound_world()->add_xaudio_order(
				MT_NEW ( functor_order ) (
				boost::bind( &sound_voice::on_buffer_end_impl, this, buffer_context )
			)
		);

//	LOG_DEBUG( "on_buffer_end xaudio order with" );
}

// Called when this voice has just reached the end position of a loop.
void sound_voice::on_loop_end			( pvoid buffer_context )
{
	XRAY_UNREFERENCED_PARAMETERS( buffer_context );
	//LOG_DEBUG("OnLoopEnd: %x", buffer_context);
}

// Called in the event of a critical error during voice processing,
void sound_voice::on_voice_error		( pvoid buffer_context, HRESULT error )
{
	LOG_ERROR("OnVoiceError: %d, %x", error, buffer_context);
}

void sound_voice::on_stream_end_impl( )
{
	LOG_DEBUG				( "on_stream_end_impl" );
}


void	sound_voice::on_buffer_start_impl( pvoid pcontext )
{
	LOG_DEBUG				( "on_buffer_start_impl start" );

	R_ASSERT				( m_conv_state == conversion_buffer_preparing );

	sound_buffer* sb	= static_cast_checked<sound_buffer*>( pcontext );
	u32 new_pcm_cursor	= sb->next_sound_buffer_offset		( );

	u32 pcm_cursor_end			= new_pcm_cursor;
	u32 old_sample_rate			= m_voice->get_sample_rate( );
	u64 rms_start_time			= ( new_pcm_cursor * 1000 ) / old_sample_rate;
	u32 sound_buffer_time		= ( sound_buffer_size * ( 1000 / m_voice->get_bytes_per_second() )) / old_sample_rate;
	u64 rms_end_time			= rms_start_time + sound_buffer_time;

	if ( rms_end_time >= m_current_sound_quality->get_length_in_msec( ) )
	{
		threading::interlocked_exchange( m_conv_state, conversion_not_requested );
		return;
	}

	u64 conv_time = 0;
	{
		sound_rms_pinned rms	= m_emitter.get_sound_rms( );
		conv_time				= rms->find_min_value_time_in_interval( rms_start_time + 50, rms_end_time, 0.02f );
		if ( conv_time < rms_start_time )
			conv_time	= rms_start_time;
	}

	pcm_cursor_end					= (u32)(( conv_time * old_sample_rate ) / 1000);

	u32 old_pcm						= new_pcm_cursor;
	u32 playing_length				= pcm_cursor_end - new_pcm_cursor;

	sound_buffer* conv_sb			= 0;
	R_ASSERT						( playing_length );

	u32 samples_count	= ( old_sample_rate / 11025 ) * 256;

	float vol_coeff		= 1.0f;
	float vol_dif		= vol_coeff / (float)samples_count;

	if ( playing_length != 0 )
	{
//		LOG_DEBUG		( "playing_length: %d", playing_length );
		conv_sb			= m_world_user.get_sound_world()->get_sound_buffer( m_current_sound_quality, new_pcm_cursor, new_pcm_cursor );

		s16* pl_buf		= (s16*)&conv_sb->m_buff[( playing_length - samples_count ) * m_voice->get_bytes_per_second()];

		for ( u32 i = 0; i < samples_count; ++i, vol_coeff -= vol_dif )
			pl_buf[i] = (s16)(pl_buf[i] * vol_coeff);

		m_voice->submit_source_buffer	( conv_sb, 0, playing_length );
		threading::interlocked_increment( m_buffers_queued );
	}

	u32 new_sample_rate	= m_target_sound_quality->get_samples_per_sec( );

	new_pcm_cursor		= old_pcm + playing_length;
	if ( new_sample_rate > old_sample_rate )
		new_pcm_cursor		= new_pcm_cursor * ( new_sample_rate / old_sample_rate );
	else
		new_pcm_cursor		= new_pcm_cursor / ( old_sample_rate / new_sample_rate );

	m_stream_cursor_pcm		= new_pcm_cursor;
	m_conversion_buffer		= m_world_user.get_sound_world()->get_no_cahceable_sound_buffer( m_target_sound_quality, m_stream_cursor_pcm, m_stream_cursor_pcm );

	samples_count		= ( new_sample_rate / 11025 ) * 256;

	vol_coeff			= 0; 
	vol_dif				= 1.0f / (float)samples_count;

	s16* buf			= (s16*)(m_conversion_buffer->m_buff);

	for ( u32 i = 0; i < samples_count; ++i, vol_coeff += vol_dif )
		buf[i] =(s16)( buf[i] * vol_coeff );

	//memory::writer w1	( g_allocator );
	//w1.write			( conv_sb->m_buff, sound_buffer_size );
	//w1.save_to			( "Z:/1.raw" );

	//memory::writer w2	( g_allocator );
	//w2.write			( conv_sb->m_buff, playing_length * 2 );
	//w2.save_to			( "Z:/2.raw" );

	//memory::writer w3	( g_allocator );
	//w3.write			( m_conversion_buffer->m_buff, sound_buffer_size );
	//w3.save_to			( "Z:/3.raw" );


	threading::interlocked_exchange	( m_conv_state, conversion_buffer_ready );
}

void sound_voice::on_buffer_end_impl( void* pcontext )
{
	threading::interlocked_decrement( m_buffers_queued );

	if ( pcontext == 0 )
		return;

	if ( m_is_playing && ( m_conv_state == conversion_not_requested || m_conv_state == conversion_buffer_submited ) )
		refill_buffers		( );



	sound_buffer* sb		= static_cast_checked<sound_buffer*>(pcontext);
	m_world_user.get_sound_world()->free_sound_buffer ( sb );

	return;
}

void sound_voice::on_buffer_error_impl( void* pcontext )
{
	XRAY_UNREFERENCED_PARAMETERS( pcontext );
	LOG_DEBUG("---voice on_buffer_error");
}

} // namespace sound
} // namespace xray