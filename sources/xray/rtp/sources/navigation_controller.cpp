////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_controller.h"
#include "navigation_action.h"
#include "animation_controller.h"

#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>

#include <xray/configs.h>
#include <xray/console_command.h>

namespace xray {
namespace rtp {

bool	g_rtp_navigation_control_view_dir = false;
static console_commands::cc_bool rtp_navigation_control_view_dir( "rtp_navigation_control_view_dir", g_rtp_navigation_control_view_dir, true, console_commands::command_type_user_specific );

bool	g_rtp_head_view = false;
static console_commands::cc_bool rtp_head_view( "rtp_head_view", g_rtp_head_view, true, console_commands::command_type_user_specific );


template< class base_type >
navigation_controller< base_type >::navigation_controller	( pcstr name, xray::animation::i_animation_controller_set *set ):
super( name, set ),
m_control_path( float2( 0, 0.3f ) ),
m_control_view_dir( float2( 0, 0.3f ) ),
m_control_jump( false ),
m_dbg_cam_height ( 2.f ),
m_dbg_cam_dist( 5.f )
{
	super::m_dbg_camera_follower.set_object(this);
	super::m_dbg_camera_follower.on_attach();
}

template< class base_type >
navigation_controller< base_type >::navigation_controller	( pcstr name,  animation::world &w, pcstr animation_set_path ):
super( name, w, animation_set_path ),
m_control_path( float2( 0, 0.3f ) ),
m_control_view_dir( float2( 0, 0.3f ) ),
m_control_jump( false )
{
	super::m_dbg_camera_follower.set_object(this);
	super::m_dbg_camera_follower.on_attach();
}
template< class base_type >
navigation_controller< base_type >::navigation_controller	( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg ):
	super				( name, w, animation_set_path, cfg ),
	m_control_path		( float2( 0, 0.3f ) ),
	m_control_view_dir	( float2( 0, 0.3f ) ),
	m_control_jump		( false )
{
	super::m_dbg_camera_follower.set_object(this);
	super::m_dbg_camera_follower.on_attach();
}

template< class base_type >
navigation_controller< base_type >::navigation_controller	( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &config_ptr ):
	super				( name, w, config_ptr ),
	m_control_path		( float2( 0, 0.3f ) ),
	m_control_view_dir	( float2( 0, 0.3f ) ),
	m_control_jump		( false ),
	m_control_gait		( animation::gait_walk )
{
	super::m_dbg_camera_follower.set_object(this);
	super::m_dbg_camera_follower.on_attach();
}

template< class base_type >
navigation_controller< base_type >::~navigation_controller	( )
{

}

template< class base_type >
void	navigation_controller< base_type >::from_world( const world_space_param_type& w, space_param_type &p )const
{
	w.from_world( action_start_world_pos(), p );
}

template< class base_type >
bool	navigation_controller< base_type >::dbg_update_walk( float dt )
{
	super::dbg_update_walk( dt );

	if( !m_debug_wolk_initialized )
		return false;
	
	R_ASSERT( current_action() );

	if( current_action()->gait() == animation::gait_walk_jump )
		m_control_jump = false;

	return update( m_controller_position, dt );
}

template< class base_type >
void	navigation_controller< base_type >::on_new_walk_action( world_space_param_type& w, space_param_type	&params )
{
	w.set_path		( m_control_path );
	w.set_torso	( m_control_view_dir );

	w.set_jump		( m_control_jump );

	w.set_gait(
		m_control_gait
		
	);
	w.controller_pos = w.animation_controller->position();
	
	from_world( w, params );
	clamp_to_range( params );

	set_action_start_world_pos( w );
}

template< class base_type >
void	navigation_controller< base_type >::debug_render( 
		render::scene_ptr const& scene,
		render::scene_renderer& scene_renderer,
		render::debug::renderer& r
	) const
{
	if( !super::m_loaded  )
		return;
	
	super::debug_render( scene, scene_renderer, r );
	
	ASSERT ( super::base().global_params().m_set );

	float4x4 m;
	//super::base().global_params().m_set->get_bone_world_matrix( m, 0 );
	ASSERT( super::m_animation_controller );
	super::m_animation_controller->get_bone_world_matrix( m, 0 );

	const float length_factor = 0.5f;

	const float3 root_pos = m.c.xyz();
	const float3 control_path_end = root_pos + float3(m_control_path.x, 0, m_control_path.y ) * length_factor * 3.f;

	r.draw_arrow( scene, root_pos, control_path_end, math::color( 255, 0 , 255 ), math::color( 0, 255, 0 ) );

	const float3 root_dir_end = m.c.xyz() - float3( m.k.x, 0, m.k.z ) * length_factor * 0.8f;

	u32 head_idx = super::m_animation_controller->action_set().bone_index( "Head" );
	float4x4 head_matrix;
	super::m_animation_controller->get_bone_world_matrix( head_matrix, head_idx );
	
	if( !g_rtp_head_view )
		r.draw_arrow( scene, root_pos, root_dir_end, math::color( 0, 255 , 0 ), math::color( 255, 0 , 255 ) );
	else
		r.draw_arrow( scene, head_matrix.c.xyz(), head_matrix.c.xyz() + head_matrix.k.xyz() * length_factor
			, math::color( 0, 255 , 0 ), math::color( 255, 0 , 255 ) );
	
	const float3 view_dir_end = root_pos + float3(m_control_view_dir.x, 0, m_control_view_dir.y ) * length_factor * 3.f;;
	
	r.draw_arrow( scene, root_pos, view_dir_end, math::color( 255, 255 , 255 ), math::color( 255, 255, 255 ) );

}

static void	vector_path_from_input_vector( float2 &move, float2 &res_vector )
{
	float path_mag = res_vector.length();
	
	float ipath_mag = 1.f / path_mag;

	float2 dir = res_vector * ipath_mag;
	
	float2 cros_dir = float2().cross_product( dir );

	res_vector += cros_dir * move.x;
	
	res_vector.normalize();
	res_vector *= path_mag;
	
	res_vector += dir * move.y;

	const float max_path_length = 1.f;
	const float min_path_length = 0.1f;

	float mag = res_vector.length();

	float mag_clamped = math::clamp_r( mag, min_path_length, max_path_length );
	
	res_vector *= mag_clamped/mag;

}

void get_input_vector( float2& move, input::world& input_world )
{
	const float factor = 0.01f;

	non_null<input::keyboard const>::ptr const keyboard	= input_world.get_keyboard();

	if ( keyboard->is_key_down( input::key_leftarrow ) )
		move.x				-= factor;
	
	if ( keyboard->is_key_down( input::key_rightarrow ) )
		move.x				+= factor;
	
	if ( keyboard->is_key_down( input::key_uparrow ) )
		move.y				+= factor * 0.2f;
	
	if ( keyboard->is_key_down( input::key_downarrow ) )
		move.y				-= factor* 0.2f;

}

void get_input_control_vectors( float2& move, float2& view, input::world& input_world )
{
	non_null<input::keyboard const>::ptr const keyboard	= input_world.get_keyboard();

	if ( keyboard->is_key_down( input::key_lcontrol ) )
		get_input_vector( view, input_world );
	else
		get_input_vector( move, input_world );
}

extern bool b_camera_control;
template< class base_type >
void	navigation_controller< base_type >::dbg_move_control			( float2 const& move_direction, float2 const& torso_direction, bool jump )
{
	animation::e_gait	gait = world_space_param_type::path_length_to_gait( m_control_path, m_control_jump );
	move_control( move_direction, torso_direction, gait, jump );
}

template< class base_type >
void	navigation_controller< base_type >::move_control( float2 const& move_direction, float2 const& torso_direction, animation::e_gait gait, bool jump )
{
	XRAY_UNREFERENCED_PARAMETERS( &torso_direction );
	m_control_path		= move_direction;
	m_control_view_dir	= torso_direction;
	m_control_gait		= gait;
	m_control_jump		= jump;
}

template< class base_type >
void	navigation_controller< base_type >::dbg_move_control( float4x4 &view_inverted, input::world& input_world )
{
	R_ASSERT						( view_inverted.valid() );

	non_null<input::keyboard const>::ptr const keyboard	= input_world.get_keyboard();

	float2 move = float2(0,0);
	
	float2 view_dir = float2(0,0);

	get_input_control_vectors( move, view_dir, input_world );
	
	vector_path_from_input_vector( move, m_control_path );

	if ( g_rtp_navigation_control_view_dir )
		vector_path_from_input_vector( view_dir, m_control_view_dir );
	else
		m_control_view_dir = m_control_path;

	if( keyboard->is_key_down( input::key_space ) )
		m_control_jump = true;

	m_control_gait = world_space_param_type::path_length_to_gait( m_control_path, m_control_jump );

	if( b_camera_control )
		super::m_dbg_camera_follower.move_control( view_inverted, input_world );
}

#ifndef MASTER_GOLD
template< class base_type >
void	navigation_controller< base_type >::save( xray::configs::lua_config_value cfg, bool training_trees )const
{
	
	super::save( cfg, training_trees );
	save_settings( cfg );

}
#endif // MASTER_GOLD
template< class base_type >
void	navigation_controller< base_type >::load( const xray::configs::binary_config_value &cfg )
{
	super::load( cfg );
	load_settings( cfg );
}

