////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/encoded_sound_interface.h>

namespace xray {
namespace sound {

encoded_sound_interface::encoded_sound_interface ( ) :
	m_length_in_pcm			( 0 ),
	m_length_in_msec		( 0 ),		
	m_samples_per_sec		( 0 ),
	m_bytes_per_sample		( 0 ),
	m_channels_type			( channels_type_count )
{

}

} // namespace sound
} // namespace xray