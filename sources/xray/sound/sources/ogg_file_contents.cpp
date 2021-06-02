////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_file_contents.h"

namespace xray
{
namespace sound
{

ogg_file_contents::ogg_file_contents(resources::managed_resource_ptr raw_file)
{
	m_raw_file.resource			= raw_file;
	m_raw_file.pointer			= 0;

	memory::zero				(&m_wfx, sizeof(m_wfx));

	ov_callbacks ovc			= {ogg_utils::ov_read_func, ogg_utils::ov_seek_func, ogg_utils::ov_close_func, ogg_utils::ov_tell_func};
 	ov_open_callbacks			(&m_raw_file, &m_ovf, NULL, 0, ovc);

	vorbis_info* ovi			= ov_info(&m_ovf, -1);

	ASSERT						(ovi, "Invalid source info");

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_wfx.nSamplesPerSec		= (ovi->rate);
	m_wfx.wFormatTag			= WAVE_FORMAT_PCM;
	m_wfx.nChannels				= u16(ovi->channels);
	m_wfx.wBitsPerSample		= 16;

	m_wfx.nBlockAlign			= (m_wfx.nChannels * m_wfx.wBitsPerSample) / 8;
	m_wfx.nAvgBytesPerSec		= m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	m_pcm_total					= (u32)ov_pcm_total(&m_ovf, -1);
}

ogg_file_contents::~ogg_file_contents( )
{
	ov_clear	(&m_ovf);
}

u32	ogg_file_contents::decompress( pbyte dest, u32 pcm_pointer, u32& pcm_pointer_after_decompress, u32 bytes_needed )
{
	u32 res							= ogg_utils::decompress(&m_ovf, dest, pcm_pointer, bytes_needed);
	pcm_pointer_after_decompress	= pcm_pointer;
	return							res;
}

u32 ogg_file_contents::get_length_in_pcm ( ) const
{
	return m_pcm_total;
}

u32 ogg_file_contents::get_length_in_msec ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	return (m_pcm_total * 1000) / m_wfx.nSamplesPerSec;
#else
	NOT_IMPLEMENTED( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

u32  ogg_file_contents::get_samples_per_sec	( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	return m_wfx.nSamplesPerSec;
#else
	NOT_IMPLEMENTED( );
	return 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

channels_type ogg_file_contents::get_channels_type ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	switch (m_wfx.nChannels)
	{
	case 1:	 return mono;
	case 2:	 return stereo;
	default: NODEFAULT();
	}
	return channels_type_count;
#else
	NOT_IMPLEMENTED( );
	return channels_type_count;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

}

} // namespace sound
} // namespace xray