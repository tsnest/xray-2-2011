#include "pch.h"
#include "sound_stream.h"
#include <assert.h>
#include <vorbis/vorbisfile.h>
#include "ogg_utils.h"

sound_stream::sound_stream( FILE* raw_file)
{

	m_raw_file					= raw_file;

	ZeroMemory(&m_wfx, sizeof(m_wfx));

	ov_callbacks ovc			= {ogg_utils::ov_read_func, ogg_utils::ov_seek_func, ogg_utils::ov_close_func, ogg_utils::ov_tell_func};
 	ov_open_callbacks			(m_raw_file, &m_ovf, NULL, 0, ovc);

	vorbis_info* ovi			= ov_info(&m_ovf, -1);

	assert						(ovi, "Invalid source info");

	m_wfx.nSamplesPerSec		= (ovi->rate);
	m_wfx.wFormatTag			= WAVE_FORMAT_PCM;
	m_wfx.nChannels				= unsigned short(ovi->channels);
	m_wfx.wBitsPerSample		= 16;

	m_wfx.nBlockAlign			= (m_wfx.nChannels * m_wfx.wBitsPerSample) / 8;
	m_wfx.nAvgBytesPerSec		= m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

	m_pcm_total					= (unsigned int)ov_pcm_total(&m_ovf, -1);
}

unsigned int sound_stream::decompress( void* dest, unsigned int& pcm_pointer, unsigned int bytes_needed )
{

	unsigned int res	= ogg_utils::decompress(&m_ovf, (unsigned char*)dest, pcm_pointer, bytes_needed);
	return			res;
}

sound_stream::~sound_stream( )
{
	ov_clear	( &m_ovf );
}