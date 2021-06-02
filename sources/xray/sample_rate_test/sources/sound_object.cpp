#include "pch.h"
#include "sound_object.h"
//#include <assert.h>
//#include <xaudio2.h>

#define STREAMING_BUFFER_SIZE /*2048*10*/ 44100 /* * 4*/ //65536
#define MAX_BUFFER_COUNT 3

BYTE buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
BYTE buffer_for_resample[STREAMING_BUFFER_SIZE];
BYTE buffer_for_conversion[STREAMING_BUFFER_SIZE];

#define TAIL_BUFFER_SIZE 20
#define COMPARE_BUFFER_SIZE 60
s16		conversion_tail_buffer[TAIL_BUFFER_SIZE];
s16		compare_buffer[COMPARE_BUFFER_SIZE];

sound_object::sound_object(IXAudio2* pXAudio2, sound_stream* stream_0, sound_stream* stream_1, sound_stream* stream_2) :
	m_pSourceVoice(0),
	m_p_stream_0(stream_0),
	m_p_stream_1(stream_1),
	m_p_stream_2(stream_2),
	m_p_stream_current(stream_0),
	m_p_stream_next(stream_0),
	m_stream_cursor_pcm(0),
	m_target_quality(0),
	m_buffer_ended(false),
	m_current_disk_read_buffer(0),
	m_conversion_state(NONE)
{
	HRESULT hr = pXAudio2->CreateSourceVoice( &m_pSourceVoice,
		(WAVEFORMATEX*)&m_p_stream_current->wave_format(),
											0,
											XAUDIO2_DEFAULT_FREQ_RATIO,
											this,
											NULL,
											NULL );
	assert(hr == S_OK);
	memset(&m_temp_buffer, 0, sizeof(XAUDIO2_BUFFER));
	//m_pSourceVoice->FlushSourceBuffers();

	if (!xray::fs::open_file(&m_file, xray::fs::open_file_write | xray::fs::open_file_create , "Z:/test/ogg/original_raw.raw"))
	{
		LOG_ERROR("open_file failed!");
	}

	m_timer.start();
	printf("time - %d\n", m_timer.get_elapsed_ms());
}

sound_object::~sound_object()
{
	m_pSourceVoice->Stop();
	m_pSourceVoice->DestroyVoice();

}

void sound_object::update()
{

	
	if (m_target_quality != m_current_quality && m_conversion_state == NONE)
	{
		change_sound_quality(m_target_quality);
	}


	if (m_buffer_ended)
	{
		m_buffer_ended = false;
		refill_buffers();
	}



}

void sound_object::refill_buffers()
{
	printf("REFILL_BUFFERS START TIME: %d\n", m_timer.get_elapsed_ms());
	HRESULT					res;
	XAUDIO2_VOICE_STATE		vstate;
	m_pSourceVoice->GetState( &vstate );
	unsigned int pcm_total 			= m_p_stream_current->pcm_total( );

	while( (vstate.BuffersQueued < MAX_BUFFER_COUNT) && ( m_stream_cursor_pcm < pcm_total) )
	{

		printf("new buffer number: %d queued time: %d\n", vstate.BuffersQueued, m_timer.get_elapsed_ms());
		XAUDIO2_BUFFER buffer		= {0};
		
		unsigned int  bytes_filled			= 0;

		bytes_filled			= m_p_stream_current->decompress( buffers[ m_current_disk_read_buffer ], m_stream_cursor_pcm, STREAMING_BUFFER_SIZE);
				
		printf("stream decompressed to buffer number : %d time: %d\n", vstate.BuffersQueued, m_timer.get_elapsed_ms());


		if (m_stream_cursor_pcm==pcm_total)
		{
			buffer.Flags			= XAUDIO2_END_OF_STREAM;
			m_conversion_state		= CONVESION_CAN_NOT_BE_APPLIED;
		}
		else
		{
			m_temp_buffer.Flags			= 0;
		}

		buffer.pAudioData		= buffers[ m_current_disk_read_buffer ];
		buffer.AudioBytes		= bytes_filled;
		buffer.pContext			= NULL;


		assert				(m_stream_cursor_pcm <= pcm_total);
		assert				( buffer.AudioBytes>0 );
		res					= m_pSourceVoice->SubmitSourceBuffer( &buffer );
		assert				( !FAILED(res) );

		++m_current_disk_read_buffer;
        m_current_disk_read_buffer %= MAX_BUFFER_COUNT;
		vstate.BuffersQueued++;

		if (m_conversion_state == RESAMPLED_BUFFER_FINISH_PLAYING)
			m_conversion_state = NONE;
	}
}

