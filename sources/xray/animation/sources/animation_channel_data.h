////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CHANNEL_DATA_H_INCLUDED
#define ANIMATION_CHANNEL_DATA_H_INCLUDED
#include "bi_spline_data.h"

#include <xray/configs_lua_config.h>
#include <xray/animation/i_animation_channel_data.h>	

namespace xray {
namespace animation {

class animation_channel_data:
	public i_animation_channel_data
{
public:
			animation_channel_data	( );

public:
	virtual void							save					( xray::configs::lua_config_value config  )const ;
	virtual void							load					( xray::configs::lua_config_value const& config  );

public:
	virtual const bi_spline_data< float >	&bi_spline				( )const	{ return m_bi_spline; }
	virtual bi_spline_data< float >			&bi_spline				( )			{ return m_bi_spline; }

private:
	bi_spline_data< float >	m_bi_spline;

}; // class animation_channel_data

} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_CHANNEL_DATA_H_INCLUDED