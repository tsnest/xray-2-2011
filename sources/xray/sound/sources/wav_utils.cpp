////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/wav_utils.h>
#include <xray/fs/file_type_pointer.h>

namespace xray {
namespace sound {
namespace wav_utils {

pcstr _wav_ext					= ".wav";
pcstr _options_ext				= ".raw_options";

static u32 read_u32_le( u8* buf )
{
	return (((buf)[3]<<24)|((buf)[2]<<16)|((buf)[1]<<8)|((buf)[0]&0xff));
}

static u16 read_u16_le( u8* buf )
{
	return (((buf)[1]<<8)|((buf)[0]&0xff));
}

s32 wav_id( u8* buf, s32 len )
{

	
	if( len < 12 ) return 0; /* Something screwed up */

	if( memcmp( buf, "RIFF", 4 ) )
		return 0; /* Not wave */

	read_u32_le(buf+4); /* We don't use this */

	if( memcmp(buf+8, "WAVE",4) )
		return 0; /* RIFF, but not wave */

	return 1;
}

static s32 seek_forward(fs_new::file_type * in, fs_new::synchronous_device_interface const & device, u32 length)
{
	if(device->seek(in, length, fs_new::seek_file_current))
	{
		/* Failed. Do it the hard way. */
		u8 buf[1024];
		u32 seek_needed = length;
		s32 seeked;
		while(seek_needed > 0)
		{
			seeked = (s32)device->read(in, buf, seek_needed>1024?1024:seek_needed);
			if(!seeked)
				return 0; /* Couldn't read more, can't read file */
			else
				seek_needed -= seeked;
		}
	}
	return 1;
}

static u32 find_wav_chunk(fs_new::file_type *in, fs_new::synchronous_device_interface const & device, char* type, u32 *len)
{
	unsigned char buf[8];
	unsigned int result = 1;

	while(result)
	{
		if(device->read(in, buf, 8) < 8) /* Suck down a chunk specifier */
		{
			LOG_ERROR("Warning: Unexpected EOF in reading WAV header\n");
			result = 0;
			return 0; /* EOF before reaching the appropriate chunk */
		}

		if(memcmp(buf, type, 4))
		{
			*len = read_u32_le(buf+4);
			if(!seek_forward(in, device, *len))
			{
				result = 0;
				return 0;
			}

			buf[4] = 0;
			LOG_ERROR("Skipping chunk of type \"%s\", length %d\n", buf, *len);
		}
		else
		{
			*len = read_u32_le(buf+4);
			result = 0;
			return 1;
		}
	}
	return 0;
}

wav_fmt get_wav_format ( fs_new::native_path_string const & wav_file_path, 
						 fs_new::synchronous_device_interface const & device )
{
	using namespace fs_new;
	wav_fmt format;

	file_type_pointer	wav_file			( wav_file_path, device, file_mode::open_existing, file_access::read );
	R_ASSERT								( wav_file );

	{
		unsigned char buf[12];
		s32 buf_filled			= 0;
		s32 ret					= 0;

		ret						= (s32)device->read( wav_file, buf, 12 );
		buf_filled				+= ret;

		u32 const result		= wav_id(buf, buf_filled);
		R_ASSERT_U				( result == 1 );

	}

	unsigned int len		= 0;
	unsigned char buf[40];

	u32 const result	= find_wav_chunk(wav_file, device, "fmt ", &len);
	R_ASSERT_U			( result, "Error: invalid wav file" );
	
	R_ASSERT			( len == 16 || len == 18 || len == 40, "INVALID format chunk in wav header." );

	size_t const bytes_readed	= device->read( wav_file, buf, len );
	R_ASSERT_U			( bytes_readed == len );


	format.format		= read_u16_le(buf); 
	format.channels		= read_u16_le(buf+2); 
	format.samplerate	= read_u32_le(buf+4);
	format.bytespersec	= read_u32_le(buf+8);
	format.align		= read_u16_le(buf+12);
	format.samplesize	= read_u16_le(buf+14);
	

	return format;
}

#ifndef MASTER_GOLD
configs::lua_config_ptr create_default_config ( fs_new::native_path_string const & wav_file_path,
											    fs_new::synchronous_device_interface const & device )
{
	wav_fmt format								= get_wav_format( wav_file_path, device );

	fs_new::native_path_string	options_path	= wav_file_path;
	options_path.replace						( _wav_ext, _options_ext );

	configs::lua_config_ptr	config				= configs::create_lua_config( options_path.c_str() );
	configs::lua_config_value config_options	= config->get_root()["options"];
	config_options["name"]						= "sound";
	config_options["channels_number"]			= format.channels;
	config_options["bits_per_sample"]			= format.samplesize;
	config_options["sample_rate"]				= format.samplerate;
	config_options["bit_rate"]					= format.bytespersec / 1024;
	config_options["high_quality"]["sample_rate"] = format.samplerate;
	config_options["high_quality"]["bit_rate"]	= format.bytespersec >> 10;

	if ( format.samplerate >= 44100 )
	{
		config_options["medium_quality"]["sample_rate"]	= format.samplerate >> 1;
		config_options["medium_quality"]["bit_rate"]	= format.bytespersec >> 11;
		config_options["low_quality"]["sample_rate"]	= format.samplerate >> 2;
		config_options["low_quality"]["bit_rate"]		= format.bytespersec >> 12;

	}
	else if ( format.samplerate >= 22050 )
	{
		config_options["medium_quality"]["sample_rate"]	= format.samplerate >> 0;
		config_options["medium_quality"]["bit_rate"]	= format.bytespersec >> 10;
		config_options["low_quality"]["sample_rate"]	= format.samplerate >> 1;
		config_options["low_quality"]["bit_rate"]		= format.bytespersec >> 11;

	}
	else
	{
		config_options["medium_quality"]["sample_rate"]	= format.samplerate;
		config_options["medium_quality"]["bit_rate"]	= format.bytespersec >> 10;
		config_options["low_quality"]["sample_rate"]	= format.samplerate;
		config_options["low_quality"]["bit_rate"]		= format.bytespersec >> 10;

	}
	
	config->save								( configs::target_sources );
	return config;
}
#endif // #ifndef MASTER_GOLD

} // namespace wav_utils
} // namespace sound
} // namespace xray
