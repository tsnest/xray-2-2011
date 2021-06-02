////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "wav_file_options.h"
#include "sound_editor.h"

using namespace xray::configs;
using xray::sound_editor::wav_file_options;

wav_file_options::wav_file_options(lua_config_ptr config)
{
	lua_config_value const& config_root = config->get_root();
	lua_config_value t = config_root["options"]; 

	m_sample_rate = t["sample_rate"];
	m_channels_count = t["channels_number"];
	m_bits_per_sample = t["bits_per_sample"];
	m_bitrate = t["bit_rate"];

	m_high_quality_options = gcnew conversion_options();
	m_high_quality_options->bitrate = t["high_quality"]["bit_rate"];
	m_high_quality_options->samplerate = t["high_quality"]["sample_rate"];

	m_medium_quality_options = gcnew conversion_options();
	m_medium_quality_options->bitrate = t["medium_quality"]["bit_rate"];
	m_medium_quality_options->samplerate = t["medium_quality"]["sample_rate"];

	m_low_quality_options = gcnew conversion_options();
	m_low_quality_options->bitrate = t["low_quality"]["bit_rate"];
	m_low_quality_options->samplerate = t["low_quality"]["sample_rate"];
}

void wav_file_options::save(String^ name)
{
	String^ file_path = gcnew String(sound_editor::sound_resources_path + "single/" + name + ".raw_options");
	unmanaged_string str(file_path);
	lua_config_ptr const& config = create_lua_config(str.c_str());
	lua_config_value root = config->get_root()["options"];

	root["sample_rate"] = m_sample_rate;
	root["channels_number"] = m_channels_count;
	root["bits_per_sample"] = m_bits_per_sample;
	root["bit_rate"] = m_bitrate;

	root["high_quality"]["bit_rate"] = m_high_quality_options->bitrate;
	root["high_quality"]["sample_rate"] = m_high_quality_options->samplerate;

	root["medium_quality"]["bit_rate"] = m_medium_quality_options->bitrate;
	root["medium_quality"]["sample_rate"] = m_medium_quality_options->samplerate;

	root["low_quality"]["bit_rate"] = m_low_quality_options->bitrate;
	root["low_quality"]["sample_rate"] = m_low_quality_options->samplerate;

	config->save( configs::target_sources );
}