template<int val>
static bool compare_function(int i, int j)
{
	bool res = std::abs(i - j) < val;  
	return res;
}

void sound_object::fill_temp_buffer()
{
	printf("fill_temp_buffer start time: %d\n", m_timer.get_elapsed_ms());
	unsigned int pcm_total 			= m_p_stream_next->pcm_total( );
	unsigned int bytes_per_sample	= (m_p_stream_next->wave_format( ).wBitsPerSample) >> 3;
	unsigned int  bytes_filled		= 0;

	bytes_filled					= m_p_stream_next->decompress( buffer_for_resample, m_stream_cursor_pcm, STREAMING_BUFFER_SIZE );

	m_temp_buffer.Flags				= ( m_stream_cursor_pcm==pcm_total ) ? XAUDIO2_END_OF_STREAM : 0;
	m_temp_buffer.pAudioData		= buffer_for_resample;
	m_temp_buffer.AudioBytes		= bytes_filled;
	m_temp_buffer.pContext			= NULL;
	m_temp_buffer.LoopBegin			= 0;
	m_temp_buffer.LoopCount			= 0;
	m_temp_buffer.LoopLength		= 0;
	printf("fill_temp_buffer end time: %d\n\n", m_timer.get_elapsed_ms());

	short* compare_buff_start;
	u32 total_samples_in_buffer = STREAMING_BUFFER_SIZE / bytes_per_sample;
	if (m_p_stream_current->wave_format().nSamplesPerSec > m_p_stream_next->wave_format().nSamplesPerSec)
	{
		u32 conversion_ratio = m_p_stream_current->wave_format().nSamplesPerSec / m_p_stream_next->wave_format().nSamplesPerSec;
		compare_buff_start		= (short*)buffer_for_resample;
		compare_buff_start		+=  total_samples_in_buffer / conversion_ratio - COMPARE_BUFFER_SIZE / 2;

		std::copy					(compare_buff_start, compare_buff_start + COMPARE_BUFFER_SIZE, compare_buffer);

		short* compare_buffer_start			= compare_buffer;
		short* compare_buffer_end			= compare_buffer + COMPARE_BUFFER_SIZE;
		short* conversion_buffer_start		= conversion_tail_buffer;
		short* conversion_buffer_end		= conversion_tail_buffer + TAIL_BUFFER_SIZE;
		short* pos = std::search(compare_buffer_start, compare_buffer_end, conversion_buffer_start, conversion_buffer_end, compare_function<100>);
		if (pos == compare_buffer_end)
		{
			pos = std::search(compare_buffer_start, compare_buffer_end, conversion_buffer_start, conversion_buffer_end, compare_function<500>);
			if (pos != compare_buffer_end)
				printf("\n\nwave finded with delta 500\n");
		}
		else
		{
			printf("\n\nwave finded with delta 100\n");
		}

		if (pos != compare_buffer_end)
		{
			m_temp_buffer.PlayBegin			= std::distance((short*)buffer_for_resample, compare_buff_start) + std::distance(conversion_buffer_start, pos) + TAIL_BUFFER_SIZE;
			m_temp_buffer.PlayLength		= total_samples_in_buffer - m_temp_buffer.PlayBegin;
		}
		else
		{
			m_temp_buffer.PlayBegin			= total_samples_in_buffer / conversion_ratio;
			m_temp_buffer.PlayLength		= total_samples_in_buffer - m_temp_buffer.PlayBegin;
		}
	}
	else if (m_p_stream_current->wave_format().nSamplesPerSec < m_p_stream_next->wave_format().nSamplesPerSec)
	{
		m_temp_buffer.PlayBegin = 0;
		m_temp_buffer.PlayLength = 0;
//		compare_buff_start = ((short*)buffer_for_resample) + total_samples_in_buffer - COMPARE_BUFFER_SIZE;
	}

	if (m_conversion_state != TAIL_BUFFER_SUBMITED)
		m_conversion_state		= RESAMPLED_BUFFER_FILLED;
}


