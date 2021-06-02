#pragma once

#include <xray/os_preinclude.h>		// for GetCommandLine
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>		// for GetCommandLine

#include <xaudio2.h>
#include "ogg_utils.h"

struct sound_rms
{
				sound_rms	( );
	void		clear		( );

	float*					m_prms;
	unsigned int			m_count;
};

class sound_stream
{
public:
							sound_stream	( FILE* raw_ogg_file);
	virtual					~sound_stream	( );

	unsigned int			pcm_total		( ) const	{return m_pcm_total;}
	WAVEFORMATEX const&		wave_format		( )	const	{return m_wfx;}
	unsigned int			decompress		( void* dest, unsigned int& pcm_pointer, unsigned int bytes_needed );

private:

	WAVEFORMATEX			m_wfx;

	FILE*					m_raw_file;

	OggVorbis_File			m_ovf;
	unsigned int			m_pcm_total;
}; // class sound_stream