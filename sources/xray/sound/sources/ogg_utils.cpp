////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_utils.h"
#include <xray/fs/file_type_pointer.h>
#include <vorbis/vorbisfile.h>
#include <xray/os_include.h>

namespace xray {
namespace sound {
namespace ogg_utils {

int ov_seek_func(void* datasource, s64 offset_bytes, int whence)	
{
	ogg_file_source* sdata				= (ogg_file_source*)datasource;
	resources::pinned_ptr_const<u8> pdata	( sdata->resource );
	memory::reader	r		(pdata.c_ptr(), pdata.size());
	r.seek					(sdata->pointer);
	switch (whence)
	{
	case SEEK_SET: 
		r.seek		((int)offset_bytes);	 
		break;

	case SEEK_CUR: 
		r.advance	((int)offset_bytes); 
		break;

	case SEEK_END: 
		r.seek		((int)offset_bytes + r.length());
		break;
	}
	sdata->pointer	= r.tell( );
	return 0; 
}

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{ 
	ogg_file_source* sdata				= (ogg_file_source*)datasource;
	resources::pinned_ptr_const<u8> pdata	( sdata->resource );

	memory::reader r		(pdata.c_ptr(), pdata.size());
	r.seek					(sdata->pointer);

	size_t exist_block	= math::max	(0ul, math::floor(r.elapsed()/(float)size));
	size_t read_block	= math::min	(exist_block, nmemb);
	
	size_t bytes_to_read = read_block*size;

	r.r					(ptr, (u32)bytes_to_read, (u32)bytes_to_read);
	sdata->pointer		= r.tell( );
	return				read_block;
}

int ov_close_func(void *datasource)									
{	
	XRAY_UNREFERENCED_PARAMETERS(datasource);
	return				0;
}

long ov_tell_func(void *datasource)									
{	
	ogg_file_source* sdata				= (ogg_file_source*)datasource;
	return sdata->pointer;
}

u32 decompress(OggVorbis_File* ovf, pbyte dest, u32& pcm_pointer, u32 bytes_needed)
{
	int			current_section;
	u32			total_readed	= 0;
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
		else 
		if (ret < 0)
			LOG_ERROR("Error in vorbis bitstream");
		else
			total_readed += ret;
	}
	
	pcm_pointer				= (u32)ov_pcm_tell( ovf );

	return					total_readed;
}

struct wav_fmt
{
	u32 mask;
	s32 samplerate;
	s32 bytespersec;
	s16 channels;
	s16 samplesize;
	s16 format;
	s16 align;
};

struct wavfile
{
	s32 totalsamples;
	s32 samplesread;
	s32 channel_map;
    s32 *channel_permute;
	FILE *f;
	s16 channels;
	s16 samplesize;
};


void encode_sound_file (		fs_new::native_path_string const & input_file_path,
								fs_new::native_path_string const & output_file_path,	
								fs_new::synchronous_device_interface const & device, 
								s16 bits_per_sample,
								u16 number_of_chanels,
								s32 samples_per_second,
								s32 output_bitrate
							)
{
	XRAY_UNREFERENCED_PARAMETERS(&input_file_path, &output_file_path, bits_per_sample, number_of_chanels, samples_per_second, output_bitrate);
#ifndef MASTER_GOLD

	using namespace fs_new;
	file_type_pointer	input_file			(input_file_path, device, file_mode::open_existing, file_access::read);
	if ( !input_file )
	{
		LOG_ERROR							("unable to open file [%s]", input_file_path.c_str());
	}

	file_type_pointer	output_file			(output_file_path, device, file_mode::create_always, file_access::write);
	if ( !output_file )
	{
		LOG_INFO							("unable to open file [%s]", output_file_path.c_str());
	}

#if XRAY_PLATFORM_WINDOWS
	ogg_encode_impl(input_file, output_file, device, number_of_chanels, samples_per_second, output_bitrate);
#endif // #if XRAY_PLATFORM_WINDOWS

#endif // #ifndef MASTER_GOLD
}

void make_sound_rms( fs_new::native_path_string & end_file_path )
{
	XRAY_UNREFERENCED_PARAMETER	( end_file_path );

#ifndef MASTER_GOLD
	//make path's
	fs_new::native_path_string		file_path;

	file_path				= end_file_path;
	file_path.set_length	(file_path.length()-8);
	file_path.append		(".ogg");

#if XRAY_PLATFORM_WINDOWS
	STARTUPINFO				si;
	ZeroMemory				( &si, sizeof(si) );
	si.cb					= sizeof(STARTUPINFO);
	si.wShowWindow			= 0;
	si.dwFlags				= STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION		pi;
	ZeroMemory				( &pi, sizeof(pi) );

	fixed_string512			cl_args;
	cl_args					+= "xray_rms_generator-debug.exe";
	cl_args 				+= " -o=\"";
	cl_args 				+= end_file_path.c_str();
	cl_args 				+= "\" -i=\"";
	cl_args 				+= file_path.c_str();
	cl_args 				+= "\"";

	if(CreateProcess(NULL, cl_args.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		WaitForSingleObject(pi.hProcess, INFINITE);
#endif // XRAY_PLATFORM_WINDOWS

#endif // #ifndef MASTER_GOLD
}

}; // namespace ogg_utils

} // namespace sound
} // namespace xray
