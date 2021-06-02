////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_rms.h>
#include "ogg_utils.h"

namespace xray {
namespace sound {

sound_rms::sound_rms ( resources::managed_resource_ptr const& ogg_raw_file, float samples_discr ) :
	m_samples_discr	( samples_discr )
{
	ogg_file_source				ogg;
	ogg.resource				= ogg_raw_file;
	ogg.pointer					= 0;

	OggVorbis_File				ovf;

	ov_callbacks ovc			= {ogg_utils::ov_read_func, ogg_utils::ov_seek_func, ogg_utils::ov_close_func, ogg_utils::ov_tell_func};
	ov_open_callbacks			(&ogg, &ovf, NULL, 0, ovc);
	
	vorbis_info* ovi			= ov_info(&ovf, -1);
	u32 samples_per_sec			= ovi->rate;
	const u32	one_sec_samples	= samples_per_sec * 60;
	m_length_in_msec			= (ov_pcm_total(&ovf, -1) * 1000) / samples_per_sec;

	u64 samples_discr_in_pcm	= static_cast<u64>(ovi->rate * m_samples_discr);

	m_count						= math::ceil( ov_pcm_total(&ovf, -1) / (float)samples_discr_in_pcm);

	float rms					= 0.0f;
	int							current_section;
	float**						pcm_buffer; 
	u32							curr_samples_counter = 0;

	u32 rms_idx					= 0;



	for(;;)
	{
		long samples_readed		= ov_read_float(&ovf, &pcm_buffer, one_sec_samples, &current_section);

		if(samples_readed)
		{
			float *src			= pcm_buffer[0];// mono (first channel)
			for(long sit=0; sit<samples_readed; ++sit)
			{
				float sample	= src[sit];
				rms				+= sample*sample;
				++curr_samples_counter;

				if ( curr_samples_counter == samples_discr_in_pcm )
				{
					//flush_rms	(rms, curr_samples_counter);
					rms						/= curr_samples_counter;
					rms						= sqrt( rms );
					m_data[rms_idx++]		= rms;
					rms						= 0.0f;
					curr_samples_counter	= 0;
				}
			}
		}else
		{
			// flush tail
			rms						/= curr_samples_counter;
			rms						= sqrt( rms );
			m_data[rms_idx++]		= rms;
			rms						= 0.0f;
			curr_samples_counter	= 0;
			break;
		}
	}
	
	ov_clear			(&ovf);
	R_ASSERT			( rms_idx == m_count );
}

sound_rms::~sound_rms ( )
{

}

sound_rms::rms_value_type sound_rms::get_rms_by_time ( u64 msec ) const
{
	R_ASSERT			( m_length_in_msec >= msec );
	u32 ratio			= math::ceil( m_length_in_msec / (float)m_count );
	u64 index			= msec / ratio;		
	return				m_data[index];
}

u64 sound_rms::find_min_value_time_in_interval	( u64 start_time, u64 end_time, rms_value_type min_value ) const
{
	R_ASSERT			( m_length_in_msec >= end_time );
	R_ASSERT			( start_time < end_time );
	float ratio			= (float)m_length_in_msec /(float)m_count;

	u64 start_index		= (u64)(start_time / ratio);
	u64 end_index		= (u64)(end_time / ratio);

	rms_value_type min		= m_data[++start_index];
	u64 min_index			= start_index;

	for ( ; start_index <= end_index; ++start_index )
	{
		if ( min < min_value || math::is_similar( min, min_value ) )
		{
			min_index	= start_index;
			break;
		}

		if ( m_data[start_index] < min )
		{
			min			= m_data[start_index];
			min_index	= start_index;
		}

	}

	return (u64)(min_index * ratio);
}

//u64 sound_rms::get_min_value_time_in_interval	( u64 start_time, u64 end_time, rms_value_type& min_value ) const
//{
//	R_ASSERT			( m_length_in_msec >= start_time && m_length_in_msec >= end_time );
//	R_ASSERT			( start_time < end_time );
//	u32 ratio			= math::ceil( m_length_in_msec / (float)m_count );
//
//	u64 start_index		= start_time / ratio;
//	u64 end_index		= end_time / ratio;
//
//	rms_value_type min_val	= m_data[start_index];
//	u64 min_index			= start_index++;
//	for ( ; start_index <= end_index; ++start_index )
//	{
//		if ( m_data[start_index] < min_val )
//		{
//			min_val		= m_data[start_index];
//			min_index	= start_index;
//		}
//	}
//
//	min_value			= min_val;
//
//	return min_index * ratio;
//}

} // namespace sound
} // namespace xray
