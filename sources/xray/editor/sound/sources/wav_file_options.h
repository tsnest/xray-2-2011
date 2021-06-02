////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WAV_FILE_OPTIONS_H_INCLUDED
#define WAV_FILE_OPTIONS_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace sound_editor {
	public ref class conversion_options
	{
	public:
		virtual String^ ToString			() override {return "";};
		[DefaultValueAttribute(0), DisplayNameAttribute("bitrate"), DescriptionAttribute("bitrate")]
		property int bitrate
		{
			int get() {return m_bitrate;};
			void set(int value){m_bitrate=value;};
		}

		[DefaultValueAttribute(0), DisplayNameAttribute("samplerate"), DescriptionAttribute("samplerate")]
		property int samplerate
		{
			int get() {return m_saplerate;}
			void set(int value) {m_saplerate=value;}
		}

	private:
		int m_bitrate;
		int m_saplerate;
	}; // ref class conversion_options

	public ref class wav_file_options
	{
	public:
						wav_file_options	(configs::lua_config_ptr config);
				void	save				(String^ name);
		virtual String^ ToString			() override {return "";};

		[CategoryAttribute("WAV options"), DefaultValueAttribute(0), DisplayNameAttribute("samplerate"), DescriptionAttribute("samplerate")]
		property int samplerate
		{
			int get() {return m_sample_rate;};
			void set(int value) {m_sample_rate=value;};
		}

		[CategoryAttribute("WAV options"), DefaultValueAttribute(0), DisplayNameAttribute("channels_count"), DescriptionAttribute("channels count")]
		property int channels_count
		{
			int get() {return m_channels_count;};
			void set(int value) {m_channels_count=value;};
		}

		[CategoryAttribute("WAV options"), DefaultValueAttribute(0), DisplayNameAttribute("bits_per_sample"), DescriptionAttribute("bits per sample")]
		property int bits_per_sample
		{
			int get() {return m_bits_per_sample;};
			void set(int value) {m_bits_per_sample=value;};
		}

		[CategoryAttribute("WAV options"), DefaultValueAttribute(0), DisplayNameAttribute("bitrate"), DescriptionAttribute("bitrate")]
		property int bitrate
		{
			int get() {return m_bitrate;};
			void set(int value) {m_bitrate=value;};
		}

		[CategoryAttribute("Conversion options"), DisplayNameAttribute("high_quality_options"), DescriptionAttribute("conversion options")]
		property conversion_options^ high_quality_options
		{
			conversion_options^ get() {return m_high_quality_options;};
			void set(conversion_options^ value)	{m_high_quality_options=value;};
		}

		[CategoryAttribute("Conversion options"), DisplayNameAttribute("medium_quality_options"), DescriptionAttribute("conversion options")]
		property conversion_options^ medium_quality_options
		{
			conversion_options^ get	() {return m_medium_quality_options;};
			void set(conversion_options^ value)	{m_medium_quality_options=value;};
		}

		[CategoryAttribute("Conversion options"), DisplayNameAttribute("low_quality_options"), DescriptionAttribute("conversion options")]
		property conversion_options^ low_quality_options
		{
			conversion_options^ get() {return m_low_quality_options;};
			void set(conversion_options^ value)	{m_low_quality_options=value;};
		}

	private:
		int					m_sample_rate;
		int					m_channels_count;
		int					m_bits_per_sample;
		int					m_bitrate;
		conversion_options^	m_high_quality_options;
		conversion_options^	m_medium_quality_options;
		conversion_options^	m_low_quality_options;
	}; // class wav_file_options
} // namespace sound_editor
} // namespace xray
#endif // #ifndef WAV_FILE_OPTIONS_H_INCLUDED