////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "panning_lut.h"

namespace xray {
namespace sound {

static float pos_to_angle	( u32 pos )
{
	const u32 quadrant_num		= panning_lut::quadrant_num; 
	if( pos < quadrant_num )
		return math::atan( (float)pos / (float)( quadrant_num - pos ) );
	if( pos < 2 * quadrant_num )
		return math::pi_d2 +  math::atan( (float)( pos - quadrant_num ) / (float)( 2 * quadrant_num - pos ));
	if( pos < 3 * quadrant_num )
		return math::atan((float)( pos - 2 * quadrant_num ) / (float)( 3 * quadrant_num - pos )) - math::pi;
	return math::atan((float)( pos - 3 * quadrant_num ) / (float)(4 * quadrant_num - pos)) - math::pi_d2;
}

panning_lut::panning_lut	( channels_type type )
{
	channels	speaker_to_channel	[channels_count];
	float		speaker_angle		[channels_count];
	u32			num_channels		= 0;

	switch ( type )
	{
	case mono:
		{
			speaker_to_channel[0]	= front_center;
			speaker_angle[0]		= 0.0f * math::pi / 180.0f;
			num_channels			= 1;
			break;
		}
	case stereo:
		{
			speaker_to_channel[0]	= front_left;
			speaker_to_channel[1]	= front_right;
			speaker_angle[0]		= -90.0f * math::pi / 180.0f;
            speaker_angle[1]		=  90.0f * math::pi / 180.0f;
			num_channels			= 2;
			break;
		}
	default: NODEFAULT( );
	};

	for( u32 pos = 0; pos < lut_num; pos++ )
    {
	   /* clear all values */
		u32 offset = channels_count * pos;
        for( u32 i = 0; i < channels_count; i++ )
			m_table[ offset + i ] = 0.0f;

        if( type == mono )
        {
            m_table[ offset + speaker_to_channel[0] ] = 1.0f;
            continue;
        }

		/* source angle */
        float theta		= pos_to_angle( pos );

		/* set panning values */
		u32 s = 0;
        for( ; s < num_channels - 1; s++ )
        {
            if( theta >= speaker_angle[s] && theta < speaker_angle[s+1] )
            {
                /* source between speaker s and speaker s+1 */
				float alpha		= math::pi_d2 * ( theta-speaker_angle[s] ) / ( speaker_angle[s+1] - speaker_angle[s] );
				m_table[offset + speaker_to_channel[s]]		= math::cos( alpha );
				m_table[offset + speaker_to_channel[s+1]]	= math::sin( alpha );
				//LOG_DEBUG				("idx = %d", offset + speaker_to_channel[s]);
				//LOG_DEBUG				("idx = %d", offset + speaker_to_channel[s + 1]);
                break;
            }
        }
		if ( s == num_channels - 1 )
        {
            /* source between last and first speaker */
            if ( theta < speaker_angle[0] )
				theta += 2.0f * math::pi;
			float alpha		= math::pi_d2 * ( theta - speaker_angle[s] ) / ( 2.0f * math::pi + speaker_angle[0] - speaker_angle[s] );
			m_table[offset + speaker_to_channel[s]]		= math::cos( alpha );
			m_table[offset + speaker_to_channel[0]]	= math::sin( alpha );
			//LOG_DEBUG				("idx = %d", offset + speaker_to_channel[s]);
			//LOG_DEBUG				("idx = %d", offset + speaker_to_channel[0]);
        }
	}
}

panning_lut::~panning_lut	( )
{

}


} // namespace sound
} // namespace xray
