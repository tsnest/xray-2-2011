#include "pch.h"
#include "ogg_utils.h"
#//include <stdlib.h>
//#include <iostream>
#include <vorbis/vorbisfile.h>

namespace ogg_utils
{

	int ov_seek_func(void* datasource, long long offset_bytes, int whence)	
	{
		return fseek((FILE*)datasource, offset_bytes, whence);
	}

	size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
	{ 
		return fread(ptr, size, nmemb, (FILE*)datasource);
	}

	int ov_close_func(void *datasource)									
	{	
		return fclose((FILE*)datasource);
	}

	long ov_tell_func(void *datasource)									
	{	
		
		return ftell((FILE*)datasource);
	}

	unsigned int decompress(OggVorbis_File* ovf, unsigned char* dest, unsigned int& pcm_pointer, unsigned int bytes_needed)
	{
		int				current_section;
		unsigned int	total_readed	= 0;
		long			ret;

		ov_pcm_seek	(ovf, pcm_pointer);

		// Read loop
		while(total_readed < bytes_needed)
		{
			char*	dest_ptr	= (char*)(dest+total_readed);
			ret					= ov_read(	ovf, 
				dest_ptr, 
				bytes_needed-total_readed, 
				0, 
				2, 
				1, 
				&current_section
				);

			// if end of file or read limit exceeded
			if (ret == 0) 
				break;
			else if (ret < 0)
			{
				LOG_ERROR("error in vorbis stream");
			}
			else
				total_readed += ret;
		}

		pcm_pointer				= (unsigned int)ov_pcm_tell( ovf );

		return					total_readed;
	}

}