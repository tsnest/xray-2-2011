////////////////////////////////////////////////////////////////////////////
//	Created		: 06.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GRASPING_SPACE_PARAMS_H_INCLUDED
#define GRASPING_SPACE_PARAMS_H_INCLUDED

#include "space_params.h"

#include <xray/math_randoms_generator.h>

namespace xray {

namespace configs {
	class lua_config_value;
} // namespace configs

namespace rtp {

struct test_grasping_world_params
{

			test_grasping_world_params	(const float2 &dir_, const float2 &pos_ );
			test_grasping_world_params	();

	void	transform					(  float rotate, const float2 &translate );

	void	dbg_add_pose_displacement( float3 const&  )
	{
		NODEFAULT();
		//controller_pos.c.xyz() += v;
	}

	void	dbg_get_pose( float3 & ) const
	{
		NODEFAULT();
		//v = controller_pos.c.xyz();
	}

#ifndef	MASTER_GOLD
	void	save						( xray::configs::lua_config_value cfg )const;
#endif	// MASTER_GOLD

	void	load						( const xray::configs::binary_config_value &cfg );
	


	float2	dir;
	float2	pos;
};

class grasping_space_params;
class grasping_tree_space_params;
class test_grasping_action;
class animation_grasping_action;


class grasping_tree_space_params:
	public tree_space_param< 2, grasping_tree_space_params >

{
public:
	typedef	tree_space_param< 2, grasping_tree_space_params > super;

	typedef	animation_grasping_action	action_type;

public:
	grasping_tree_space_params	( const float2 &v ){ target_position	( )	 = v; }
	grasping_tree_space_params	( ) { target_position( ) = float2( math::float_max, math::float_max ); }


	const	float2&		target_position	( ) const	{ return	*( (const float2*)&params ); }
			float2&		target_position	( )			{ return	*( ( float2*)&params ); }

public:
	bool	similar		( const grasping_tree_space_params& other ) const;
	void	render		( const grasping_tree_space_params& to, xray::render::scene_ptr const& scene,  xray::render::debug::renderer& renderer, math::color const& color )const;

public:

	void	transform		(  float rotate, const float2 &translate );

public:

#ifndef	MASTER_GOLD
	void	save				( xray::configs::lua_config_value cfg )const;
#endif	// MASTER_GOLD

	void	load				( const xray::configs::binary_config_value &cfg );

}; // class grasping_space_params


class grasping_space_params:
	public space_param< 1, 1,  grasping_tree_space_params, grasping_space_params >
{

	typedef space_param< 1, 1,  grasping_tree_space_params, grasping_space_params > super;

public:
	grasping_space_params( const float2 &v ){ separate_params( 0 ) = grasping_tree_space_params( v ); }
	grasping_space_params( ){};

public:
	const	float2&		target_position	( ) const	{ return	separate_params( 0 ).target_position( ); }
			float2&		target_position	( )			{ return	separate_params( 0 ).target_position( ); }

public:
	bool	similar		( const grasping_space_params& other ) const { return super::similar( other ); }
	void	render		( const grasping_space_params& to, xray::render::scene_ptr const& scene,  xray::render::debug::renderer& renderer, math::color const& clr)const;

public:

	void	transform		(  float rotate, const float2 &translate );

public:

#ifndef	MASTER_GOLD
	void	save				( xray::configs::lua_config_value cfg )const;
#endif	// MASTER_GOLD

	void	load				( const xray::configs::binary_config_value &cfg );


};



float3 render_pos(const float2& pos );

} // namespace rtp
} // namespace xray

#endif // #ifndef GRASPING_SPACE_PARAMS_H_INCLUDED