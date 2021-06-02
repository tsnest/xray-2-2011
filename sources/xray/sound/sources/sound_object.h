////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OBJECT_H_INCLUDED
#define SOUND_OBJECT_H_INCLUDED

#include <xray/sound/sound.h>
#include "sound_stream.h"

namespace xray {
namespace sound {


class sound_world;

// buffer size must be divisible by 80ms
//
// 0.4 sec buffer 44100Hz * 2byte_per_sample * 0.4sec = 35280
 #define snd_buff_size		(35280)

// 3 sec 44100Hz * 2byte_per_sample * 3sec = 264600
//  #define snd_buff_size		(264600)

#define max_queued_buffers	( 3 )

class sound_object_impl:	public sound_object,
							public IXAudio2VoiceCallback,
							public boost::noncopyable
{
public:
								sound_object_impl			( sound_stream_ptr stream );
	virtual						~sound_object_impl			( );
	
	virtual void				play						( );
	virtual void				stop						( );
	virtual void				set_position				( float3 const& p );
	virtual float3 const&		get_position				( ) const				{return m_position;}

	virtual bool				is_playing					( ) const {return m_playing;}
	WAVEFORMATEX const&			wave_format					( )	const	{return m_stream->wave_format();}

	//virtual	void				recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }


			void				play_impl					( ); // int, tmp
			void				stop_impl					( ); // int, tmp
	base_allocator_type*		thread_allocator			( ) const { return m_thread_allocator;}
			void				set_thread_allocator		( base_allocator_type* a ) {m_thread_allocator=a;}
			void				set_position_impl			( float3 const& p );

	IXAudio2SourceVoice*		m_source_voice;
		sound_stream_ptr		m_stream;
			u32					m_stream_cursor_pcm;
		base_allocator_type*	m_thread_allocator;
		bool					m_playing;
		float3					m_position;
private:
			void				refill_buffers				( );
public: // callback impl
    void __stdcall 				OnVoiceProcessingPassStart	( UINT32 BytesRequired );
    void __stdcall 				OnVoiceProcessingPassEnd	( );
    void __stdcall 				OnStreamEnd					( );
    void __stdcall 				OnBufferStart				( void* pBufferContext );
    void __stdcall 				OnBufferEnd					( void* pBufferContext );
    void __stdcall 				OnLoopEnd					( void* pBufferContext );
    void __stdcall 				OnVoiceError				( void* pBufferContext, HRESULT Error );

	// in sound thread:
			void				on_end_stream				( );
			void				on_buffer_end				( void* pcontext );
			void				on_buffer_error				( void* pcontext );

}; // class sound_object_impl

typedef	resources::resource_ptr<
			sound_object_impl,
			resources::unmanaged_intrusive_base
		> sound_impl_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_OBJECT_H_INCLUDED