////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_action.h"
#include <xray/console_command.h>

#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>

namespace xray {
namespace rtp {

u32 navigation_action::super::max_samples = 50;
u32 navigation_theta_action::super::max_samples = 100;

static const float action_transition_reward			= 10;

static const float prohibition_action_transition_reward			= -10000000.f;

float g_ro_factor = 50;
float g_delta_theta_factor = 10;
float g_theta_factor = 0;
float g_ro_theta_factor = 1000;
float g_strafe_discount = 0.f;

static console_commands::cc_float s_cmd_ro_factor( "rtp_ro_factor", g_ro_factor, 0.f, 10000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_delta_theta_factor( "rtp_delta_theta_factor", g_delta_theta_factor, 0.f, 10000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_theta_factor( "rtp_theta_factor", g_theta_factor, 0.f, 10000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_ro_theta_factor( "rtp_ro_theta_factor", g_ro_theta_factor, 0.f, 10000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_strafe_discount( "rtp_strafe_discount", g_strafe_discount, 0.f, 10000.f, true, console_commands::command_type_engine_internal );

float g_torso_factor	= 100;
static console_commands::cc_float s_cmd_torso_factor( "rtp_torso_factor", g_torso_factor, 0.f, 10000.f, true, console_commands::command_type_engine_internal );

float g_gait_state_factor = 1000;
float g_gait_self_transition_reward_factor = 10;
float g_gait_transition_reward_factor = 190;

static console_commands::cc_float s_cmd_gait_state_factor( "rtp_gait_state_factor", g_gait_state_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_gait_self_transition_reward_factor( "rtp_gait_self_transition_reward_factor", g_gait_self_transition_reward_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_gait_transition_reward_factor( "rtp_gait_transition_reward_factor", g_gait_transition_reward_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );

float g_ro_transition_discount_factor = 100;
float g_theta_transition_discount_factor = 100;
float g_tau_transition_discount_factor = 300;

static console_commands::cc_float s_cmd_ro_transition_discount_factor( "rtp_ro_transition_discount_factor", g_ro_transition_discount_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_theta_transition_discount_factor( "rtp_theta_transition_discount_factor", g_theta_transition_discount_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );
static console_commands::cc_float s_cmd_tau_transition_discount_factor( "rtp_tau_transition_discount_factor", g_tau_transition_discount_factor, 0.f, 10000000.f, true, console_commands::command_type_engine_internal );

bool	 g_disable_theta = false;
bool	 g_disable_ro	= false;
bool	 g_disable_tau	= false;

static console_commands::cc_bool s_cmd_disable_theta( "rtp_disable_theta", g_disable_theta, true, console_commands::command_type_engine_internal );
static console_commands::cc_bool s_cmd_disable_ro( "rtp_disable_ro", g_disable_ro, true, console_commands::command_type_engine_internal );
static console_commands::cc_bool s_cmd_disable_tau( "rtp_disable_tau", g_disable_tau, true, console_commands::command_type_engine_internal );


void	navigation_action::cache_blend_transform( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float >  const &from_weights )
{
	super::cache_blend_transform( id_transform, blend, weights, from_action_id, from_weights );
	//m_blend_lin_velocities.push_back( m_animation_action->mean_velocity( weights ) );
}

navigation_params::separate_reward_type	navigation_action::reward( const navigation_action& a ) const
{
	XRAY_UNREFERENCED_PARAMETERS(  &a );

	if ( gait() == animation::gait_take || a.gait() == animation::gait_take )
		return separate_reward_type().set( prohibition_action_transition_reward );

	return separate_reward_type().set(  action_transition_reward );
}

navigation_params::separate_reward_type	navigation_action::reward( const navigation_params& a, u32 blend ) const
{
	XRAY_UNREFERENCED_PARAMETERS( blend );
	
	return separate_reward_type().set( - math::abs( a.theta() ) * 1000 -   a.x() *  1000 ); //math::abs( a.x() * a.x() ) *  1000
}

float	navigation_action::run( navigation_world_params& wfrom, const navigation_params& lfrom, const navigation_action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const
{
	return super::run( wfrom, lfrom, afrom, from_sample, blend_sample, step, time );
}

void	navigation_action::run( navigation_params& from, const navigation_action& af, u32 from_sample, u32 blend_sample  )const
{
	XRAY_UNREFERENCED_PARAMETERS(  &af, &from_sample, &blend_sample );

	float4x4 transform = calculate_transform( af, from_sample, blend_sample );

	//const float3 anles = m_cache_transforms[blend_sample].get_angles_xyz();
	//
	//const float3 disp  = m_cache_transforms[blend_sample].c.xyz();

	const float3 anles = transform.get_angles_xyz();
	
	const float3 disp  = transform.c.xyz();

	from.x()	+= disp.z;
	ASSERT( math::valid( from.x() ) );
	//from.x() -= m_blend_lin_velocities[ blend_sample ];

	from.theta() +=	  math::angle_normalize_signed( anles.y );

	//from.theta() = math::angle_normalize_signed( from.theta() );
	
	//if( from.x() < 0 )
	//{
	//	from.theta() += math::pi;
	//	from.x() = - from.x();
	//}

	//from.theta() = math::angle_normalize_signed( from.theta() );

}

navigation_theta_action::navigation_theta_action( action_base<navigation_theta_action> &ab, animation::i_animation_action  const *animation )
	: super( ab, animation ),
	m_view_motion_angle_min( -math::pi ), m_view_motion_angle_max( math::pi )
	//m_view_angle_min( -math::pi ),		m_view_angle_max( math::pi ),
	//m_motion_angle_min( -math::pi ),	m_motion_angle_max( math::pi ),
	//m_rotation_angle_min( -math::pi ),	m_rotation_angle_max( math::pi ),
{

}

navigation_theta_action::navigation_theta_action( action_base<navigation_theta_action> &ab )
	: super( ab ),
	m_view_motion_angle_min( -math::pi ), m_view_motion_angle_max( math::pi )
	//m_view_angle_min	( -math::pi ),	m_view_angle_max	( math::pi ),
	//m_motion_angle_min	( -math::pi ),	m_motion_angle_max	( math::pi ),
	//m_rotation_angle_min( -math::pi ),	m_rotation_angle_max( math::pi ),
{

}


float	navigation_theta_action::run( navigation_theta_world_params& wfrom, const navigation_theta_params& lfrom, const navigation_theta_action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const
{
	return super::run( wfrom, lfrom, afrom, from_sample, blend_sample, step, time );
}

inline float get_y_angle( float4x4 const & m )
{
	float	inv_scale_x	= 1/sqrt( m.i.xyz().squared_length( ) );
	float	inv_scale_y	= 1/sqrt( m.j.xyz().squared_length( ) );	
	
	float const	horde = sqrt( math::sqr( m.j.z*inv_scale_y ) + math::sqr( m.k.z*inv_scale_x ) );     //x->y ->z

	return atan2( m.i.z*inv_scale_x, horde );
}


void	navigation_theta_action::cache_blend_transform( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights )
{
	super::cache_blend_transform( id_transform, blend, weights, from_action_id, from_weights );

	float4x4 transform =  calculate_transform( blend, weights, from_action_id, from_weights );

	float delta_theta_angle	= math::angle_normalize_signed( get_y_angle( transform ) );

	float ro_angle = 0;
	if(	gait()!= animation::gait_stand &&
		( !math::is_zero(transform.c.x) || !math::is_zero( transform.c.z ) )
	)
		ro_angle = math::angle_normalize_signed( atan2( transform.c.x, -transform.c.z ) );

	m_cache_angles[ id_transform ].delta_theta	=	delta_theta_angle;
	m_cache_angles[ id_transform ].ro			=	ro_angle;

	u32 head_bone_id = m_animation_action->bone_index( "Head" );
	//u32 rh = m_animation_action->bone_index( "RightHand" );

	float4x4 head_bone_matrix;
	m_animation_action->local_bone_matrix( head_bone_matrix, head_bone_id, weights );

	m_cache_angles[ id_transform ].tau = look_angle_in_root( head_bone_matrix );
	//m_animation_action->local_bone_matrix( mr, rh, weights );


	//m_cache_angles[ id_transform ]  = math::angle_normalize_signed( m_cache_transforms[id_transform].get_angles_xyz().y );
	//LOG_INFO( "	 action_id : %d, blend %d pcurrent : %f ", id(), blend, m_cache_angles[ blend ] );	
}

void	navigation_theta_action::cache_transforms				( )
{
	//calc_samples_per_weight	( );
	motion_angles init = { math::infinity, memory::uninitialized_value<float>(), memory::uninitialized_value<float>() };
	m_cache_angles.resize( transforms_per_blend( ) * weights_combinations_count( ), init );
	LOG_DEBUG( "action id: %d; group name: %s; samples: %d total_samples: %d; total_angeles: %d", id(), name(), weights_combinations_count( ), this->get_action_base().total_blend_samples(), m_cache_angles.size()  );
	super::cache_transforms	( );
	
	//m_cache_transforms.clear();
}

#ifndef MASTER_GOLD

void	navigation_theta_action::save_cached_data( configs::lua_config_value cfg ) const
{
	super::save_cached_data( cfg );
	
	ASSERT( m_animation_action );

	const u32 anim_cnt	=	anim_count();
	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * anim_cnt ), anim_cnt );
	
	u32 const cnt = m_cache_angles.size();
	for( u32 i = 0; i < cnt; ++i )
	{
			ASSERT( math::valid( m_cache_angles[ i ].delta_theta ) );
			ASSERT( math::valid( m_cache_angles[ i ].ro ) );
			ASSERT( math::valid( m_cache_angles[ i ].tau ) );
			cfg["angles"][i]["delta_theta"] = m_cache_angles[ i ].delta_theta;
			cfg["angles"][i]["ro"] = m_cache_angles[ i ].ro;
			cfg["angles"][i]["tau"]= m_cache_angles[ i ].tau;
			
			get_weights( i, weights );

			for( u32 j = 0; j < weights.size(); ++j )
				cfg["angles"][i]["weights"][j] = weights[ j ];
	}
}




void	navigation_theta_action::save( xray::configs::lua_config_value cfg ) const
{
	super::save( cfg );
}
#endif // MASTER_GOLD


void	navigation_theta_action::load_config( )
{
	ASSERT( m_animation_action );
	configs::binary_config_value cfg = m_animation_action->config()->get_root();
	if( cfg["animation_group"].value_exists("restrictions") )
	{
		xray::configs::binary_config_value restrictions = cfg["animation_group"]["restrictions"];
		m_view_motion_angle_min = math::deg2rad( restrictions["view_motion_angle_min"] );
		m_view_motion_angle_max = math::deg2rad( restrictions["view_motion_angle_max"] );
	}
	super::load_config();
}

void	navigation_theta_action::load_cached_data( const configs::binary_config_value &cfg )
{
	super::load_cached_data( cfg );

	u32 const cnt = cfg["angles"].size();
	m_cache_angles.resize( cnt );
	for( u32 i = 0; i < cnt; ++i )
	{

		m_cache_angles[ i ].delta_theta = cfg["angles"][i]["delta_theta"];
		m_cache_angles[ i ].ro = cfg["angles"][i]["ro"];
		m_cache_angles[ i ].tau = cfg["angles"][i]["tau"];
	}
}

void	navigation_theta_action::load							( const xray::configs::binary_config_value &cfg )
{
	super::load( cfg );


	//if( !cfg.value_exists("angles") )
	//	return;

	//load_cached_data( cfg );
}

void	navigation_theta_action::debug_render_trajectory	(	math::color const& c, u32 id,
																navigation_theta_world_params& current_world,
																const navigation_theta_params& current,
																separate_reward_type const& current_reward,
																const navigation_theta_params& next,	
																navigation_theta_action const&  next_action,  
																separate_reward_type const& next_reward,
																xray::render::scene_ptr const& scene,
																xray::render::debug::renderer& r
																)const
{
		XRAY_UNREFERENCED_PARAMETERS( &c, id, &next_reward );
		
		//draw trajectory line 
		float4x4 const pose = current_world.controller_pos;

		float3 start_pos = current_world.controller_pos.c.xyz();

		next_action.run_simulate( current_world, current, *this, current.blend_id(), next.blend_id() ); //, id, next_action.duration() 
		
		float3 end_pos = current_world.controller_pos.c.xyz();

		r.draw_arrow( scene, start_pos, end_pos, math::color(255,255,0), math::color(255,0,0) );

		//draw root direction
		float3 const root = -pose.k.xyz();
		//r.draw_arrow( scene, start_pos, start_pos + ( root * 0.1f ), math::color(0,255,0), math::color(255,0,255) );

		//draw desired move direction 
		float4x4 const theta_rotation = math::create_rotation_y( current.theta_ro() );
		float3 const desired_move_dir = theta_rotation.transform_direction( root );
		r.draw_arrow( scene, start_pos, start_pos + ( desired_move_dir * 0.1f ), math::color(255,0,255) , math::color(0,255,0) );

		//draw desired torso direction 
		float4x4 const tau_rotation = math::create_rotation_y( current.theta_ro() - current.tau() );
		float3 const desired_torso_dir = tau_rotation.transform_direction( root );
		r.draw_arrow( scene, start_pos, start_pos + ( desired_torso_dir * 0.1f ), math::color(255,255,255), math::color(255,255,255) );
		
		//draw move direction
		motion_angles angles = next_action.angles( next.blend_id(), this, current.blend_id() );

		float4x4 const ro_rotation = math::create_rotation_y( angles.ro );
		float3 const move_dir = ro_rotation.transform_direction( root );
		r.draw_arrow( scene, start_pos, start_pos + ( move_dir * 0.1f ), math::color(0,0,255), math::color(255,255,0) );
		
		//draw head direction
		//float4x4 head_matrix = current_world.head_matrix();
		//r.draw_arrow( head_matrix.c.xyz(), head_matrix.c.xyz() + head_matrix.k.xyz() * 0.1
		//	, math::color( 0, 255 , 0 ), math::color( 255, 0 , 255 ) );

		float reward_draw_factor = 0.001f;
		
		float res_reward	= current_reward.sum_reward() * reward_draw_factor;
		float move_reward	= current_reward[0]			  * reward_draw_factor;
		float torso_reward	= current_reward[1]			  * reward_draw_factor;
		
		r.draw_line( scene, start_pos, start_pos + res_reward		* float3(0,1,0), math::color( 0, 255, 0 ) );
		r.draw_line( scene, start_pos, start_pos + move_reward		* float3(0,1,0), math::color( 255, 0, 255 ) );
		r.draw_line( scene, start_pos, start_pos + torso_reward	* float3(0,1,0), math::color( 255, 255, 255 ) );
		//float res_reward_ln		= math::log( current_reward.sum_reward() );
		//float move_reward_ln	= math::log( current_reward[0] );
		//float torso_reward_ln	= math::log( current_reward[1] );
};

} // namespace rtp
} // namespace xray
