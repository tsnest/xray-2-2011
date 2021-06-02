////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/world.h>

namespace xray {
namespace sound {

float world::m_speed_of_sound	= 0.3432f;

float world::get_speed_of_sound		( ) const
{ 
	return m_speed_of_sound;
}


} // namespace sound
} // namespace xray