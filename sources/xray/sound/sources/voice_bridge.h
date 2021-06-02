////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VOICE_BRIDGE_H_INCLUDED
#define VOICE_BRIDGE_H_INCLUDED

#include <xray/sound/channels_type.h>
#include "voice_format.h"
#include "voice_callback_handler.h"

struct IXAudio2Voice;
struct IXAudio2SubmixVoice;

namespace xray {
namespace sound {

class sound_buffer;

class voice_bridge :
	private IXAudio2VoiceCallback,
	private boost::noncopyable
{
public:
			channels_type	get_channels_type				( ) const;
			u32				get_sample_rate					( ) const;
			u8				get_bytes_per_second			( ) const;

			void			start							( );
			void			stop							( );
			void			submit_source_buffer			( sound_buffer* buffer, u32 playing_offset = 0, u32 playing_length = 0 );
//			for debug only
//			void			submit_source_buffer			( XAUDIO2_BUFFER& buf );
			void			flush_source_buffers			( );

			u32				buffers_queued					( ) const;
			sound_buffer*	current_sound_buffer			( ) const;
			u64				samples_played					( ) const;

			void			set_output_matrix				( float const* level_matrix );
			void			set_channel_volumes				( channels_type type, float const* level_matrix );
			void			set_low_pass_filter_params		( float coeff );
			void			set_output_voice				( IXAudio2SubmixVoice* output_voice  );
			void			set_sample_rate					( u32 new_sample_rate );
			void			set_frequency_ratio				( float ratio );
private:
	struct creation_parametrs
	{
		IXAudio2*			xaudio_engine;
		float				max_frequency_ratio;
		channels_type		type;
		channels_type		master_channels_type;
		
	}; // struct creation_parametrs

							voice_bridge					( creation_parametrs& params );

			bool			has_handler						( ) const { return m_handler != 0; }
			void			set_handler						( voice_callback_handler* handler );
			

	virtual void __stdcall	OnVoiceProcessingPassStart		( UINT32 BytesRequired );
    virtual void __stdcall	OnVoiceProcessingPassEnd		( );
    virtual void __stdcall	OnStreamEnd						( );
    virtual void __stdcall	OnBufferStart					( void* pBufferContext );
    virtual void __stdcall	OnBufferEnd						( void* pBufferContext );
    virtual void __stdcall	OnLoopEnd						( void* pBufferContext );
    virtual void __stdcall	OnVoiceError					( void* pBufferContext, HRESULT Error );

public:
	voice_bridge*			m_next;
	virtual					~voice_bridge					( );
private:
	voice_callback_handler*	m_handler;
	IXAudio2SourceVoice*	m_source_voice;
	channels_type			m_master_channels;

	friend class			voice_factory;
}; // class voice_bridge

typedef intrusive_list<voice_bridge, voice_bridge*, &voice_bridge::m_next, threading::single_threading_policy> voice_list_type;

} // namespace sound
} // namespace xray

#endif // #ifndef VOICE_BRIDGE_H_INCLUDED