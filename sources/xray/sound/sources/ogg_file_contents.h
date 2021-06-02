////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_FILE_CONTENTS_H_INCLUDED
#define OGG_FILE_CONTENTS_H_INCLUDED

#include "ogg_utils.h"
#include <xray/sound/channels_type.h>
#include <xray/sound/encoded_sound_interface.h>

namespace xray {
namespace sound {

class ogg_file_contents : 
							public resources::unmanaged_resource,
							private boost::noncopyable
{
public:
							ogg_file_contents			(resources::managed_resource_ptr raw_file);
	virtual					~ogg_file_contents			( );

			u32				get_length_in_pcm			( ) const;
			u32				get_length_in_msec			( ) const;
			u32				get_samples_per_sec			( ) const;
			channels_type	get_channels_type			( ) const;
			u32				pcm_total					( ) const	{ return m_pcm_total; }
			u32				decompress					( pbyte dest, u32 pcm_pointer, u32& pcm_pointer_after_decompress, u32 bytes_needed );

			ogg_file_source&	file_source				( ) { return m_raw_file; }
private:
	WAVEFORMATEX			m_wfx;
	OggVorbis_File			m_ovf;
	ogg_file_source			m_raw_file;
	u32						m_pcm_total;
}; // class ogg_file_contents

typedef	resources::resource_ptr < ogg_file_contents, resources::unmanaged_intrusive_base > ogg_file_contents_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef OGG_FILE_CONTENTS_H_INCLUDED
