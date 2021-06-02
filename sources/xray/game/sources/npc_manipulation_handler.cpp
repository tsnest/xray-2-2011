////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "npc_manipulation_handler.h"
#include "game.h"
#include <xray/input/keyboard.h>
#include <xray/input/world.h>
#include <xray/input/mouse.h>

namespace stalker2 {

npc_manipulation_handler::npc_manipulation_handler	( game& environment ) :
	m_game											( environment )
{
}

npc_manipulation_handler::~npc_manipulation_handler( )
{
}

static bool are_alt_shift_pressed					( xray::input::world* input_world )
{
	return ( ( input_world->get_keyboard()->is_key_down( xray::input::key_lalt ) || input_world->get_keyboard()->is_key_down( xray::input::key_ralt ) ) &&
		   ( input_world->get_keyboard()->is_key_down( xray::input::key_lshift ) || input_world->get_keyboard()->is_key_down( xray::input::key_rshift ) ) );
}

static bool are_ctrl_shift_pressed					( xray::input::world* input_world )
{
	return ( ( input_world->get_keyboard()->is_key_down( xray::input::key_lcontrol ) || input_world->get_keyboard()->is_key_down( xray::input::key_rcontrol ) ) &&
		   ( input_world->get_keyboard()->is_key_down( xray::input::key_lshift ) || input_world->get_keyboard()->is_key_down( xray::input::key_rshift ) ) );
}

bool npc_manipulation_handler::on_keyboard_action	( xray::input::world* input_world, xray::input::enum_keyboard key, xray::input::enum_keyboard_action action )
{
	if ( action != xray::input::kb_key_down )
		return										false;

	switch ( key )
	{
		case xray::input::key_8:
		case xray::input::key_multiply:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.toggle_npc_creation_mode		( );
				return								true;
			}
			break;
		}

		case xray::input::key_1:
		case xray::input::key_numpad1:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.query_npc					( human_npc_neutral, m_game.get_camera_position() );
				return								true;
			}
			break;
		}

		case xray::input::key_2:
		case xray::input::key_numpad2:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.query_npc					( human_npc_aggressive, m_game.get_camera_position() );
				return								true;
			}
			break;
		}

		case xray::input::key_right:
		case xray::input::key_numpad6:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.rotate_selected_npc			( -90.f );
				return								true;
			}
			if ( are_ctrl_shift_pressed( input_world ) )
			{
				m_game.rotate_selected_npc			( -45.f );
				return								true;
			}
			break;
		}

		case xray::input::key_left:
		case xray::input::key_numpad4:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.rotate_selected_npc			( 90.f );
				return								true;
			}
			if ( are_ctrl_shift_pressed( input_world ) )
			{
				m_game.rotate_selected_npc			( 45.f );
				return								true;
			}
			break;
		}

		case xray::input::key_delete:
			m_game.delete_selected_npc				( );
		break;

		case xray::input::key_b:
		{
			if ( are_alt_shift_pressed( input_world ) )
			{
				m_game.assign_behaviour				( );
				return								true;
			}
			break;
		}
	}
	return											false;
}

bool npc_manipulation_handler::on_gamepad_action	(
		xray::input::world* input_world,
		xray::input::gamepad_button button,
		xray::input::enum_gamepad_action action
	)
{
	XRAY_UNREFERENCED_PARAMETERS					( action, button, input_world );
	return											false;
}

bool npc_manipulation_handler::on_mouse_key_action	(
		xray::input::world* input_world,
		xray::input::mouse_button button,
		xray::input::enum_mouse_key_action action
	)
{
	if ( button == xray::input::mouse_button_left && action == xray::input::ms_key_up &&
	   ( input_world->get_keyboard()->is_key_down( xray::input::key_lalt ) || input_world->get_keyboard()->is_key_down( xray::input::key_ralt ) ) )
		m_game.check_selected_npc					( );
	
	return											false;
}

bool npc_manipulation_handler::on_mouse_move		( xray::input::world* input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS					( input_world, x, y, z );
	return											false;
}

} // namespace stalker2