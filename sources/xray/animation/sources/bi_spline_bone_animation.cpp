////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bi_spline_bone_animation.h"

typedef xray::animation::bi_spline_bone_animation::animation_channel_data animation_channel_data;

namespace xray {
namespace animation {

animation_channel_data&	bi_spline_bone_animation::channel( enum_channel_id ch )
{
	return m_data[ ch ]; 
}

animation_channel_data const& bi_spline_bone_animation::channel( enum_channel_id ch )const
{
	return m_data[ ch ];
}
#ifndef MASTER_GOLD
void	bi_spline_bone_animation::save( xray::configs::lua_config_value config  )const 
{

	for (u32 i = channel_translation_x ; i < channel_max; ++i )
		channel((enum_channel_id)(i)).save( config[i] );

}
#endif //  MASTER_GOLD

 void bi_spline_bone_animation::load		( xray::configs::binary_config_value const& config  )
{
	for (u32 i = channel_translation_x ; i < channel_max; ++i )
		channel((enum_channel_id)(i)).load( config[i] );
}

bool	bi_spline_bone_animation::compare( bi_spline_bone_animation const& d )const
{
	XRAY_UNREFERENCED_PARAMETER( d );
	for (u32 i = channel_translation_x ; i < channel_max; ++i )
	{
		
		bi_spline_data<float> const&ch_d0 = channel( enum_channel_id(i) );
		bi_spline_data<float> const&ch_d1 = d.channel( enum_channel_id(i) );
		if ( !ch_d0.compare( ch_d1 ) )
			return false;
	}
	return true;
}

void	bi_spline_bone_animation::write			( stream &file )const
{
	for ( u32 i = channel_translation_x ; i < channel_max; ++i )
		channel( enum_channel_id(i) ).write( file );
}


} // namespace animation
} // namespace xray