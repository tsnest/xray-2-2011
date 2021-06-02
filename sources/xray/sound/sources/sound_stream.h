////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_STREAM_H_INCLUDED
#define SOUND_STREAM_H_INCLUDED

#include "ogg_utils.h"
#include <vorbis/vorbisfile.h>

namespace xray {
namespace sound {

//struct sound_rms : public boost::noncopyable
//{
//				sound_rms	( );
//	void		clear		( );
//
//	float*		m_prms;
//	u32			m_count;
//};

class sound_stream : public resources::unmanaged_resource 
{
public:
							sound_stream	( resources::managed_resource_ptr raw_ogg_file/*, resources::managed_resource_ptr options*/ );
	virtual					~sound_stream	( );

	u32						pcm_total		( ) const	{return m_pcm_total;}
	WAVEFORMATEX const&		wave_format		( )	const	{return m_wfx;}
	u32						decompress		( pbyte dest, u32& pcm_pointer, u32 bytes_needed );
	void					recalc_rms		( );

	//virtual	void			recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

private:
	WAVEFORMATEX			m_wfx;

	ogg_file_source			m_raw_file;
	resources::managed_resource_ptr	m_options;

	OggVorbis_File			m_ovf;
	u32						m_pcm_total;
//	sound_rms				m_rms;
}; // class sound_stream

typedef	resources::resource_ptr<
	sound_stream,
	resources::unmanaged_intrusive_base
> sound_stream_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_STREAM_H_INCLUDED
