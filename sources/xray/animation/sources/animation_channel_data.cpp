////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_channel_data.h"

namespace xray {
namespace animation {

	animation_channel_data::animation_channel_data( )
	{

	}

	void	animation_channel_data::save( xray::configs::lua_config_value cfg  )const 
	{
		m_bi_spline.save( cfg );
	}

	void	animation_channel_data::load( xray::configs::lua_config_value const& cfg  )
	{
		m_bi_spline.load ( cfg );
	}

} // namespace animation
} // namespace xray

