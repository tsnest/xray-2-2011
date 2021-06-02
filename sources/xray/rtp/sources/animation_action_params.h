////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_ACTION_PARAMS_H_INCLUDED
#define ANIMATION_ACTION_PARAMS_H_INCLUDED

#include <xray/animation/i_animation_controller.h>

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace animation
{
	class i_animation_controller_set;
} // namespace animation

namespace rtp {

class animation_global_actions_params
{

public:
	animation_global_actions_params( xray::animation::i_animation_controller_set *set );
	

public:
	xray::animation::i_animation_controller_set		*m_set;

};

struct animation_world_params
{
	animation_world_params( animation::i_animation_controller &ctrl ): animation_controller( &ctrl ), controller_pos( ctrl.position() )
	{}
//	animation_world_params( const float4x4 &pos_controller ): controller_pos(pos_controller)
//	{}
	animation_world_params( ): animation_controller( 0 ), controller_pos(float4x4().identity()){}
	
#ifndef MASTER_GOLD
	void	save( xray::configs::lua_config_value cfg )const;
#endif // MASTER_GOLD

	void	load( const xray::configs::binary_config_value & );
	
	void	dbg_add_pose_displacement( float3 const& v )
	{
		controller_pos.c.xyz() += v;
	}

	void	dbg_get_pose( float3 &v ) const
	{
		v = controller_pos.c.xyz();
	}

	animation::i_animation_controller	*animation_controller;

//private:
	float4x4							controller_pos;
};

#ifndef	MASTER_GOLD
inline void	animation_world_params::save( xray::configs::lua_config_value v  )const
{
	const float3 angles	= controller_pos.get_angles_xyz();
	const float3 t		= controller_pos.c.xyz();

	v["angles"]		= angles;
	v["translation"]= t;
};
#endif	// MASTER_GOLD	

inline void	animation_world_params::load( const xray::configs::binary_config_value &v )
{
	const float3 angles	= v["angles"];
	const float3 t		= v["translation"];


	controller_pos = create_rotation( angles );
	controller_pos.c.xyz() = t;
	
};


} // namespace rtp
} // namespace xray

#endif // #ifndef ANIMATION_ACTION_PARAMS_H_INCLUDED