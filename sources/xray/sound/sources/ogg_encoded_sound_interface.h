////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_ENCODED_SOUND_INTERFACE_H_INCLUDED
#define OGG_ENCODED_SOUND_INTERFACE_H_INCLUDED

#include <xray/sound/encoded_sound_interface.h>
#include <xray/sound/channels_type.h>
#include "ogg_utils.h"

namespace xray {
namespace sound {

class ogg_encoded_sound_interface : public encoded_sound_interface
{
public:
					ogg_encoded_sound_interface	(resources::managed_resource_ptr raw_file);
	virtual			~ogg_encoded_sound_interface( );

	virtual	u32		decompress					(	pbyte dest,
													u32 pcm_pointer,
													u32& pcm_pointer_after_decompress,
													u32 bytes_needed
												);
private:
	WAVEFORMATEX			m_wfx;
	OggVorbis_File			m_ovf;
	ogg_raw_file			m_raw_file;
}; // class ogg_encoded_sound_interface

} // namespace sound
} // namespace xray

#endif // #ifndef OGG_ENCODED_SOUND_INTERFACE_H_INCLUDED////////////////////