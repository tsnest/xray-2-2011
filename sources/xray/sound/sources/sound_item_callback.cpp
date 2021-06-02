////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_item.h"
#include "sound_item_callback.h"
#include "sound_world.h"

namespace xray {
namespace sound {

// Called just before this voice's processing pass begins.
void sound_item::OnVoiceProcessingPassStart( UINT32 BytesRequired )
{
	XRAY_UNREFERENCED_PARAMETERS( BytesRequired );
//	LOG_INFO("OnVoiceProcessingPassStart: %d", BytesRequired);
}

// Called just after this voice's processing pass ends.
void sound_item::OnVoiceProcessingPassEnd( )
{
//	LOG_INFO("OnVoiceProcessingPassEnd");
}

// Called when this voice has just finished playing a buffer stream
// (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
void sound_item::OnStreamEnd( )
{
	m_sound_world.add_callback_command( MT_NEW(callback_command)(callback_command::ev_stream_end, this, NULL));
	LOG_INFO("OnStreamEnd");
}

// Called when this voice is about to start processing a new buffer.
void sound_item::OnBufferStart( void* pBufferContext )
{
	XRAY_UNREFERENCED_PARAMETERS( pBufferContext );
//	LOG_INFO("OnBufferStart: %x", pBufferContext);
}

// Called when this voice has just finished processing a buffer.
// The buffer can now be reused or destroyed.
void sound_item::OnBufferEnd( void* pBufferContext )
{
	m_sound_world.add_callback_command( MT_NEW(callback_command)(callback_command::ev_buffer_end, this, pBufferContext));
//	LOG_INFO("OnBufferEnd: %x", pBufferContext);
}

// Called when this voice has just reached the end position of a loop.
void sound_item::OnLoopEnd( void* pBufferContext )
{
	LOG_INFO("OnLoopEnd: %x", pBufferContext);
}
// Called in the event of a critical error during voice processing,
// such as a failing xAPO or an error from the hardware XMA decoder.
// The voice may have to be destroyed and re-created to recover from
// the error.  The callback arguments report which buffer was being
// processed when the error occurred, and its HRESULT code.
void sound_item::OnVoiceError( void* pBufferContext, HRESULT Error )
{
	m_sound_world.add_callback_command( MT_NEW(callback_command)(callback_command::ev_buffer_error, this, pBufferContext));
	LOG_INFO("OnVoiceError: %d, %x", Error, pBufferContext);
}

void callback_command::execute( )
{
	switch (m_event_type)
	{
	case ev_stream_end:
		m_owner->on_end_stream( );
		break;

	case ev_buffer_end:
		m_owner->on_buffer_end( m_context );
		break;

	case ev_buffer_error:
		m_owner->on_buffer_error( m_context );
		break;

	default:
		LOG_ERROR("unknown callback event: %d", (int)m_event_type);
		NODEFAULT();
	}
}

} // namespace sound
} // namespace xray
