////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_handler_free_fly.h"
#include "game.h"
#include <xray/input/gamepad.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/world.h>



using stalker2::free_fly_handler;

using xray::input::gamepad;
using xray::input::keyboard;
using xray::input::mouse;

using xray::raw;

free_fly_handler::free_fly_handler	( stalker2::game& game ) :
	m_game						( game ),
	m_start_time_ms				( game.time_ms( ) )
{
}

free_fly_handler::~free_fly_handler				( )
{
}

bool free_fly_handler::on_keyboard_action	(	xray::input::world& input_world, 
										xray::input::enum_keyboard key, 
										xray::input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( &input_world );

	if(action == xray::input::kb_key_hold)
		m_keyb_events.push_back	( key );
	return false;
}

bool free_fly_handler::on_gamepad_action	(	xray::input::world& input_world, 
										xray::input::gamepad_button button, 
										xray::input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETER		(action);

	if ( button == xray::input::gamepad_x) {
		input_world.get_gamepad()->set_vibration	( input::gamepad_vibrator_left, input_world.get_gamepad()->get_vibration( input::gamepad_vibrator_left ) + 0.01f );
		return					true;
	}

	if ( button == xray::input::gamepad_b) {
		input_world.get_gamepad()->set_vibration	( input::gamepad_vibrator_right, input_world.get_gamepad()->get_vibration( input::gamepad_vibrator_right ) + 0.01f );
		return					true;
	}
	return	false;
}

bool free_fly_handler::on_mouse_key_action	(	xray::input::world& input_world, 
										xray::input::mouse_button button, 
										xray::input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( &input_world );

	if(action==xray::input::ms_key_hold)
		m_mouse_events.push_back	( button );

	return false;
}

bool free_fly_handler::on_mouse_move		(	xray::input::world& input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS	( &input_world );

	m_mouse_x += x;
	m_mouse_y += y;
	m_mouse_z += z;
	return false;
}

bool free_fly_handler::keyb_event_present( int e )
{
	return std::find(m_keyb_events.begin(), m_keyb_events.end(), e) != m_keyb_events.end();
}

bool free_fly_handler::mouse_event_present( int e )
{
	return std::find(m_mouse_events.begin(), m_mouse_events.end(), e) != m_mouse_events.end();
}

void free_fly_handler::set_view_matrix	( xray::math::float2 const& raw_angles, float const shift_forward, float const shift_right, float const shift_up )
{
	float4x4 const view_inverted	= m_game.get_inverted_view_matrix( );
	float3 const angles				= view_inverted.get_angles_xyz_INCORRECT( );

 	float x_new	= math::clamp_r		( raw_angles.x + angles.x, -math::pi_d2, math::pi_d2);

	float3 const new_angles			= float3( x_new, angles.y + raw_angles.y, angles.z );
	float4x4 rotation				= math::create_rotation_INCORRECT( new_angles );

	float3 const position			=	view_inverted.c.xyz( ) +
										view_inverted.i.xyz( ) * shift_right +
										view_inverted.j.xyz( ) * shift_up +
										view_inverted.k.xyz( ) * shift_forward;

	float4x4 const translation		= math::create_translation( position );

	float4x4	new_matrix = rotation * translation;
	
	m_game.set_inverted_view_matrix	( new_matrix );
}

void free_fly_handler::tick			( input::world& input_world )
{
	XRAY_UNREFERENCED_PARAMETERS	( &input_world );


#if XRAY_PLATFORM_WINDOWS
	u32 const game_time_ms		= m_game.time_ms( );

	ASSERT						( game_time_ms >= m_start_time_ms );
	float const time_delta		= float( game_time_ms - m_start_time_ms )*.001f;

	float factor				= 30.f*time_delta;

	m_start_time_ms				= game_time_ms;

	if( m_keyb_events.empty() && m_mouse_events.empty() && m_mouse_x==0 && m_mouse_x==0 && m_mouse_x==0 )
		return;


	if ( keyb_event_present( input::key_lcontrol ) || keyb_event_present( input::key_rcontrol ) )
		factor					*= 20.f;

	if ( keyb_event_present( input::key_lshift ) || keyb_event_present( input::key_rshift ) )
		factor					*= .1f;

	float						forward = 0.f;
	float						right	= 0.f;
	float						up		= 0.f;

	if ( mouse_event_present(input::mouse_button_left) )
		forward					+= factor*.1f;
	
	if ( mouse_event_present(input::mouse_button_right) )
		forward					-= factor*.1f;

	if ( !keyb_event_present( input::key_lalt ) && !keyb_event_present( input::key_ralt ) ) 
	{
		if ( keyb_event_present(input::key_d) )
			right				+= factor*.1f;
		
		if ( keyb_event_present(input::key_a) )
			right				-= factor*.1f;
		
		if ( keyb_event_present(input::key_w) )
			up					+= factor*.1f;
		
		if ( keyb_event_present(input::key_s) )
			up					-= factor*.1f;
	}
	
	float const	angle_x			= math::deg2rad( float( m_mouse_y/*state.y*/ ) );
	float const	angle_y			= math::deg2rad( float( m_mouse_x/*state.x*/ )*.75f );
	set_view_matrix				( float2( angle_x, angle_y ), forward, right, up );

	m_keyb_events.clear();
	m_mouse_events.clear();
	m_mouse_x=0;
	m_mouse_y=0;
	m_mouse_z=0;
#else
	raw<gamepad>::ptr const gamepad	= input_world.get_gamepad();
	if ( !( gamepad->get_state().buttons & input::gamepad_x ) )
		gamepad->set_vibration	( input::gamepad_vibrator_left, gamepad->get_vibration( input::gamepad_vibrator_left ) - .01f );

	if ( !( gamepad->get_state().buttons & input::gamepad_b ) )
		gamepad->set_vibration	( input::gamepad_vibrator_right, gamepad->get_vibration( input::gamepad_vibrator_right ) - .01f );

	ASSERT						( gamepad );
	gamepad::state const& state	= gamepad->get_state ( );
	u32 const game_time_ms		= m_game.time_ms( );
	float const time_delta		= float( game_time_ms - m_start_time_ms )*.001f;
	m_start_time_ms				= game_time_ms;
	float						factor = 10.f*time_delta;
	set_view_matrix				(
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
