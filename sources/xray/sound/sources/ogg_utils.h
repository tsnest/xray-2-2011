////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_UTILS_H_INCLUDED
#define OGG_UTILS_H_INCLUDED

namespace xray {
namespace sound {

namespace ogg_utils {

int		ov_seek_func		(void *datasource, s64 offset_bytes, int whence);
size_t	ov_read_func		(void *ptr, size_t size_bytes, size_t nmemb, void *datasource);
int		ov_close_func		(void *datasource);
long	ov_tell_func		(void *datasource);

// return number of bytes readed
u32		decompress			(OggVorbis_File* ovf, pbyte dest, u32& pcm_pointer, u32 bytes_needed);

void	encode_sound_file	(	fs_new::native_path_string const& input_file_path,
								fs_new::native_path_string const& output_file_path,	
								fs_new::synchronous_device_interface const & device,
								s16 bits_per_sample,
								u16 number_of_chanels,
								s32 samples_per_second,
								s32 output_bitrate
							);

#ifndef MASTER_GOLD
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
int ogg_encode_impl(fs_new::file_type* input_file,
					fs_new::file_type* output_file,
					fs_new::synchronous_device_interface const & device,
					u16 channels,
					int new_rate,
					int new_bitrate);
#endif //#ifdef XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
#endif // ##ifndef MASTER_GOLD

}; // namespace ogg_utils


struct ogg_file_source
{
	resources::managed_resource_ptr	resource;
	u32								pointer;
};

struct ogg_raw_file
{
	resources::managed_resource_ptr	resource;
	u32								pointer;
};

extern pcstr _converted_local_path;
extern pcstr _sound_sources_path;
extern pcstr _wav_ext;
extern pcstr _ogg_ext;
extern pcstr _options_ext;



} // namespace sound
} // namespace xray

#endif // #ifndef OGG_UTILS_H_INCLUDED