void sound_object::fill_conversion_buffer()
{

	printf("fill_temp_buffer start time: %d\n", m_timer.get_elapsed_ms());

	unsigned int pcm_total 				= m_p_stream_current->pcm_total( );

	m_stream_tmp_pcm					= m_stream_cursor_pcm;
	unsigned int  bytes_filled			= 0;
	
	bytes_filled						= m_p_stream_current->decompress( buffer_for_conversion, m_stream_cursor_pcm, STREAMING_BUFFER_SIZE );

	m_conversion_buffer.Flags			= ( m_stream_cursor_pcm==pcm_total ) ? XAUDIO2_END_OF_STREAM : 0;
	m_conversion_buffer.pAudioData		= buffer_for_conversion;
	m_conversion_buffer.AudioBytes		= bytes_filled;
	m_conversion_buffer.pContext		= NULL;
	m_conversion_buffer.LoopBegin		= 0;
	m_conversion_buffer.LoopCount		= 0;
	m_conversion_buffer.LoopLength		= 0;
	m_conversion_buffer.PlayBegin		= 0;
	m_conversion_buffer.PlayLength		= 0;


	u32 conversion_ration = 0;
	// hi quality to low
	if (m_p_stream_current->wave_format().nSamplesPerSec > m_p_stream_next->wave_format().nSamplesPerSec)
	{
		conversion_ration = m_p_stream_current->wave_format().nSamplesPerSec / m_p_stream_next->wave_format().nSamplesPerSec;
		short* tail_pos	= (short*)(buffer_for_conversion + STREAMING_BUFFER_SIZE);
		tail_pos -=  TAIL_BUFFER_SIZE * conversion_ration;
		for (u32 i = 0; i < TAIL_BUFFER_SIZE; ++i)
		{
			s16 sum = 0;
			for (u32 j = 0; j < conversion_ration; ++j)
			{
				sum += *tail_pos;
				++tail_pos;
			}
			sum /= (s16)conversion_ration;
			conversion_tail_buffer[i] = sum;
		}
		
		m_conversion_state = TAIL_BUFFER_FILLED;
	}
	// low quality to hi
	else if (m_p_stream_current->wave_format().nSamplesPerSec < m_p_stream_next->wave_format().nSamplesPerSec)
	{
		// don't submit tail buffer
		m_conversion_state = TAIL_BUFFER_SUBMITED;
		// no convesion buffer
		/*conversion_ration = m_p_stream_next->wave_format().nSamplesPerSec / m_p_stream_current->wave_format().nSamplesPerSec;
		short* tail_pos	= ((short*)(buffer_for_conversion + STREAMING_BUFFER_SIZE / conversion_ration)) - TAIL_BUFFER_SIZE;
		u32 bytes_per_sample = m_p_stream_current->wave_format().wBitsPerSample >> 3;
		m_conversion_buffer.PlayLength		= STREAMING_BUFFER_SIZE / bytes_per_sample / conversion_ration;
		
		for (u32 i = 0; i < TAIL_BUFFER_SIZE; ++i)
		{
			conversion_tail_buffer[i] = *tail_pos;
			++tail_pos;
		}*/
	}

}

void sound_object::play()
{
	refill_buffers			( );
	m_pSourceVoice->Start	( );
}


