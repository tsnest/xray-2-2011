////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_GRASPING_PARAMS_H_INCLUDED
#define ANIMATION_GRASPING_PARAMS_H_INCLUDED

#include "grasping_space_params.h"
#include "animation_action_params.h"
//

namespace xray {

namespace animation {
	class i_animation_controller;
} // namespace animation

namespace rtp {

struct animation_grasping_world_params:
	public animation_world_params
{
	animation_grasping_world_params( animation::i_animation_controller &controller );
	animation_grasping_world_params(  );

#ifndef	MASTER_GOLD
	void	save( xray::configs::lua_config_value config )const;
#endif	// MASTER_GOLD

	void	load( const xray::configs::binary_config_value &config );

	float4x4 target_pos;

}; // struct animation_grasping_world_params ;






typedef	 grasping_space_params		animation_grasping_space_params;

inline void	transform_to_param	( const float4x4 &transform,  animation_grasping_space_params& param );





} // namespace rtp
} // namespace xray



#include "animation_grasping_params_inline.h"

#endif // #ifndef ANIMATION_GARSPING_PARAMS_H_INCLUDED