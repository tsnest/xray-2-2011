////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_ACTION_H_INCLUDED
#define NAVIGATION_ACTION_H_INCLUDED

#include "action_base.h"
#include "animation_action.h"

#include "navigation_params.h"
#include "animation_action_params.h"

#include "dbg_sample_check_res.h"

namespace xray {
namespace rtp {

class navigation_action;

class navigation_theta_action;

template<>
struct global_actions_params< navigation_action >:
	public animation_global_actions_params
{
		global_actions_params< navigation_action >( xray::animation::i_animation_controller_set *set ):
			animation_global_actions_params(set){}
} ;

template<>
struct global_actions_params< navigation_theta_action >:
	public animation_global_actions_params
{
		global_actions_params< navigation_theta_action >( xray::animation::i_animation_controller_set *set ):
			animation_global_actions_params(set){}
} ;


//class navigation_action;

class navigation_action:
	public rtp::animation_action< navigation_params, navigation_world_params, navigation_action >
{
	typedef rtp::animation_action< navigation_params, navigation_world_params, navigation_action >	super;
public:
	navigation_action( action_base<navigation_action> &ab, animation::i_animation_action  const *animation ): super( ab, animation ){};
	navigation_action( action_base<navigation_action> &ab ): super( ab ){};

	inline separate_reward_type	reward						( const navigation_action& a ) const;
	inline separate_reward_type	reward						( const navigation_params& a, u32 blend ) const;

	inline		bool	target							( const navigation_params& )const	{ return false; }
	inline		bool	is_target_action				( )const { return false; }
				void	run								( navigation_params& from, const navigation_action& af, u32 from_sample, u32 blend_sample  )const;
				float	run								( navigation_world_params& wfrom, const navigation_params& lfrom, const navigation_action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const;
	virtual		void	cache_blend_transform			( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights );
	//virtual		void	on_cache_transform				( u32 id_transform, float4x4 const &transform );

	virtual		bool	remove_out_of_area_trajectory	( )const { return false; }

private:
	vector< float >					m_blend_lin_velocities;
}; // class navigation_action

class navigation_theta_action:
	public rtp::animation_action< navigation_theta_params, navigation_theta_world_params, navigation_theta_action >
{
	typedef rtp::animation_action< navigation_theta_params, navigation_theta_world_params, navigation_theta_action >	super;

public:
	struct	motion_angles
	{
		float	delta_theta;
		float	ro;
		float	tau;
	};

public:
	navigation_theta_action( action_base<navigation_theta_action> &ab, animation::i_animation_action  const *animation );
	navigation_theta_action( action_base<navigation_theta_action> &ab );

public:
#ifndef	MASTER_GOLD
	virtual		void	save								( configs::lua_config_value cfg ) const;
	virtual		void	save_cached_data					( configs::lua_config_value cfg ) const;
#endif	// MASTER_GOLD
	
	virtual		void	load_cached_data					( const configs::binary_config_value &cfg );
	virtual		void	load								( const configs::binary_config_value &cfg );
	virtual		void	load_config							( );

public:
	inline		separate_reward_type	transition_reward	( u32 my_blend, const navigation_theta_action& a, u32 to_blend ) const;
	inline		motion_angles			angles				( u32 blend, navigation_theta_action const* from_action,  u32 from_blend ) const;

	inline		separate_reward_type	reward				( const navigation_theta_params& p, u32 blend ) const;

	inline		bool	target								( const navigation_theta_params& )const	{ return false; }
	inline		bool	is_target_action					( )const { return false; }
	inline		bool	is_in_range							( const navigation_theta_params& params )const;

	inline		void	run									( navigation_theta_params& from, const navigation_theta_action& af, u32 from_sample, u32 blend_sample  ) const;


				float	run									( navigation_theta_world_params& wfrom, const navigation_theta_params& lfrom, const navigation_theta_action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const;


	virtual		bool	remove_out_of_area_trajectory		( )const { return false; }

public:
		void	debug_render_trajectory						(	math::color const& c, u32 id,
																navigation_theta_world_params& current_world,
																const navigation_theta_params& current,
																separate_reward_type const& current_reward,
																const navigation_theta_params& next,	
																navigation_theta_action const&  next_action,  
																separate_reward_type const& next_reward,
																xray::render::scene_ptr const& scene,
																xray::render::debug::renderer&
																)const;
private:
	inline		float	jump_reward							( const navigation_theta_params& a )const;
	inline		float	jump_reward							( const navigation_theta_action& a )const;

public:
	virtual		void	cache_transforms					( );

private:
	virtual		void	cache_blend_transform				( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights );
	//virtual		void	on_cache_transform				( u32 id_transform, float4x4 const &transform );
	
	float	m_view_motion_angle_min, m_view_motion_angle_max;
	//float	m_motion_angle_min, m_motion_angle_max;
	//float	m_rotation_angle_min, m_rotation_angle_max;

 	mutable	vector< motion_angles >					m_cache_angles;

}; // class navigation_action

} // namespace rtp
} // namespace xray

#include "navigation_action_inline.h"

#endif // #ifndef NAVIGATION_ACTION_H_INCLUDED