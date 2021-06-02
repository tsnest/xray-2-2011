////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_stream.h"

namespace xray {
namespace sound {

sound_stream::sound_stream( resources::managed_resource_ptr raw_file/*, resources::managed_resource_ptr options*/ )
//:m_options( options )
{

	m_raw_file.resource			= raw_file;
	m_raw_file.pointer			= 0;

	memory::zero				(&m_wfx, sizeof(m_wfx));

	ov_callbacks ovc			= {ogg_utils::ov_read_func, ogg_utils::ov_seek_func, ogg_utils::ov_close_func, ogg_utils::ov_tell_func};
 	ov_open_callbacks			(&m_raw_file, &m_ovf, NULL, 0, ovc);

	vorbis_info* ovi			= ov_info(&m_ovf, -1);

	ASSERT						(ovi, "Invalid source info");

#if !XRAY_PLATFORM_PS3
	m_wfx.nSamplesPerSec		= (ovi->rate);
	m_wfx.wFormatTag			= WAVE_FORMAT_PCM;
	m_wfx.nChannels				= u16(ovi->channels);
	m_wfx.wBitsPerSample		= 16;

	m_wfx.nBlockAlign			= (m_wfx.nChannels * m_wfx.wBitsPerSample) / 8;
	m_wfx.nAvgBytesPerSec		= m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
#endif // #if !XRAY_PLATFORM_PS3

	m_pcm_total					= (u32)ov_pcm_total(&m_ovf, -1);
}

u32 sound_stream::decompress( pbyte dest, u32& pcm_pointer, u32 bytes_needed )
{

	u32 res			= ogg_utils::decompress(&m_ovf, dest, pcm_pointer, bytes_needed);
	return			res;
}

sound_stream::~sound_stream( )
{
	ov_clear	( &m_ovf );
//	m_rms.clear	( );
}

} // namespace sound
} // namespace xray
