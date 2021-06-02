////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "proximity_trigger_input_handler.h"

#ifndef MASTER_GOLD

#include "object_proximity_trigger.h"
#include "proximity_trigger_behaviour.h"
#include "game_world.h"
#include "game.h"
#include <xray/input/world.h>
#include <xray/input/keyboard.h>

namespace stalker2{

proximity_trigger_input_handler::proximity_trigger_input_handler		( proximity_trigger_behaviour& proximity_trigger_behaviour ):
	m_proximity_trigger_behaviour		( proximity_trigger_behaviour ),
	m_is_control_pressed				( false ),
	m_is_attached						( false )
{
}

proximity_trigger_input_handler::~proximity_trigger_input_handler		( )
{

}

void proximity_trigger_input_handler::add_handler ( )
{
	if( m_is_attached )
		return;

	m_proximity_trigger_behaviour.get_proximity_trigger( ).get_game_world( ).get_game( ).input_world( ).add_handler		( *this );
	m_is_attached = true;
}

void proximity_trigger_input_handler::remove_handler ( )
{
	if( !m_is_attached )
		return;

	m_proximity_trigger_behaviour.get_proximity_trigger( ).get_game_world( ).get_game( ).input_world( ).remove_handler	( *this );
	m_is_attached = false;
}

bool proximity_trigger_input_handler::on_keyboard_action ( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETER		( input_world );

	if( action == xray::input::kb_key_up && key == xray::input::key_lcontrol )
		m_is_control_pressed = false;

	if( action == xray::input::kb_key_down && key == xray::input::key_add )
	{
		m_proximity_trigger_behaviour.m_testee_primitive.type = (collision::primitive_type)( ( (u32)m_proximity_trigger_behaviour.m_testee_primitive.type + 1 ) % 4 );
		
		if( m_proximity_trigger_behaviour.m_testee_primitive.type == collision::primitive_cylinder )
			m_proximity_trigger_behaviour.m_testee_primitive.type = collision::primitive_capsule;

		return true;
	}

	if( !( action == xray::input::kb_key_down || action == xray::input::kb_key_hold ) )
		return false;

	if( key == xray::input::key_lcontrol )
	{
		m_is_control_pressed = true;
		return true;
	}

	float3	velocity	= float3( 0, 0, 0 );
	float3	angles		= float3( 0, 0, 0 );
	bool	accept		= false;

	switch (key)
	{
		case xray::input::key_uparrow:
			velocity.z = 10; accept = true; break;

		case xray::input::key_downarrow:
			velocity.z = -10; accept = true; break;
		
		case xray::input::key_leftarrow:
			velocity.x = -10; accept = true; break;

		case xray::input::key_rightarrow:
			velocity.x = 10; accept = true; break;

		case xray::input::key_pgup:
			velocity.y = 10; accept = true; break;

		case xray::input::key_pgdn:
			velocity.y = -10; accept = true; break;


		case xray::input::key_numpad4:
			angles.y = .1f; accept = true; break;

		case xray::input::key_numpad6:
			angles.y = -.1f; accept = true; break;
		
		case xray::input::key_numpad2:
			angles.x = -.1f; accept = true; break;

		case xray::input::key_numpad8:
			angles.x = .1f; accept = true; break;

		case xray::input::key_numpad7:
			angles.z = .1f; accept = true; break;

		case xray::input::key_numpad9:
			angles.z = -.1f; accept = true; break;
	}

	if( m_is_control_pressed )
		velocity *= 10;

	if( accept )
	{
		float4x4 new_transform = m_proximity_trigger_behaviour.get_testee_transform( );
		float3 new_pos = new_transform.c.xyz( ) + velocity * m_proximity_trigger_behaviour.get_proximity_trigger( ).get_game_world( ).last_frame_time_sec( );
		new_transform.c.xyz( ) = float3( 0, 0, 0 );
		new_transform = new_transform * ( math::create_rotation_x( angles.x ) * math::create_rotation_y( angles.y ) * math::create_rotation_z( angles.z ) );
		new_transform.c.xyz( ) = new_pos;

		m_proximity_trigger_behaviour.set_testee_transform( new_transform );

		return true;
	}

	return		false;
}

bool proximity_trigger_input_handler::on_gamepad_action ( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETERS		(action, button, input_world);
	return false;
}

bool proximity_trigger_input_handler::on_mouse_key_action ( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS		(action, button, input_world);
	return false;
}

bool proximity_trigger_input_handler::on_mouse_move ( input::world* input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS		(input_world, x, y, z);
	return false;
}

} // namespace stalker2

#endif //#ifndef MASTER_GOLD