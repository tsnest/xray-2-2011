////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_spl.h>

namespace xray {
namespace sound {

sound_spl::sound_spl	( )
{
}

sound_spl::~sound_spl	( )
{
}

#ifndef MASTER_GOLD
void sound_spl::save	( xray::configs::lua_config_value t_root )
{
//	m_curve_line.save						( t_root["spl"] );
}

void sound_spl::load	( xray::configs::lua_config_value const& t_root )
{
	m_curve_line.load					( t_root );
}
#endif // #ifndef MASTER_GOLD

void sound_spl::load	( xray::configs::binary_config_value const& t_root )
{
	m_curve_line.load					( t_root );
}


} // namespace sound
} // namespace xray
