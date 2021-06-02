////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_object.h"
#include "sound_world.h"
//#include <xray/>

namespace xray {
namespace sound {
static bool _logging_initialized = false;

void check_logging()
{
	if(!_logging_initialized)
	{
//		xray::core::debug::on_thread_spawn		( );
		xray::logging::on_thread_spawn			( );

		_logging_initialized = true;
	}
}
// Called just before this voice's processing pass begins.
void sound_object_impl::OnVoiceProcessingPassStart( UINT32 BytesRequired )
{
	check_logging();
	XRAY_UNREFERENCED_PARAMETERS( BytesRequired );
//	LOG_INFO("OnVoiceProcessingPassStart: %d", BytesRequired);
}

// Called just after this voice's processing pass ends.
void sound_object_impl::OnVoiceProcessingPassEnd( )
{
	check_logging();
//	LOG_INFO("OnVoiceProcessingPassEnd");
}

// Called when this voice has just finished playing a buffer stream
// (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
void sound_object_impl::OnStreamEnd( )
{
	check_logging();
	g_sound_world->add_xaudio_callback_command( MT_NEW(callback_command)(&::xray::memory::g_mt_allocator, callback_command::ev_stream_end, this, NULL));
	LOG_INFO("OnStreamEnd");
}

// Called when this voice is about to start processing a new buffer.
void sound_object_impl::OnBufferStart( void* pBufferContext )
{
	check_logging();
	XRAY_UNREFERENCED_PARAMETERS( pBufferContext );
//	LOG_INFO("OnBufferStart: %x", pBufferContext);
}

// Called when this voice has just finished processing a buffer.
// The buffer can now be reused or destroyed.
void sound_object_impl::OnBufferEnd( void* pBufferContext )
{
	check_logging();
	g_sound_world->add_xaudio_callback_command( MT_NEW(callback_command)(&::xray::memory::g_mt_allocator, callback_command::ev_buffer_end, this, pBufferContext));
//	LOG_INFO("OnBufferEnd: %x", pBufferContext);
}

// Called when this voice has just reached the end position of a loop.
void sound_object_impl::OnLoopEnd( void* pBufferContext )
{
	check_logging();
	LOG_INFO("OnLoopEnd: %x", pBufferContext);
}
// Called in the event of a critical error during voice processing,
// such as a failing xAPO or an error from the hardware XMA decoder.
// The voice may have to be destroyed and re-created to recover from
// the error.  The callback arguments report which buffer was being
// processed when the error occurred, and its HRESULT code.
void sound_object_impl::OnVoiceError( void* pBufferContext, HRESULT Error )
{
	check_logging();
	g_sound_world->add_xaudio_callback_command( MT_NEW(callback_command)(&::xray::memory::g_mt_allocator, callback_command::ev_buffer_error, this, pBufferContext));
	LOG_INFO("OnVoiceError: %d, %x", Error, pBufferContext);
}

} // namespace sound
} // namespace xray
