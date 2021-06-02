////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_item.h"
#include "sound_world.h"
#include "sound_stream.h"


namespace xray {
namespace sound {

sound_item::sound_item( sound_world& w )
:m_sound_world	( w ),
m_stream		( NULL ),
m_source_voice	( NULL ),
m_stream_cursor_pcm	( 0 )
{
}

sound_item::~sound_item( )
{
	if( m_source_voice )
		m_source_voice->DestroyVoice( );

	m_source_voice					= NULL;

}

//void sound_item::play( pcstr name )
//{
//	resources::query_resource
//	(
//		name,
//		resources::raw_data_class,
//		resources::query_callback(this, &sound_item::on_source_loaded),
//		g_allocator
//	);
//}

void sound_item::start( sound_stream* stream )
{
	m_stream	= stream;
	HRESULT res = m_sound_world.xaudio_engine()->CreateSourceVoice( 
										&m_source_voice,
										&m_stream->wave_format(), 
										0, 
										XAUDIO2_DEFAULT_FREQ_RATIO, 
										this, 
										NULL, 
										NULL
										);

	ASSERT					(!FAILED(res));

	refill_buffers			( );
	res						= m_source_voice->Start( );
	ASSERT					( !FAILED(res) );
}

void sound_item::refill_buffers( )
{
	ASSERT					( m_stream );
	HRESULT					res;
	XAUDIO2_VOICE_STATE		vstate;
	m_source_voice->GetState( &vstate );
	u32 pcm_total 			= m_stream->pcm_total();
	
	while( (vstate.BuffersQueued < max_queued_buffers) && ( m_stream_cursor_pcm < pcm_total) )
	{
		XAUDIO2_BUFFER			buffer = {0};

		pbyte dest_buffer		= m_sound_world.get_cache_buffer( snd_buff_size );
		buffer.AudioBytes		= m_stream->decompress( dest_buffer, m_stream_cursor_pcm, snd_buff_size );
		buffer.pAudioData		= dest_buffer;
		buffer.pContext			= dest_buffer;

		buffer.Flags			= ( m_stream_cursor_pcm==pcm_total ) ? XAUDIO2_END_OF_STREAM : 0;

		ASSERT					( buffer.AudioBytes>0 );

		res						= m_source_voice->SubmitSourceBuffer( &buffer );
		ASSERT					( !FAILED(res) );
		++vstate.BuffersQueued;
	}
}

void sound_item::on_end_stream( )
{
	m_source_voice->Stop			( );
	m_source_voice->DestroyVoice	( );
	m_source_voice					= NULL;
	m_sound_world.on_voice_stopped	( this );
	LOG_INFO("---voice destroyed");
}

void sound_item::on_buffer_end( void* pcontext )
{
	m_sound_world.free_cache_buffer ( (pbyte)pcontext );
	LOG_INFO("---voice on_buffer_end");
	refill_buffers					( );
}

void sound_item::on_buffer_error( void* pcontext)
{
	XRAY_UNREFERENCED_PARAMETERS( pcontext );
	LOG_INFO("---voice on_buffer_error");
}

} // namespace sound
} // namespace xray
