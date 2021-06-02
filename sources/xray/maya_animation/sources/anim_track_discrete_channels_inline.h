////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace maya_animation {

inline void write(const xray::maya_animation::anim_track_discrete_channels &v, FILE* f )
{
	v.save( f );
}

inline void read( xray::maya_animation::anim_track_discrete_channels &v, FILE* f )
{
	v.load( f );
} 

inline void lua_write( const xray::maya_animation::anim_track_discrete_channels &ch, xray::configs::lua_config_value f )
{
	ch.save( f );
}

inline void lua_read(  xray::maya_animation::anim_track_discrete_channels &ch, xray::configs::lua_config_value const& f )
{
	ch.load( f );
}

} // namespace maya_animation
} // namespace xray