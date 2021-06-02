////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEM_H_INCLUDED
#define SOUND_ITEM_H_INCLUDED

namespace xray {
namespace sound {

class sound_world;
class sound_stream;

// 0.5 sec buffer (44.1 kHz*2byte_per_sample / 2 )
#define snd_buff_size		(44100 * 2 / 2)
#define max_queued_buffers	( 2 )


class sound_item:	public IXAudio2VoiceCallback,
					public boost::noncopyable
{
public:
								sound_item					( sound_world& w );
								~sound_item					( );

			void				start						( sound_stream* stream );

private:
			sound_world&		m_sound_world;

	IXAudio2SourceVoice*		m_source_voice;
			sound_stream*		m_stream;
			u32					m_stream_cursor_pcm;
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

}; // class sound_item

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_ITEM_H_INCLUDED