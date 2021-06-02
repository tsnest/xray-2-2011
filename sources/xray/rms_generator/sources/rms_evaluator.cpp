////////////////////////////////////////////////////////////////////////////
//	Created		: 18.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "rms_evaluator.h"
#include <xray/os_include.h>
#include <audiodefs.h>
#include <vorbis/vorbisfile.h>
#include "xray/buffer_string.h"
#include <xray/core/core.h>
#include <xray/os_include.h>
#include "rms_generator_application.h"
#include "rms_generator_memory.h"
#include <xray/configs.h>

using namespace xray;
using namespace xray::configs;

static xray::command_line::key			s_output_text		("output_text",			"txt", "RMS Generator", "set output file binary mode or text mode. Default binary.");
static xray::command_line::key			s_output_filename	("output_file_name",	"o", "RMS Generator", "set output file name. Default input file name.");
static xray::command_line::key			s_discr_frequency	("discr_frequency",		"d", "RMS Generator", "set discretization frequency(floats per second). Default 10.");

namespace rms_generator
{

	const u32	one_sec_samples	= 44100 * 60;

	float	rms_vec			[128*1024]; //128 kb
	u32		rms_count		= 0;
	float	discretization	= 10;

	void flush_rms(float& in_rms, u32& samples)
	{
		if(!samples)	return;
		//in_rms				= sqrt((in_rms+0.001f)/samples);
		//in_rms				= 20 * log10(in_rms);
		in_rms					/= samples;
		in_rms					= sqrt(in_rms);
//		printf					("[%d]%3.3f\n", rms_count, in_rms);
		rms_vec[rms_count]		= in_rms;
		rms_count++;
		samples					= 0;
		in_rms					= 0.0f;
	}

	


	void save_rms_file(pcstr fn, bool change_extension = true, bool is_text = false)
	{
		char	path[1024];

		strcpy_s(path, sizeof(path), fn);
		if(change_extension)
		{
			size_t sz = strlen(path);
			path[sz-3] = 'r';
			path[sz-2] = 'm';
			path[sz-1] = 's';
		}

		u32 cnt					= rms_count;

		if(is_text)
		{
			FILE* f					= NULL;
			fopen_s					(&f, path, "w");

			for(u32 i=0; i<cnt; ++i)
			{
				char destbuf[64];
				float rms_smpl	= rms_vec[i];

				int		first	=(int)rms_smpl;
				float	second	=(rms_smpl-first);
				_itoa_s			(first, destbuf, 10);
				fputs			(destbuf, f);
				fputs			(",", f);

				for(int j=0; j<5; j++)
				{
					second		-= (int)second;
					second		*=10;
					char c		= static_cast_checked<char>(second+'0');
					fputc		(c, f);
				}
				
				fputs			(";\n", f);
			}

			fclose					(f);
		}
		else
		{
			//open config file and get root of it
			configs::lua_config_ptr		config				= configs::create_lua_config(path);
			configs::lua_config_value		config_options		= config->get_root()["options"];
			
			config_options["rms_discretization"]				= discretization;

			configs::lua_config_value		config_rms			= config_options["rms_data"];

			//store data to config file
			
			for(u32 i=0; i<cnt; ++i)
			{
				config_rms[i]				= rms_vec[i];
			}
			
			//save config file to disk
			config->save();
		}

		rms_count				= 0;
	}

	void process_file(const char * filename)
	{
		OggVorbis_File				ovf;
		FILE* f						= NULL;
		fopen_s						(&f, filename, "rb");
		if(!f)
		{
			printf("error opening %s\n", filename);
			return;
		}
		int res						= ov_open(f, &ovf, NULL, 0);

		WAVEFORMATEX				m_wformat;
		ZeroMemory					( &m_wformat, sizeof( WAVEFORMATEX ) );

		vorbis_info* ovi			= ov_info(&ovf,-1);

		m_wformat.nSamplesPerSec	= (ovi->rate); //44100;
		m_wformat.wFormatTag		= WAVE_FORMAT_PCM;
		m_wformat.nChannels			= u16(ovi->channels);
		m_wformat.wBitsPerSample	= 16; //ovi->bitrate_nominal / 1000

		m_wformat.nBlockAlign		= m_wformat.wBitsPerSample / 8/*bits*/ * m_wformat.nChannels;
		m_wformat.nAvgBytesPerSec	= m_wformat.nSamplesPerSec * m_wformat.nBlockAlign;

		int							current_section;
		long						samples_readed;
		float**						pcm_buffer; 

		u32							curr_samples_counter = 0;

		if(s_discr_frequency.is_set())
			s_discr_frequency.is_set_as_number(&discretization);

		u32 const discr				= math::floor(m_wformat.nSamplesPerSec / ((discretization > 0)?discretization:10)) ; //100 ms by default
		float rms					= 0.0f;
		
		for(;;)
		{
			samples_readed			= ov_read_float(&ovf, &pcm_buffer, one_sec_samples, &current_section);

			if(samples_readed)
			{
				float *src			= pcm_buffer[0];// mono
				for(long sit=0; sit<samples_readed; ++sit)
				{
					float sample	= src[sit];
					rms				+= sample*sample;
					++curr_samples_counter;

					if ( curr_samples_counter == discr )
						flush_rms	(rms, curr_samples_counter);
				}
			}else
			{
				flush_rms(rms, curr_samples_counter);
				break;
			}

		}
		res				= ov_clear(&ovf);
		fclose			(f);

		bool is_text = false;
		if(s_output_text.is_set())
		{
			is_text = true;
		}

		if(s_output_filename.is_set())
		{
			char buffer[256];
			xray::buffer_string output_file_name(&buffer[0], array_size(buffer));

			s_output_filename.is_set_as_string(&output_file_name);
			save_rms_file	(output_file_name.c_str(), false, is_text);
		}
		else
		{
			save_rms_file	(filename, true, is_text);
		}
	}
}//namespace rms_generator