void sound_object::change_sound_quality(int quality)
{
	m_target_quality = quality;

	if (m_conversion_state != NONE)
		return;

	sound_stream* tmp;

	switch(quality)
	{
	case 0: tmp = m_p_stream_0; break;
	case 1: tmp = m_p_stream_1; break;
	case 2: tmp = m_p_stream_2; break;
	default: return;
	}

	if (tmp == m_p_stream_current)
	{
		return;
	}

	m_current_quality = quality;
	m_conversion_state = CONVERSION_QUEUED;

	m_p_stream_next		= tmp;

	int old_pcm_cursor = m_stream_cursor_pcm;
	fill_conversion_buffer();

	unsigned int new_samples_per_sec	= tmp->wave_format().nSamplesPerSec;
	unsigned int old_samples_per_sec	= m_p_stream_current->wave_format().nSamplesPerSec;
	if (new_samples_per_sec  != old_samples_per_sec)
	{
		if (new_samples_per_sec > old_samples_per_sec)
		{
			m_stream_cursor_pcm			= (new_samples_per_sec / old_samples_per_sec) * old_pcm_cursor; 
		}
		else
		{
			m_stream_cursor_pcm			= old_pcm_cursor /(old_samples_per_sec / new_samples_per_sec) ; 
		}
		
	}
	fill_temp_buffer			( );
	m_p_stream_current			= tmp;
}

// Called just before this voice's processing pass begins.
void sound_object::OnVoiceProcessingPassStart( UINT32 /*BytesRequired*/ )
{
	//std::cout << "OnVoiceProcessingPassStart";
}

// Called just after this voice's processing pass ends.
void sound_object::OnVoiceProcessingPassEnd( )
{
}

// Called when this voice has just finished playing a buffer stream
// (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
void sound_object::OnStreamEnd( )
{
}

// Called when this voice is about to start processing a new buffer.
void sound_object::OnBufferStart( void* /*pBufferContext*/ )
{
	
}

// Called when this voice has just finished processing a buffer.
// The buffer can now be reused or destroyed.
void sound_object::OnBufferEnd( void* pBufferContext )
{
	XAUDIO2_VOICE_STATE state;
	m_pSourceVoice->GetState(&state);
//	printf("OnBufferEnd called! buffer qued: %d time: %d\n", state.BuffersQueued, m_timer.get_elapsed_ms());


	if (m_conversion_state == NONE || m_conversion_state == RESAMPLED_BUFFER_SUBMITED)
	{
	//	printf("buffer ended\n");

		if (pBufferContext != NULL)
		{
		//	printf("pBufferContext != NULL\n");
			m_conversion_state					= RESAMPLED_BUFFER_FINISH_PLAYING;
		}

		m_buffer_ended = true;

	}
	else
	{


		if (state.BuffersQueued == 1 && (/*m_conversion_state == TAIL_BUFFER_FILLED ||*/ m_conversion_state == RESAMPLED_BUFFER_FILLED))
		{
		//	printf("conversion buffer submitted start time: %d\n", m_timer.get_elapsed_ms());
			m_pSourceVoice->SubmitSourceBuffer(&m_conversion_buffer);
			m_conversion_state = TAIL_BUFFER_SUBMITED;
		//	printf("conversion buffer submitted end time: %d\n", m_timer.get_elapsed_ms());
		}
		else if (state.BuffersQueued == 0 && m_conversion_state == TAIL_BUFFER_SUBMITED)
		{
			printf("\n************NEW SAMPLE RATE SUBMITTED************\n\n");
		//	printf("new sample rate submitted start time: %d\n", m_timer.get_elapsed_ms());
			m_pSourceVoice->SetSourceSampleRate	(m_p_stream_current->wave_format( ).nSamplesPerSec);
			m_temp_buffer.pContext =  (void*)true;
			m_pSourceVoice->SubmitSourceBuffer	(&m_temp_buffer);
		//	printf("new sample rate submitted end time: %d\n\n", m_timer.get_elapsed_ms());
			m_conversion_state = RESAMPLED_BUFFER_SUBMITED;
			m_buffer_ended = true;
		}
	}
}

// Called when this voice has just reached the end position of a loop.
void sound_object::OnLoopEnd( void* /*pBufferContext*/ )
{
}

// Called in the event of a critical error during voice processing,
// such as a failing xAPO or an error from the hardware XMA decoder.
// The voice may have to be destroyed and re-created to recover from
// the error.  The callback arguments report which buffer was being
// processed when the error occurred, and its HRESULT code.
void sound_object::OnVoiceError( void* /*pBufferContext*/, HRESULT /*Error*/ )
{
}



