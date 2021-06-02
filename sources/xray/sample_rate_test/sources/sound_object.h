#pragma once

#include "sound_stream.h" 

enum conversion_state
{
	NONE,
	CONVERSION_QUEUED,
	TAIL_BUFFER_FILLED,
	RESAMPLED_BUFFER_FILLED,
	TAIL_BUFFER_SUBMITED,
	RESAMPLED_BUFFER_SUBMITED,
	RESAMPLED_BUFFER_FINISH_PLAYING,
	CONVESION_CAN_NOT_BE_APPLIED
};

class sound_object : public IXAudio2VoiceCallback
{
public:
	sound_object(IXAudio2* pXAudio2, sound_stream* steam_0, sound_stream* steam_1, sound_stream* steam_2);
	~sound_object();
	void				play											( );
	void				change_sound_quality							(int quality);
	void				update											( );
private:
	// callback impl
    void __stdcall 		OnVoiceProcessingPassStart	( UINT32 BytesRequired );
    void __stdcall 		OnVoiceProcessingPassEnd	( );
    void __stdcall 		OnStreamEnd					( );
    void __stdcall 		OnBufferStart				( void* pBufferContext );
    void __stdcall 		OnBufferEnd					( void* pBufferContext );
    void __stdcall 		OnLoopEnd					( void* pBufferContext );
    void __stdcall 		OnVoiceError				( void* pBufferContext, HRESULT Error );

	void refill_buffers( );
	void fill_temp_buffer( );
	void fill_conversion_buffer( );

	xray::timing::timer			m_timer;
	IXAudio2SourceVoice*	m_pSourceVoice;
	sound_stream*			m_p_stream_0;
	sound_stream*			m_p_stream_1;
	sound_stream*			m_p_stream_2;
	sound_stream*			m_p_stream_current;
	sound_stream*			m_p_stream_next;
	XAUDIO2_BUFFER			m_temp_buffer;
	XAUDIO2_BUFFER			m_conversion_buffer;
	unsigned int			m_stream_cursor_pcm;
	unsigned int			m_current_disk_read_buffer;
	unsigned int			m_stream_tmp_pcm;
	unsigned int			m_target_quality;
	unsigned int			m_current_quality;
	bool					m_buffer_ended;
//	bool					m_sample_rate_changing;
//	bool					m_conversion_buffer_added;
//	bool					m_can_change_quality;
	conversion_state		m_conversion_state;
	xray::fs::file_type*	m_file; 
};