	extern float g_ro_factor;
	extern float g_delta_theta_factor;
	extern float g_theta_factor;
	extern float g_gait_state_factor;
	extern float g_ro_theta_factor;
	extern float g_torso_factor;

	extern float g_gait_state_factor;
	extern float g_gait_self_transition_reward_factor;
	extern float g_gait_transition_reward_factor;

	extern float g_ro_transition_discount_factor;
	extern float g_theta_transition_discount_factor;


#ifndef MASTER_GOLD
template< class base_type >
void	navigation_controller< base_type >::save_settings	( xray::configs::lua_config_value config )const
{

 xray::configs::lua_config_value cfg = config["action_settings"];

cfg ["ro_factor"]							= g_ro_factor;
cfg ["delta_theta_factor"]					= g_delta_theta_factor;
cfg ["theta_factor"]						= g_theta_factor;
cfg ["gait_state_factor"]					= g_gait_state_factor;
cfg ["ro_theta_factor"]						= g_ro_theta_factor;
cfg ["torso_factor"]						= g_torso_factor;

cfg ["gait_state_factor"]					= g_gait_state_factor;
cfg ["gait_self_transition_reward_factor"]	= g_gait_self_transition_reward_factor;
cfg ["gait_transition_reward_factor"]		= g_gait_transition_reward_factor;

cfg ["ro_transition_discount_factor"]			= g_ro_transition_discount_factor;
cfg ["theta_transition_discount_factor"]		= g_theta_transition_discount_factor;

//global learn parameters; not restored on load
cfg ["learn_max_trajectory_length"]			= g_max_trajectory_length;
cfg ["learn_discount"]						= g_discount;
cfg ["learn_num_trajectories_per_action"]	= g_num_trajectories;
}
#endif // MASTER_GOLD

template< class base_type >
void	navigation_controller< base_type >::load_settings	( const xray::configs::binary_config_value &config )
{

if(!config.value_exists("action_settings"))
		return;

xray::configs::binary_config_value cfg = config["action_settings"];

g_ro_factor								= cfg ["ro_factor"];					
g_delta_theta_factor					= cfg ["delta_theta_factor"];	
g_theta_factor							= cfg ["theta_factor"];			
g_gait_state_factor						= cfg ["gait_state_factor"];
g_ro_theta_factor						= cfg ["ro_theta_factor"];		
g_torso_factor							= cfg ["torso_factor"];			

g_gait_state_factor						= cfg ["gait_state_factor"];
g_gait_self_transition_reward_factor	= cfg ["gait_self_transition_reward_factor"];
g_gait_transition_reward_factor			= cfg ["gait_transition_reward_factor"];

g_ro_transition_discount_factor			= cfg ["ro_transition_discount_factor"];
g_theta_transition_discount_factor		= cfg ["theta_transition_discount_factor"];

}

typedef navigation_controller< animation_controller< walking_controller< navigation_theta_action > > > walking_navigation_controller;
typedef navigation_controller< animation_controller< learning_controller< navigation_theta_action > > > learning_navigation_controller;

typedef learning_navigation_controller navigation_controller_type;

base_controller* create_learning_navigation_controller( pcstr name, animation::world &w, pcstr animation_set_path )
{
	return NEW( learning_navigation_controller )( name, w, animation_set_path );
}

base_controller* create_navigation_learning_controller( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg )
{
	return  NEW( learning_navigation_controller )( name, w, cfg );
}

base_controller* create_navigation_walking_controller( pcstr name, animation::world &w, pcstr animation_set_path )
{
	return NEW( walking_navigation_controller )( name, w, animation_set_path );
}

base_controller* create_navigation_walking_controller( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg )
{
	return  NEW( walking_navigation_controller )( name, w, cfg );
}

//#if ( XRAY_RTP_LEARNING )
//#	define learning_navigation_controller	navigation_controller_type;
//#else  // #if ( XRAY_RTP_LEARNING )
//#	define walking_navigation_controller	navigation_controller_type;
//#endif // #if ( XRAY_RTP_LEARNING )


base_controller* create_navigation_controller( pcstr name, animation::world &w, pcstr animation_set_path )
{
	return NEW( navigation_controller_type )( name, w, animation_set_path );
}

base_controller* create_navigation_controller( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg )
{
	return NEW( navigation_controller_type )( name, w, animation_set_path, cfg );
}

base_controller* create_navigation_controller( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg )
{
	return  NEW( navigation_controller_type )( name, w, cfg );
}

template< class base_type >
void navigation_controller< base_type >::on_attach	( )
{
	super::m_dbg_camera_follower.set_object( this );
	super::m_dbg_camera_follower.on_attach	( );
}

} // namespace rtp
} // namespace xray
