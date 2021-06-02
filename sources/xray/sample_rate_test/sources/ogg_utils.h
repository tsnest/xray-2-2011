#pragma once

//#include <iostream>
#include <vorbis/vorbisfile.h>

struct OggVorbis_File;

namespace ogg_utils
{

int		ov_seek_func		(void *datasource, long long offset_bytes, int whence);
size_t	ov_read_func		(void *ptr, size_t size_bytes, size_t nmemb, void *datasource);
int		ov_close_func		(void *datasource);
long	ov_tell_func		(void *datasource);

// return number of bytes readed
unsigned int decompress(OggVorbis_File* ovf, unsigned char* dest, unsigned int& pcm_pointer, unsigned int bytes_needed);

}