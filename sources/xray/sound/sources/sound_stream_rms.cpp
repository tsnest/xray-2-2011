////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_stream.h"

namespace xray {
namespace sound {

//sound_rms::sound_rms( )
//:m_prms( NULL )
//{
//	clear( );
//}
//
//void sound_rms::clear( )
//{
//	if( m_prms )
//		FREE	( m_prms );
//
//	m_prms		= NULL;
//	m_count		= 0;
//}

void sound_stream::recalc_rms( )
{
	const u32	one_sec_samples	= 44100 * 60;

	u32 samples_discr			= 4410; // avg 0.1sec
//	m_rms.m_count				= math::ceil( ov_pcm_total(&m_ovf, -1) / (float)samples_discr );
	//m_rms.m_prms				= (float*)MALLOC( m_rms.m_count * sizeof(float), "rms data" );
	float rms					= 0.0f;
	
	int							current_section;
	float**						pcm_buffer; 
	u32							curr_samples_counter = 0;

//	u32 rms_idx					= 0;

	for(;;)
	{
		long samples_readed		= ov_read_float(&m_ovf, &pcm_buffer, one_sec_samples, &current_section);

		if(samples_readed)
		{
			float *src			= pcm_buffer[0];// mono (first channel)
			for(long sit=0; sit<samples_readed; ++sit)
			{
				float sample	= src[sit];
				rms				+= sample*sample;
				++curr_samples_counter;

				if ( curr_samples_counter == samples_discr )
				{
					//flush_rms	(rms, curr_samples_counter);
					rms						/= curr_samples_counter;
					rms						= sqrt( rms );
//					m_rms.m_prms[rms_idx++]	= rms;
					rms						= 0.0f;
					curr_samples_counter	= 0;
				}
			}
		}else
		{
			// flush tail
			rms						/= curr_samples_counter;
			rms						= sqrt( rms );
//			m_rms.m_prms[rms_idx++]	= rms;
			rms						= 0.0f;
			curr_samples_counter	= 0;
			break;
		}
	}
	//ASSERT(rms_idx==m_rms.m_count);
}

} // namespace sound
} // namespace xray
