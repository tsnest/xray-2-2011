////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "free_fly_camera.h"
#include <xray/console_command.h>

#include "game.h"
#include "game_world.h"
#include "camera_director.h"
#include <xray/input/gamepad.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/world.h>

using xray::input::gamepad;
using xray::input::keyboard;
using xray::input::mouse;

namespace stalker2{

//const console_commands::command_token cameras[] = 
//{
//	{ type_camera_free_fly,				"free_fly" },
//	{ type_camera_terrain_along_fly,	"along_terrain" },
//};
//
//u32 s_type_camera			= type_camera_free_fly;
//u32 s_current_type_camera	= u32(-1);
//console_commands::cc_token camera_cmd( "camera_type", s_type_camera, cameras, array_size( cameras ), true, xray::console_commands::command_type_user_specific  );
//
//float s_terrain_fly_height = 1.8f;
//static console_commands::cc_float s_cmd_terrain_fly_height( "camera_terrain_fly_height", s_terrain_fly_height, 0.2f, 5.f, true, console_commands::command_type_user_specific );

free_fly_camera::free_fly_camera( stalker2::game_world& w ) 
:super				( w ),
m_start_time_ms		( 0 ),
m_mouse_move		( 0,0,0 ),
m_camera_director	( w.get_camera_director() )
{
}

bool free_fly_camera::on_keyboard_action( xray::input::world* input_world, 
										xray::input::enum_keyboard key, 
										xray::input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	if(action == xray::input::kb_key_hold)
		m_keyb_events.push_back	( key );

	if(action == xray::input::kb_key_down && key == xray::input::key_p)
		m_game_world.test_physics1();

	if(action == xray::input::kb_key_down && key == xray::input::key_o)
		m_game_world.test_physics2();

	return false;
}

bool free_fly_camera::on_gamepad_action( xray::input::world* input_world, 
										xray::input::gamepad_button button, 
										xray::input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETER		(action);

	if ( button == xray::input::gamepad_x) {
		input_world->get_gamepad()->set_vibration	( input::gamepad_vibrator_left, input_world->get_gamepad()->get_vibration( input::gamepad_vibrator_left ) + 0.01f );
		return					true;
	}

	if ( button == xray::input::gamepad_b) {
		input_world->get_gamepad()->set_vibration	( input::gamepad_vibrator_right, input_world->get_gamepad()->get_vibration( input::gamepad_vibrator_right ) + 0.01f );
		return					true;
	}
	return	false;
}

bool free_fly_camera::on_mouse_key_action( xray::input::world* input_world, 
										xray::input::mouse_button button, 
										xray::input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	if(action==xray::input::ms_key_hold)
		m_mouse_events.push_back	( button );

	return false;
}

bool free_fly_camera::on_mouse_move( xray::input::world* input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	m_mouse_move.x += x;
	m_mouse_move.y += y;
	m_mouse_move.z += z;
	return false;
}

bool free_fly_camera::keyb_event_present( int e )
{
	return std::find(m_keyb_events.begin(), m_keyb_events.end(), e) != m_keyb_events.end();
}

bool free_fly_camera::mouse_event_present( int e )
{
	return std::find(m_mouse_events.begin(), m_mouse_events.end(), e) != m_mouse_events.end();
}

void free_fly_camera::build_view_matrix( xray::math::float2 const& raw_angles, float const shift_forward, float const shift_right, float const shift_up )
{
	float4x4 const view_inverted	= m_camera_director->get_inverted_view_matrix( );
	float3 const angles_zxy			= view_inverted.get_angles( math::rotation_zxy );

	float3 new_angles_zxy			= float3( raw_angles.x + angles_zxy.x, raw_angles.y+angles_zxy.y, angles_zxy.z );
	new_angles_zxy.x				= math::clamp_r( new_angles_zxy.x, -math::pi_d2, math::pi_d2 );

	float4x4 rotation				= math::create_rotation( new_angles_zxy, math::rotation_zxy );

	float3 const position			=	view_inverted.c.xyz( ) +
										view_inverted.i.xyz( ) * shift_right +
										view_inverted.j.xyz( ) * shift_up +
										view_inverted.k.xyz( ) * shift_forward;

	float4x4 const translation		= math::create_translation( position );

	m_inverted_view_matrix			= rotation * translation;

	//if(s_type_camera== type_camera_terrain_along_fly)
	//{
	//	if(m_game_world.get_collision_tree())
	//	{
	//		collision::ray_objects_type			collision_results( g_allocator );

	//		const float pick_point_height_camera_point = 0.3f;//1000.f;
	//		const float pick_length = 1000.f + pick_point_height_camera_point;
	//		


	//		const float3 pick_point = m_inverted_view_matrix.c.xyz() + float3( 0, pick_point_height_camera_point, 0 );
	//		
	//		const float3 dir( 0, -1, 0 );

	//		bool ray_query_result = m_game_world.get_collision_tree()->ray_query(
	//									collision_object_type_terrain,
	//									pick_point,
	//									dir,
	//									pick_length,  
	//									collision_results, 
	//									collision::objects_predicate_type( )
	//								);

	//		if(ray_query_result)
	//			m_inverted_view_matrix.c.xyz().y = ( pick_point.y - collision_results[0].distance + s_terrain_fly_height );
	//	}	
	//}
}


void free_fly_camera::on_focus( bool b_focus_enter )
{
	super::on_focus ( b_focus_enter );
	if(b_focus_enter)
	{
		m_game_world.get_game().input_world().add_handler						(*this);
	}else
		m_game_world.get_game().input_world().remove_handler(*this);
}

void free_fly_camera::on_activate( camera_director* cd )
{
	super::on_activate		( cd );
	m_start_time_ms			= m_game_world.get_game().time_ms();
	m_inverted_view_matrix	= cd->get_inverted_view_matrix	( );
}

void free_fly_camera::tick( )
{

#if XRAY_PLATFORM_WINDOWS
	u32 const game_time_ms		= m_game_world.get_game().time_ms( );

	ASSERT						( game_time_ms >= m_start_time_ms );
	float const time_delta		= float( game_time_ms - m_start_time_ms );

	float factor				= 60.f * 0.001f * time_delta;
	float angle_factor			= 0.5f;

	m_start_time_ms				= game_time_ms;

	if( m_keyb_events.empty() && 
		m_mouse_events.empty() && 
		math::is_zero(m_mouse_move.x) && math::is_zero(m_mouse_move.y) && math::is_zero(m_mouse_move.z) )
		return;

	if ( keyb_event_present(input::key_lcontrol) || keyb_event_present(input::key_rcontrol) )
		factor					*= 20.f;

	if ( keyb_event_present( input::key_lshift ) || keyb_event_present( input::key_rshift ) )
		factor					*= .1f;

	if ( keyb_event_present( input::key_lalt ) || keyb_event_present( input::key_ralt ) ) 
		angle_factor			*= .1f;

	float						forward = 0.f;
	float						right	= 0.f;
	float						up		= 0.f;

	if ( mouse_event_present(input::mouse_button_left) )
		forward					+= factor*.1f;
	
	if ( mouse_event_present(input::mouse_button_right) )
		forward					-= factor*.1f;

	if ( keyb_event_present(input::key_d) )
		right				+= factor*.1f;
	
	if ( keyb_event_present(input::key_a) )
		right				-= factor*.1f;
	
	if ( keyb_event_present(input::key_w) )
		up					+= factor*.1f;
	
	if ( keyb_event_present(input::key_s) )
		up					-= factor*.1f;

	float const	angle_x			= angle_factor * math::deg2rad( m_mouse_move.y );
	float const	angle_y			= angle_factor * math::deg2rad( m_mouse_move.x ) * 0.75f;
	build_view_matrix			( float2( angle_x, angle_y ), forward, right, up );

	m_keyb_events.clear();
	m_mouse_events.clear();
	m_mouse_move.x = 0.0f;
	m_mouse_move.y = 0.0f;
	m_mouse_move.z = 0.0f;
#else
	raw<gamepad>::ptr const gamepad	= m_game_world.get_game().input_world().get_gamepad();
	if ( !( gamepad->get_state().buttons & input::gamepad_x ) )
		gamepad->set_vibration	( input::gamepad_vibrator_left, gamepad->get_vibration( input::gamepad_vibrator_left ) - .01f );

	if ( !( gamepad->get_state().buttons & input::gamepad_b ) )
		gamepad->set_vibration	( input::gamepad_vibrator_right, gamepad->get_vibration( input::gamepad_vibrator_right ) - .01f );

	ASSERT						( gamepad );
	gamepad::state const& state	= gamepad->get_state ( );
	u32 const game_time_ms		= m_game_world.get_game().time_ms( );
	float const time_delta		= float( game_time_ms - m_start_time_ms );
	m_start_time_ms				= game_time_ms;
	float						factor = 10.f*.001f*time_delta;
	build_view_matrix			(
		float2(
			state.left_thumb_stick.y*.001f,
			state.left_thumb_stick.x*.001f*.75f
		),
		( state.left_trigger - state.right_trigger ) * factor,
		state.right_thumb_stick.x,
		state.right_thumb_stick.y
	);
#endif
}



}// namespace stalker2