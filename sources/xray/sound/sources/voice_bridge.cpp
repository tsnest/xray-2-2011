////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "voice_bridge.h"
#include "sound_buffer.h"
#include "sound_voice.h"

namespace xray {
namespace sound {

enum
{
	default_bits_per_sample = 16,
	default_sample_rate		= 44100,
};

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
static WORD channels_type_to_xaudio_channels ( channels_type type )
{
	switch ( type )
	{
	case mono:		return 1;
	case stereo:	return 2;
	default:		NODEFAULT( return channels_type_count );
	}
}
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
static channels_type xaudio_channels_to_channels_type ( u32 channels )
{
	switch ( channels )
	{
	case 1:		return mono;
	case 2:		return stereo;
	default:	NODEFAULT( return channels_type_count );
	}
}
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360


voice_bridge::voice_bridge			( creation_parametrs& params ) : 
	m_next				( 0 ),
	m_handler			( 0 ),
	m_source_voice		( 0 ),
	m_master_channels	( params.master_channels_type )
{
	R_ASSERT_U						( params.type < channels_type_count );
	R_ASSERT_U						( params.xaudio_engine );
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	WAVEFORMATEX wfx_standard		= { 0 };
	wfx_standard.wFormatTag			= WAVE_FORMAT_PCM;
    wfx_standard.nSamplesPerSec		= default_sample_rate;
    wfx_standard.wBitsPerSample		= default_bits_per_sample;
    wfx_standard.cbSize				= 0;
	wfx_standard.nChannels			= channels_type_to_xaudio_channels(params.type);
	wfx_standard.nBlockAlign		= wfx_standard.nChannels * ( wfx_standard.wBitsPerSample >> 3 ); 
	wfx_standard.nAvgBytesPerSec	= wfx_standard.nSamplesPerSec * wfx_standard.nBlockAlign;
	
	HRESULT hr						= params.xaudio_engine->CreateSourceVoice	( 
																&m_source_voice,
																&wfx_standard,
																XAUDIO2_VOICE_USEFILTER,
																params.max_frequency_ratio,
																this,
																NULL,
																NULL );
	R_ASSERT_U				(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

}

voice_bridge::~voice_bridge( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_source_voice->DestroyVoice( );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	LOG_INFO("voice_bridge destroyed");
}

void voice_bridge::set_handler ( voice_callback_handler* handler )
{
	m_handler = handler;
}

channels_type voice_bridge::get_channels_type ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_DETAILS voice_details;
	m_source_voice->GetVoiceDetails(&voice_details);
	return xaudio_channels_to_channels_type( voice_details.InputChannels );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( );
	return channels_type_count;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

u32 voice_bridge::get_sample_rate ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_DETAILS voice_details;
	m_source_voice->GetVoiceDetails(&voice_details);
	return voice_details.InputSampleRate;
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::set_sample_rate	( u32 new_sample_rate )
{
	R_ASSERT_U			( buffers_queued() == 0 );
	if (get_sample_rate() != new_sample_rate)
	{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		HRESULT hr		= m_source_voice->SetSourceSampleRate( new_sample_rate );
		R_ASSERT_U		(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		NOT_IMPLEMENTED ( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	}
}

void voice_bridge::set_output_voice		( IXAudio2SubmixVoice* output_voice  )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	if ( output_voice )
	{
		XAUDIO2_SEND_DESCRIPTOR desc;
		desc.Flags			= 0;
		desc.pOutputVoice	= output_voice;

		XAUDIO2_VOICE_SENDS sends;
		sends.SendCount		= 1;
		sends.pSends		= &desc;

		m_source_voice->SetOutputVoices	( &sends );
	}
	else
		m_source_voice->SetOutputVoices	( 0 );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED			( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::start ( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	HRESULT const result	= m_source_voice->Start	(0);
	R_ASSERT_U				( !FAILED(result) );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED			( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::stop ( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	HRESULT const result	= m_source_voice->Stop	();
	R_ASSERT_U				( !FAILED(result) );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED			( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::submit_source_buffer	( sound_buffer* buffer, u32 playing_offset, u32 playing_length )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	XAUDIO2_BUFFER* xaudio_buffer	= buffer->get_xaudio_buffer( );

	if ( playing_offset )
	{

		R_ASSERT					( playing_offset <= buffer->get_length_in_pcm( ) );

		xaudio_buffer->PlayBegin	= playing_offset;
		xaudio_buffer->PlayLength	= buffer->get_length_in_pcm() - playing_offset;
	}
	else
	{
		xaudio_buffer->PlayBegin	= 0; 
		xaudio_buffer->PlayLength	= playing_length;

	}

	HRESULT hr		= m_source_voice->SubmitSourceBuffer( xaudio_buffer );
	R_ASSERT_U		(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

//void voice_bridge::submit_source_buffer	( XAUDIO2_BUFFER& buf )
//{
//	HRESULT hr		= m_source_voice->SubmitSourceBuffer( &buf  );
//	R_ASSERT_U		(!FAILED(hr));
//}

void voice_bridge::flush_source_buffers ( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	HRESULT hr		= m_source_voice->FlushSourceBuffers( );
	R_ASSERT_U		(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

}

u32 voice_bridge::buffers_queued ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_STATE			vstate;
	m_source_voice->GetState	( &vstate );
	return						vstate.BuffersQueued;
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

u64	voice_bridge::samples_played	( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_STATE			vstate;
	m_source_voice->GetState	( &vstate );
	return						vstate.SamplesPlayed;
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}


sound_buffer* voice_bridge::current_sound_buffer ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_STATE			vstate;
	m_source_voice->GetState	( &vstate );
	return static_cast_checked<sound_buffer*>( vstate.pCurrentBufferContext );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::set_output_matrix ( float const* level_matrix )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_DETAILS voice_details;
	m_source_voice->GetVoiceDetails( &voice_details );
	m_source_voice->SetOutputMatrix( NULL, voice_details.InputChannels, channels_type_to_xaudio_channels( m_master_channels ), level_matrix);
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::set_channel_volumes	( channels_type type, float const* level_matrix )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_source_voice->SetChannelVolumes( channels_type_to_xaudio_channels( type ), level_matrix ); 
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::set_low_pass_filter_params		( float coeff )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_FILTER_PARAMETERS params;
	params.Type				= LowPassFilter;
	params.Frequency		= coeff;
	params.OneOverQ			= 1.0f;

	HRESULT hr				= m_source_voice->SetFilterParameters( &params );
	R_ASSERT_U				(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void voice_bridge::set_frequency_ratio				( float ratio )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	R_ASSERT_U				( !( ratio < 0.0f ) );
	R_ASSERT_U				( m_source_voice );
	HRESULT hr				= m_source_voice->SetFrequencyRatio( ratio );
	R_ASSERT_U				(!FAILED(hr));
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

u8 voice_bridge::get_bytes_per_second( ) const
{
	return default_bits_per_sample >> 3;
}

void voice_bridge::OnVoiceProcessingPassStart	( UINT32 BytesRequired )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_voice_processing_pass_start( BytesRequired );
}

void voice_bridge::OnVoiceProcessingPassEnd	( )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_voice_processing_pass_end( );

}

void voice_bridge::OnStreamEnd	( )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_stream_end	( );
}

void voice_bridge::OnBufferStart( void* pBufferContext )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_buffer_start	( pBufferContext );
}

void voice_bridge::OnBufferEnd	( void* pBufferContext )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_buffer_end	( pBufferContext );
}

void voice_bridge::OnLoopEnd	( void* pBufferContext )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_loop_end		( pBufferContext );
}

void voice_bridge::OnVoiceError	( void* pBufferContext, HRESULT Error )
{
	R_ASSERT_U					(m_handler);
	m_handler->on_voice_error	( pBufferContext, Error );
}

} // namespace sound
} // namespace xray
