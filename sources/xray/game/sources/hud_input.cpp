////////////////////////////////////////////////////////////////////////////
//	Created		: 01.08.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "hud.h"
#include "game_world.h"
#include "game.h"
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/world.h>

namespace stalker2{

using xray::input::gamepad;
using xray::input::keyboard;
using xray::input::mouse;

bool hud::on_keyboard_action(	xray::input::world* input_world, 
								xray::input::enum_keyboard key, 
								xray::input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	if(action == xray::input::kb_key_hold)
		m_frame_events.m_keyb_events.push_back	( key );
	return false;
}

bool hud::on_gamepad_action( xray::input::world* input_world, 
										xray::input::gamepad_button button, 
										xray::input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world, button, action);
	return	false;
}

bool hud::on_mouse_key_action( xray::input::world* input_world, 
										xray::input::mouse_button button, 
										xray::input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	if(action==xray::input::ms_key_down)
		m_frame_events.m_mouse_events.push_back	( button );

	return false;
}

bool hud::on_mouse_move( xray::input::world* input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	m_frame_events.m_mouse_move.x += x;
	m_frame_events.m_mouse_move.y += y;
	m_frame_events.m_mouse_move.z += z;
	return false;
}

bool hud::frame_events::keyb_event_present( int e )
{
	return std::find(m_keyb_events.begin(), m_keyb_events.end(), e) != m_keyb_events.end();
}

bool hud::frame_events::mouse_event_present( int e )
{
	return std::find(m_mouse_events.begin(), m_mouse_events.end(), e) != m_mouse_events.end();
}

void hud::on_focus( bool b_focus_enter )
{
	super::on_focus ( b_focus_enter );
	if(b_focus_enter)
		m_game_world.get_game().input_world().add_handler(*this);
	else
		m_game_world.get_game().input_world().remove_handler(*this);
}

bool hud::frame_events::empty( ) const
{
	return( m_keyb_events.empty() && 
			m_mouse_events.empty() && 
			math::is_zero(m_mouse_move.x) && 
			math::is_zero(m_mouse_move.y) && 
			math::is_zero(m_mouse_move.z) );
}

void hud::frame_events::reset( )
{
	m_keyb_events.clear	( );
	m_mouse_events.clear( );
	m_mouse_move.set	( 0.0f, 0.0f, 0.0f );
}

void hud::input_tick( )
{
	u32 const game_time_ms		= m_game_world.get_game().time_ms( );

	ASSERT									( game_time_ms >= m_frame_events.m_last_frame_time_ms );
	m_frame_events.m_last_frame_time_delta	= game_time_ms - m_frame_events.m_last_frame_time_ms;

	m_frame_events.m_last_frame_time_ms		= game_time_ms;

	m_crouch = ( m_frame_events.keyb_event_present(input::key_lcontrol) || m_frame_events.keyb_event_present(input::key_rcontrol) );

	m_frame_events.m_onframe_move_fwd		= 0.f;
	m_frame_events.m_onframe_move_right		= 0.f;
	m_frame_events.m_onframe_run_fwd		= 0.f;
	m_frame_events.m_onframe_run_right		= 0.f;
	m_frame_events.m_onframe_turn_x			= 0.f;
	m_frame_events.m_onframe_turn_y			= 0.f;
	m_frame_events.m_onframe_jump			= false;
	m_frame_events.m_onframe_reload			= false;
	m_frame_events.m_onframe_shoot			= false;

	if( m_frame_events.empty() )
		return;

	if ( m_frame_events.keyb_event_present(input::key_s) ) 
	{
		m_frame_events.m_onframe_back = true;
		m_frame_events.m_onframe_move_fwd		-= 1.0f;//factor*.1f;
	} else {
		m_frame_events.m_onframe_back = false;
	}

	if(	m_frame_events.keyb_event_present(input::key_r)) {
		m_frame_events.m_onframe_reload = true;
	}
	if(	m_frame_events.keyb_event_present(input::key_q)) {
		m_frame_events.m_onframe_shoot = true;
	}

	if(	!m_frame_events.keyb_event_present(input::key_lshift)) 
	{
		m_frame_events.m_onframe_shift = false;

		if ( m_frame_events.keyb_event_present(input::key_w) ) 
		{
			m_frame_events.m_onframe_move_fwd		+= 1.0f;//factor*.1f;
		}

		if ( m_frame_events.keyb_event_present(input::key_d))
		{
			m_frame_events.m_onframe_move_right		+= 1.0f;//factor*.1f;
		}

		if ( m_frame_events.keyb_event_present(input::key_a) )
		{
			m_frame_events.m_onframe_move_right		-= 1.0f;//factor*.1f;
		}

	} else {

		m_frame_events.m_onframe_shift = true;

		if ( m_frame_events.keyb_event_present(input::key_w)) 
		{
			m_frame_events.m_onframe_run_fwd		+= 1.0f;//factor*.1f;
		}
		if ( m_frame_events.keyb_event_present(input::key_d)) 
		{
			m_frame_events.m_onframe_run_right		+= 1.0f;//factor*.1f;
		}
		if ( m_frame_events.keyb_event_present(input::key_a)) 
		{
			m_frame_events.m_onframe_run_right		-= 1.0f;//factor*.1f;
		}
	}

	if ( m_frame_events.keyb_event_present(input::key_space) )
		m_frame_events.m_onframe_jump		= true;

	m_frame_events.m_onframe_turn_x	= math::deg2rad( m_frame_events.m_mouse_move.y );
	m_frame_events.m_onframe_turn_y	= math::deg2rad( m_frame_events.m_mouse_move.x ) * 0.75f;

	if(m_frame_events.keyb_event_present(input::key_lshift) && !m_frame_events.keyb_event_present(input::key_s))
	{
		camera_move_on_frame				( float2( m_frame_events.m_onframe_turn_x, m_frame_events.m_onframe_turn_y ), 
													  m_frame_events.m_onframe_run_fwd, 
													  m_frame_events.m_onframe_run_right );
	} else {
		camera_move_on_frame				( float2( m_frame_events.m_onframe_turn_x, m_frame_events.m_onframe_turn_y ), 
													  m_frame_events.m_onframe_move_fwd, 
												      m_frame_events.m_onframe_move_right );
	}
}

void hud::camera_move_on_frame(	math::float2 const& raw_angles, 
							float const move_fwd, 
							float const move_right )
{
	float const linear_factor		= 60.f * 0.001f * m_frame_events.m_last_frame_time_delta * 0.1f;
	float const angle_factor		= 0.5f;

	float3 const angles_zxy			= m_camera_inv_view.get_angles( math::rotation_zxy );

	float3 new_angles_zxy			= float3(	angle_factor*raw_angles.x + angles_zxy.x, 
												angle_factor*raw_angles.y + angles_zxy.y, 
												angles_zxy.z );

	float const x_angle_margin		= 0.001f; // let animation clamps vertical angles math::pi_d8/2.0f;
	new_angles_zxy.x				= math::clamp_r( new_angles_zxy.x, 
													-math::pi_d2+x_angle_margin, 
													math::pi_d2-x_angle_margin );

	float4x4 rotation				= math::create_rotation( new_angles_zxy, math::rotation_zxy );

	float3 const position			=	m_camera_inv_view.c.xyz( ) +
										m_camera_inv_view.i.xyz( ) * (linear_factor*move_right) +
										m_camera_inv_view.k.xyz( ) * (linear_factor*move_fwd);

	float4x4 const translation		= math::create_translation( position );

	m_camera_inv_view				= rotation * translation;
}

}