//// Called when this voice has just finished processing a buffer.
//// The buffer can now be reused or destroyed.
//void sound_object::OnBufferEnd( void* pBufferContext )
//{
//	XAUDIO2_VOICE_STATE state;
//	m_pSourceVoice->GetState(&state);
////	printf("OnBufferEnd called! buffer qued: %d time: %d\n", state.BuffersQueued, m_timer.get_elapsed_ms());
//
//
//	if (!m_sample_rate_changing)
//	{
//		printf("buffer ended");
//		m_buffer_ended = true;
//	}
//	else
//	{
//
//		if (pBufferContext != NULL)
//		{
//			m_sample_rate_changing				= false;
//		}
//
//		if (!m_conversion_buffer_added)
//		{
//			printf("conversion buffer submitted start time: %d\n", m_timer.get_elapsed_ms());
//			m_pSourceVoice->SubmitSourceBuffer(&m_conversion_buffer);
//			m_conversion_buffer_added = true;
//		//	printf("conversion buffer submitted end time: %d\n", m_timer.get_elapsed_ms());
//		}
//		else if (state.BuffersQueued == 0)
//		{
//			printf("\n************NEW SAMPLE RATE SUBMITTED************\n\n");
//		//	printf("new sample rate submitted start time: %d\n", m_timer.get_elapsed_ms());
//			m_pSourceVoice->SetSourceSampleRate	(m_p_stream_current->wave_format( ).nSamplesPerSec);
//			m_temp_buffer.pContext =  (void*)true;
//			m_pSourceVoice->SubmitSourceBuffer	(&m_temp_buffer);
//		//	printf("new sample rate submitted end time: %d\n\n", m_timer.get_elapsed_ms());
//			
//			m_buffer_ended						= true;
//		}
//	}
//}
//
//void sound_object::change_sound_quality(int quality)
//{
//	m_target_quality = quality;
//
//	if (m_sample_rate_changing || !m_can_change_quality)
//		return;
//
//
//	sound_stream* tmp;
//
//	switch(quality)
//	{
//	case 0: tmp = m_p_stream_0; break;
//	case 1: tmp = m_p_stream_1; break;
//	case 2: tmp = m_p_stream_2; break;
//	default: return;
//	}
//
//	if (tmp == m_p_stream_current)
//	{
//		return;
//	}
//
//	m_current_quality = quality;
//	
//	m_can_change_quality			= false;
//	m_sample_rate_changing			= true;	
//
//	m_conversion_buffer_added		= false;
//	m_p_stream_next		= tmp;
//
//	int old_pcm_cursor = m_stream_cursor_pcm;
//	fill_conversion_buffer();
//
//	unsigned int new_samples_per_sec	= tmp->wave_format().nSamplesPerSec;
//	unsigned int old_samples_per_sec	= m_p_stream_current->wave_format().nSamplesPerSec;
//	if (new_samples_per_sec  != old_samples_per_sec)
//	{
//		if (new_samples_per_sec > old_samples_per_sec)
//		{
//			m_stream_cursor_pcm			= (new_samples_per_sec / old_samples_per_sec) * old_pcm_cursor; 
//		}
//		else
//		{
//			m_stream_cursor_pcm			= old_pcm_cursor /(old_samples_per_sec / new_samples_per_sec) ; 
//		}
//		
//	}
//	fill_temp_buffer			( );
//	m_p_stream_current			= tmp;
//}

