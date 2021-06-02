////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WAV_ENCODED_SOUND_INTERFACE_H_INCLUDED
#define WAV_ENCODED_SOUND_INTERFACE_H_INCLUDED

#include <xray/sound/encoded_sound_interface.h>
#include <xray/sound/wav_utils.h>

namespace xray {
namespace sound {

class wav_encoded_sound_interface : public encoded_sound_interface
{
public:
					wav_encoded_sound_interface	(resources::managed_resource_ptr raw_file);
	virtual			~wav_encoded_sound_interface( );

	virtual	u32		decompress					(	pbyte dest,
													u32 pcm_pointer,
													u32& pcm_pointer_after_decompress,
													u32 bytes_needed
												);
private:
			void	read_riff					( );
private:
	wav_utils::wav_raw_file		m_wav_file;
}; // class wav_encoded_sound_interface

} // namespace sound
} // namespace xray

#endif // #ifndef WAV_ENCODED_SOUND_INTERFACE_H_INCLUDED