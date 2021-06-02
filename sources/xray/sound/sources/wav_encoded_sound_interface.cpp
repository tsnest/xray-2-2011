////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "wav_encoded_sound_interface.h"

namespace xray {
namespace sound {

struct riff
{
  u8	id[4];			//contains identifier "RIFF"
  u32	size;			//File size minus 8 bytes
  u8	format[4];		//contains identifier "WAVE"
};

struct waveform
{
  u16	wFormatTag;			//format of digital sound
  u16	nChannels;			//Number of channels (1 for mono and
                            //2 for stereo)
  u32	nSamplesPerSec;		//Number of samples per second
  u32	nAvgBytesPerSec;	//Average number bytes of data per
                            //second
  u16	nBlockAlign;		//Minimal data size for playing
  u16	wBitsPerSample;		//Bits per sample (8 or 16)
};

struct fmt
{
  u8		fmt_id[4];		//contains identifier: "fmt " (with
							//space)
  u16		fmt_size;		//contains the size of this block
							//(for WAVE PCM 16)
  waveform	fmt_format;		//structure WAVEFORMATEX but without
							//cbSize field
};

struct data
{
  u8		data_id[4];         //contains identifier: "data"
  u32		data_size;			//data size
};

wav_encoded_sound_interface::wav_encoded_sound_interface	(resources::managed_resource_ptr raw_file)
{
	m_wav_file.resource		= raw_file;
	m_wav_file.pointer		= 0;

	read_riff				( );
}

wav_encoded_sound_interface::~wav_encoded_sound_interface	( )
{

}

u32 wav_encoded_sound_interface::decompress					(	pbyte dest,
																u32 pcm_pointer,
																u32& pcm_pointer_after_decompress,
																u32 bytes_needed	)	
{
	resources::pinned_ptr_const<u8> pdata	( m_wav_file.resource );
	memory::reader reader					( pdata.c_ptr( ), pdata.size( ) );

	u32 pcms_needed			= bytes_needed / m_bytes_per_sample;
	u32 count				= static_cast<u32>( m_length_in_pcm - pcm_pointer ) < pcms_needed ? static_cast<u32>( m_length_in_pcm - pcm_pointer ) : pcms_needed;

	reader.seek				( m_wav_file.pointer + pcm_pointer * m_bytes_per_sample );
	reader.r							( dest, bytes_needed, count * m_bytes_per_sample );
	pcm_pointer_after_decompress		= pcm_pointer + count;
	return count * m_bytes_per_sample;
}

void wav_encoded_sound_interface::read_riff					( )
{
	resources::pinned_ptr_const<u8> pdata	( m_wav_file.resource );

	memory::reader reader	( pdata.c_ptr(), pdata.size() );
	riff r; 
	reader.r				( &r, sizeof( riff ), sizeof( riff ) );
	R_ASSERT				( memcmp ( &r.id, "RIFF", sizeof( r.id ) ) == 0 );
	R_ASSERT				( memcmp ( &r.format, "WAVE", sizeof( r.format ) ) == 0 );

	fmt format;
	reader.r				( &format, sizeof( fmt ), sizeof( fmt ) );

	data d;
	reader.r				( &d, sizeof( data ), sizeof( data ) );

	m_wav_file.pointer		= reader.tell( );

	m_samples_per_sec		= format.fmt_format.nSamplesPerSec;
	m_bytes_per_sample		= format.fmt_format.wBitsPerSample >> 3;
	switch ( format.fmt_format.nChannels )
	{
	case 1:	m_channels_type = mono;		break;
	case 2:	m_channels_type = stereo;	break;
	default: NODEFAULT( );
	}

	m_length_in_pcm			= d.data_size / m_bytes_per_sample;
	m_length_in_msec		= ( m_length_in_pcm * 1000 ) / m_samples_per_sec;
}

} // namespace sound
} // namespace xray