//void sound_object::refill_buffers()
//{
//	printf("refill_buffers time: %d\n", m_timer.get_elapsed_ms());
//	HRESULT					res;
//	XAUDIO2_VOICE_STATE		vstate;
//	m_pSourceVoice->GetState( &vstate );
//	unsigned int pcm_total 			= m_p_stream_current->pcm_total( );
//
//	while( (vstate.BuffersQueued < MAX_BUFFER_COUNT) && ( m_stream_cursor_pcm < pcm_total) )
//	{
//		printf("new buffer number: %d queued time: %d\n", vstate.BuffersQueued, m_timer.get_elapsed_ms());
//		XAUDIO2_BUFFER buffer		= {0};
//		
//		unsigned int  bytes_filled			= 0;
//
//		bytes_filled			= m_p_stream_current->decompress( buffers[ m_current_disk_read_buffer ], m_stream_cursor_pcm, STREAMING_BUFFER_SIZE);
//				
//		printf("stream decompressed to buffer number : %d time: %d\n", vstate.BuffersQueued, m_timer.get_elapsed_ms());
//
//
//		if (m_stream_cursor_pcm==pcm_total)
//		{
//			buffer.Flags			= XAUDIO2_END_OF_STREAM;
//			m_can_change_quality		= false;
//		}
//		else
//		{
//			m_temp_buffer.Flags			= 0;
//		}
//
//		buffer.pAudioData		= buffers[ m_current_disk_read_buffer ];
//		buffer.AudioBytes		= bytes_filled;
//		buffer.pContext			= NULL;
//
//
//		assert				(m_stream_cursor_pcm <= pcm_total);
//		assert				( buffer.AudioBytes>0 );
//		res					= m_pSourceVoice->SubmitSourceBuffer( &buffer );
//		assert				( !FAILED(res) );
//
//		++m_current_disk_read_buffer;
//        m_current_disk_read_buffer %= MAX_BUFFER_COUNT;
//		vstate.BuffersQueued++;
//
//	}
//}
//
//template<int val>
//static bool compare_function(int i, int j)
//{
//	bool res = std::abs(i - j) < val;  
//	return res;
//}
//
//void sound_object::fill_temp_buffer()
//{
//	printf("fill_temp_buffer start time: %d\n", m_timer.get_elapsed_ms());
//	//unsigned int pcm_total 			= m_p_stream_next->pcm_total( );
//	unsigned int bytes_per_sample	= (m_p_stream_next->wave_format( ).wBitsPerSample) >> 3;
//	unsigned int  bytes_filled		= 0;
//
//	bytes_filled					= m_p_stream_next->decompress( buffer_for_resample, m_stream_cursor_pcm, STREAMING_BUFFER_SIZE );
//
//	m_temp_buffer.pAudioData		= buffer_for_resample;
//	m_temp_buffer.AudioBytes		= bytes_filled;
//	m_temp_buffer.pContext			= NULL;
//	m_temp_buffer.LoopBegin			= 0;
//	m_temp_buffer.LoopCount			= 0;
//	m_temp_buffer.LoopLength		= 0;
//	printf("fill_temp_buffer end time: %d\n\n", m_timer.get_elapsed_ms());
//
//	short* compare_buff_start;
//	u32 total_samples_in_buffer = STREAMING_BUFFER_SIZE / bytes_per_sample;
//	if (m_p_stream_current->wave_format().nSamplesPerSec > m_p_stream_next->wave_format().nSamplesPerSec)
//	{
//		u32 conversion_ratio = m_p_stream_current->wave_format().nSamplesPerSec / m_p_stream_next->wave_format().nSamplesPerSec;
//		compare_buff_start		= (short*)buffer_for_resample;
//		compare_buff_start		+=  total_samples_in_buffer / conversion_ratio - COMPARE_BUFFER_SIZE / 2;
//
//		std::copy					(compare_buff_start, compare_buff_start + COMPARE_BUFFER_SIZE, compare_buffer);
//
//		short* compare_buffer_start			= compare_buffer;
//		short* compare_buffer_end			= compare_buffer + COMPARE_BUFFER_SIZE;
//		short* conversion_buffer_start		= conversion_tail_buffer;
//		short* conversion_buffer_end		= conversion_tail_buffer + TAIL_BUFFER_SIZE;
//		short* pos = std::search(compare_buffer_start, compare_buffer_end, conversion_buffer_start, conversion_buffer_end, compare_function<100>);
//		if (pos == compare_buffer_end)
//		{
//			pos = std::search(compare_buffer_start, compare_buffer_end, conversion_buffer_start, conversion_buffer_end, compare_function<500>);
//			if (pos != compare_buffer_end)
//				printf("\n\nwave finded with delta 500\n");
//		}
//		else
//		{
//			printf("\n\nwave finded with delta 100\n");
//		}
//
//		if (pos != compare_buffer_end)
//		{
//			m_temp_buffer.PlayBegin			= std::distance((short*)buffer_for_resample, compare_buff_start) + std::distance(conversion_buffer_start, pos) + TAIL_BUFFER_SIZE;
//			m_temp_buffer.PlayLength		= total_samples_in_buffer - m_temp_buffer.PlayBegin;
//		}
//		else
//		{
//			m_temp_buffer.PlayBegin			= total_samples_in_buffer / conversion_ratio;
//			m_temp_buffer.PlayLength		= total_samples_in_buffer - m_temp_buffer.PlayBegin;
//		}
//	}
//	else if (m_p_stream_current->wave_format().nSamplesPerSec < m_p_stream_next->wave_format().nSamplesPerSec)
//	{
//		m_temp_buffer.PlayBegin = 0;
//		m_temp_buffer.PlayLength = 0;
////		compare_buff_start = ((short*)buffer_for_resample) + total_samples_in_buffer - COMPARE_BUFFER_SIZE;
//	}
//
//}
//
//
//void sound_object::fill_conversion_buffer()
//{
//
//	printf("fill_temp_buffer start time: %d\n", m_timer.get_elapsed_ms());
//
//	unsigned int pcm_total 				= m_p_stream_current->pcm_total( );
//
//	m_stream_tmp_pcm					= m_stream_cursor_pcm;
//	unsigned int  bytes_filled			= 0;
//	
//	bytes_filled						= m_p_stream_current->decompress( buffer_for_conversion, m_stream_cursor_pcm, STREAMING_BUFFER_SIZE );
//
//	if (m_stream_cursor_pcm==pcm_total)
//	{
//		m_sample_rate_changing = false;
//		m_stream_cursor_pcm = m_stream_tmp_pcm;
//		return;
//	}
//
//	m_conversion_buffer.Flags			= ( m_stream_cursor_pcm==pcm_total ) ? XAUDIO2_END_OF_STREAM : 0;
//	m_conversion_buffer.pAudioData		= buffer_for_conversion;
//	m_conversion_buffer.AudioBytes		= bytes_filled;
//	m_conversion_buffer.pContext		= NULL;
//	m_conversion_buffer.LoopBegin		= 0;
//	m_conversion_buffer.LoopCount		= 0;
//	m_conversion_buffer.LoopLength		= 0;
//	m_conversion_buffer.PlayBegin		= 0;
//	m_conversion_buffer.PlayLength		= 0;
//
//
//	u32 conversion_ration = 0;
//	// hi quality to low
//	if (m_p_stream_current->wave_format().nSamplesPerSec > m_p_stream_next->wave_format().nSamplesPerSec)
//	{
//		conversion_ration = m_p_stream_current->wave_format().nSamplesPerSec / m_p_stream_next->wave_format().nSamplesPerSec;
//		short* tail_pos	= (short*)(buffer_for_conversion + STREAMING_BUFFER_SIZE);
//		tail_pos -=  TAIL_BUFFER_SIZE * conversion_ration;
//		for (u32 i = 0; i < TAIL_BUFFER_SIZE; ++i)
//		{
//			s16 sum = 0;
//			for (u32 j = 0; j < conversion_ration; ++j)
//			{
//				sum += *tail_pos;
//				++tail_pos;
//			}
//			sum /= (s16)conversion_ration;
//			conversion_tail_buffer[i] = sum;
//		}
//	}
//	// low quality to hi
//	else if (m_p_stream_current->wave_format().nSamplesPerSec < m_p_stream_next->wave_format().nSamplesPerSec)
//	{
//		m_conversion_buffer_added = true;
//		// no convesion buffer
//		/*conversion_ration = m_p_stream_next->wave_format().nSamplesPerSec / m_p_stream_current->wave_format().nSamplesPerSec;
//		short* tail_pos	= ((short*)(buffer_for_conversion + STREAMING_BUFFER_SIZE / conversion_ration)) - TAIL_BUFFER_SIZE;
//		u32 bytes_per_sample = m_p_stream_current->wave_format().wBitsPerSample >> 3;
//		m_conversion_buffer.PlayLength		= STREAMING_BUFFER_SIZE / bytes_per_sample / conversion_ration;
//		
//		for (u32 i = 0; i < TAIL_BUFFER_SIZE; ++i)
//		{
//			conversion_tail_buffer[i] = *tail_pos;
//			++tail_pos;
//		}*/
//	}
//}
