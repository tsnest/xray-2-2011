////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_object.h"
#include "sound_world.h"
#include "sound_object_commands.h"


namespace xray {
namespace sound {

sound_object_impl::sound_object_impl( sound_stream_ptr stream )
:m_stream			( stream ),
m_source_voice		( NULL ),
m_stream_cursor_pcm	( 0 ),
m_thread_allocator	( NULL ),
m_playing			( false ),
m_position			( 0.0f, 0.0f, 0.0f)
{
}

sound_object_impl::~sound_object_impl( )
{
	if( m_source_voice )
		stop_impl		( );
}

void sound_object_impl::refill_buffers( )
{
#if !XRAY_PLATFORM_PS3
	R_ASSERT				(m_playing);
	ASSERT					( m_stream );
	HRESULT					res;
	XAUDIO2_VOICE_STATE		vstate;
	m_source_voice->GetState( &vstate );
	u32 pcm_total 			= m_stream->pcm_total();
	
	while( (vstate.BuffersQueued < max_queued_buffers) && ( m_stream_cursor_pcm < pcm_total) )
	{
		XAUDIO2_BUFFER			buffer = {0};

		pbyte const cache_buffer = g_sound_world->get_cache_buffer( snd_buff_size );
		buffer.pAudioData		= cache_buffer;
		buffer.AudioBytes		= m_stream->decompress( cache_buffer, m_stream_cursor_pcm, snd_buff_size );
		buffer.pContext			= cache_buffer;

		buffer.Flags			= ( m_stream_cursor_pcm==pcm_total ) ? XAUDIO2_END_OF_STREAM : 0;

		ASSERT					( buffer.AudioBytes>0 );

		res						= m_source_voice->SubmitSourceBuffer( &buffer );
		ASSERT					( !FAILED(res) );
		++vstate.BuffersQueued;
	}
#endif // #if !XRAY_PLATFORM_PS3
}

void sound_object_impl::on_end_stream( )
{
	stop_impl						( );
	LOG_INFO						("---voice destroyed");
}

void sound_object_impl::on_buffer_end( void* pcontext )
{
	g_sound_world->free_cache_buffer ( (pbyte)pcontext );
	
	if(is_playing())
		refill_buffers				( );
}

void sound_object_impl::on_buffer_error( void* pcontext)
{
	XRAY_UNREFERENCED_PARAMETERS( pcontext );
	LOG_INFO("---voice on_buffer_error");
}

void sound_object_impl::play( )
{
	playback_command* cmd = XRAY_NEW_IMPL(thread_allocator(), playback_command)(thread_allocator(), playback_command::ev_play, this);
	g_sound_world->add_end_user_command( cmd );
}

void sound_object_impl::stop( )
{
	playback_command* cmd = XRAY_NEW_IMPL(thread_allocator(), playback_command)(thread_allocator(), playback_command::ev_stop, this);
	g_sound_world->add_end_user_command( cmd );
}

void sound_object_impl::set_position( float3 const& p )
{
	XRAY_UNREFERENCED_PARAMETER	( p );
}

void sound_object_impl::play_impl( )
{
	m_stream_cursor_pcm		= 0;

	if(!m_source_voice)
	{
#if !XRAY_PLATFORM_PS3
		HRESULT res = g_sound_world->xaudio_engine()->CreateSourceVoice( 
											&m_source_voice,
											&m_stream->wave_format(), 
											0, 
											XAUDIO2_DEFAULT_FREQ_RATIO, 
											this, 
											NULL, 
											NULL
											);

		R_ASSERT_U					(!FAILED(res));
#endif // #if !XRAY_PLATFORM_PS3
	}
	m_playing				= true;
	refill_buffers			( );
#if !XRAY_PLATFORM_PS3
	HRESULT res				= m_source_voice->Start( );
	R_ASSERT_U					( !FAILED(res) );
#endif // #if !XRAY_PLATFORM_PS3

	g_sound_world->on_voice_started( this );
}

void sound_object_impl::set_position_impl( float3 const& position )
{
	m_position = position;
	g_sound_world->on_voice_position_changed( this );
}

void sound_object_impl::stop_impl( )
{
	m_playing						= false;
#if !XRAY_PLATFORM_PS3
	m_source_voice->Stop			( );
	m_source_voice->FlushSourceBuffers( );
	m_source_voice->DestroyVoice	( );
	m_source_voice					= NULL;
#endif // #if !XRAY_PLATFORM_PS3
	g_sound_world->on_voice_stopped	( this );
}

} // namespace sound
} // namespace xray
