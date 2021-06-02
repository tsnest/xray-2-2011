////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CHECK_ANIMATION_DATA_H_INCLUDED
#define CHECK_ANIMATION_DATA_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace animation {

class cubic_spline_skeleton_animation;

void	test_data(	configs::binary_config_value	const &animation_data, 
					configs::lua_config_value	const &check_data,
					world &world );

bool	compare(	configs::lua_config_ptr check_data,
					const cubic_spline_skeleton_animation &anim );


} // namespace animation
} // namespace xray

#endif // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

#endif // #ifndef CHECK_ANIMATION_DATA_H_INCLUDED