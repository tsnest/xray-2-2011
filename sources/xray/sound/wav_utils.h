////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WAV_UTILS_H_INCLUDED
#define WAV_UTILS_H_INCLUDED

#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace sound {
namespace wav_utils {

struct wav_fmt
{
	s16		format;
	s16		channels;
	s32		samplerate;
	s32		bytespersec;
	s16		align;
	s16		samplesize;
	u32		mask;
};

struct wav_raw_file
{
	resources::managed_resource_ptr	resource;
	u32								pointer;
};
#ifndef MASTER_GOLD
XRAY_SOUND_API configs::lua_config_ptr create_default_config	( fs_new::native_path_string const & wav_file_path,
																  fs_new::synchronous_device_interface const & device );
#endif
} // namespace wav_utils
} // namespace sound
} // namespace xray

#endif // #ifndef WAV_UTILS_H_INCLUDED