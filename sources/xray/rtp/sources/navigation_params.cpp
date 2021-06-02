////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_params.h"

#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/animation/i_animation_controller_set.h>

namespace xray {
namespace rtp {

//const navigation_tree_params navigation_tree_params::super::min ( -1.f,	-math::pi );
//const navigation_tree_params navigation_tree_params::super::max ( 2.2f,	math::pi  );

//const navigation_tree_params navigation_tree_params::super::area_min (-1.5f, -math::pi_x2 - 0.1f  );
//const navigation_tree_params navigation_tree_params::super::area_max ( 3.f,	math::pi_x2  + 0.1f   );

navigation_tree_theta_params::navigation_tree_theta_params	( float _theta )
{
	angle() = _theta;
}

//const navigation_tree_theta_params navigation_tree_theta_params::super::min (	-math::pi- 0.1f );
//const navigation_tree_theta_params navigation_tree_theta_params::super::max (	math::pi + 0.1f  );

const navigation_tree_theta_params navigation_tree_theta_params::super::area_min (	-math::pi_x2- 0.1f  );
const navigation_tree_theta_params navigation_tree_theta_params::super::area_max (	math::pi_x2 + 0.1f  );

//const navigation_theta_params navigation_theta_params::super::area_min (	-math::pi - 0.1f, -math::pi_d6 );
//const navigation_theta_params navigation_theta_params::super::area_max (	math::pi + 0.1f, math::pi_d6 );

//const navigation_theta_params navigation_theta_params::super::min (	-math::pi - 0.1f, -math::pi_d6 );
//const navigation_theta_params navigation_theta_params::super::max (	math::pi + 0.1f, math::pi_d6 );

const navigation_theta_params navigation_theta_params::super::min (	-math::pi - 0.1f, 	-math::pi - 0.1f );
const navigation_theta_params navigation_theta_params::super::max (	math::pi + 0.1f, 	math::pi + 0.1f );

navigation_theta_params::navigation_theta_params( float arg_theta, float arg_tau ): super()
{
	tau() = arg_tau;
	theta_ro() = arg_theta;
}

navigation_theta_params::navigation_theta_params( ): super()
{

}

navigation_tree_params::navigation_tree_params(float _x,  float _theta )
{
	x() = _x;
	theta() = _theta;
}

void	navigation_tree_params::render( const navigation_tree_params& to, render::scene_ptr const& scene, xray::render::debug::renderer& r , math::color& c  )const
{
	XRAY_UNREFERENCED_PARAMETERS( &to, &r, &c );

	float2 pos0( x(), theta() );

	float2 pos1( to.x(), to.theta() );

	const float3 p0 = render_pos( pos0 );
	const float3 p1 = render_pos( pos1 );

	r.draw_arrow( scene, p0, p1, c );

}

float2 vector_length_angle( float2 const& controller_direction, float2 const& vec )
{
	
		const float path_val = vec.length();

		if( math::is_zero( path_val ) )
				return float2( 0 , 0);
		
		const float2 path_dir = vec * 1.f/path_val;

		float c = path_dir | controller_direction;
		
		float s = path_dir.x * controller_direction.y - path_dir.y * controller_direction.x;

		float angle = math::atan2( s, c );

		return float2( path_val, angle );
}

float4x4	navigation_world_base_params::head_matrix()const
{
		ASSERT( animation_controller );
		u32 head_idx = animation_controller->action_set().bone_index( "Head" );
		ASSERT( u32(-1) != head_idx );
		float4x4 head_matrix;
		animation_controller->get_bone_world_matrix( head_matrix, head_idx );
		return head_matrix;
}

void	navigation_world_base_params::get_local_params( const navigation_world_base_params& action_start_params, float &x, float &theta, float &tau )const
{
		const navigation_world_base_params& w = *this;

		float4x4 inv_start; inv_start.try_invert( action_start_params.controller_pos );
		float4x4 disp =  w.controller_pos * inv_start;

#ifdef RTP_ANIMATION_SIMULATE_MOVEMENT

		const float2 controller_direction ( -w.controller_pos.k.x, -w.controller_pos.k.z );
#else
		float4x4 real_controller_pos = w.animation_controller->position();
		const float2 controller_direction ( -real_controller_pos.k.x, -real_controller_pos.k.z );
		//controller_direction.normalize();

		float4x4 head_matrix = w.head_matrix();
		const float2 controller_head_direction ( head_matrix.k.x, head_matrix.k.z );
		
		float3 displacement_world = real_controller_pos.c.xyz() - action_start_params.controller_pos.c.xyz();

		float2 motion_direction  = float2( displacement_world.x, displacement_world.z );//( -disp.c.x, -disp.c.z );
		//debug_draw_matrix( head_matrix );
#endif		

		float2  length_angle_root_whith_desired	= vector_length_angle( controller_direction, w.path() );

		float2	length_angle_torso_whith_desired	= vector_length_angle( controller_direction, w.torso());
		float2	length_angle_head_whith_desired		= vector_length_angle( controller_head_direction, w.torso());

		float2  length_angle_motion_in_root = vector_length_angle( motion_direction, controller_direction );

		float2	length_angle_head_whith_motion	= vector_length_angle( controller_head_direction, w.path() );
		float2	length_angle_head_whith_motion1	= vector_length_angle( w.torso(), motion_direction );
		float2	length_angle_head_whith_motion3	= vector_length_angle( w.torso(), w.path() );

		float2	length_angle_head_whith_controller_direction = vector_length_angle( controller_head_direction, controller_direction );

		x		= length_angle_root_whith_desired.x + disp.c.z; 	
		theta	= math::angle_normalize_signed( length_angle_root_whith_desired.y + length_angle_motion_in_root.y );

		tau =	math::angle_normalize_signed( length_angle_head_whith_desired.y );
		//tau =	math::angle_normalize_signed( length_angle_root_whith_desired.y + length_angle_head_whith_controller_direction.y );

/*

		float2  length_angle_root	= vector_length_angle( controller_direction, w.path() );

		float2	length_angle_torso_dbg_from_root	= vector_length_angle( controller_direction, w.torso());
		float2	length_angle_torso	= vector_length_angle( controller_head_direction, w.torso());

		float2  length_angle_motion = vector_length_angle( motion_direction, controller_direction );

		x		= length_angle_root.x + disp.c.z; 	
		theta	= math::angle_normalize_signed( length_angle_root.y + length_angle_motion.y );

		tau =	math::angle_normalize_signed( length_angle_torso.y );
*/
}

void log_gait( animation::e_gait gait )
{
	switch( gait )
	{
		case animation::gait_walk_jump:
		LOG_DEBUG("gait_walk_jump");
		break;
		case animation::gait_stand:
		LOG_DEBUG("gait_stand");
		break;
		case animation::gait_walk:
		LOG_DEBUG("gait_walk");
		break;
		case  animation::gait_walk_fast:
		LOG_DEBUG("gait_walk_fast");
		break;
		case animation::gait_run:
		LOG_DEBUG("gait_run");
		break;
		default:
		LOG_ERROR("gait_unkown!");
	}
}

//void navigation_theta_world_params::path_length_to_gait()
animation::e_gait navigation_world_base_params::path_length_to_gait( float2 const & path, bool jump )
{
	animation::e_gait const max_gait = animation::e_gait( navigation_theta_params::discrete_dimension_size - 1 );
	
	animation::e_gait res_gait = animation::gait_enum_end;

	if( jump && xray::identity( animation::gait_walk_jump <= max_gait ) )
	{
		res_gait = animation::gait_walk_jump;
		//LOG_DEBUG("gait_walk_jump");
	}
	else	if( path.length() < 0.2f )
	{
		res_gait = animation::gait_stand;
		//LOG_DEBUG("gait_stand");
	}
	else if( path.length() < 0.5f )
	{
		res_gait = animation::gait_walk;
		//LOG_DEBUG("gait_walk");
	}
	else if( path.length() < 0.8f )
	{
		res_gait = animation::gait_walk_fast;
		//LOG_DEBUG("gait_walk_fast");
	}
	else
	{
		res_gait = animation::gait_run;
		//LOG_DEBUG("gait_run");
	}

	if( res_gait > max_gait )
		 res_gait = max_gait;

	return res_gait;
}

void	navigation_theta_world_params::from_world( const navigation_theta_world_params& action_start_params, navigation_theta_params &p )const
{
	float dummy_x = 0; 
	get_local_params( action_start_params, dummy_x, p.theta_ro(), p.tau() );
	//p.gait() = action_start_params.gait();
	p.set_gait(  action_start_params.gait() ); 
	log_gait( p.gait() );

}

void	navigation_world_params::from_world( const navigation_world_params& action_start_params, navigation_params &p )const
{
	float dummy;	
	get_local_params( action_start_params, p.x(), p.theta(), dummy );
	p.discrete_value() = 0;
	//p.x() = vel;
}

void	navigation_theta_params::render( const navigation_theta_params&,  xray::render::scene_ptr const&, xray::render::debug::renderer& r, math::color const& color   )const
{
	
	XRAY_UNREFERENCED_PARAMETERS( &r, &color  );
	
	//const float3  render_position_common = float3(0,0,0);

	//const float3  render_position_theta = float3(0,0,0);
	//const float3  render_position_tau = float3(0,0,0);

}


} // namespace rtp
} // namespace xray
