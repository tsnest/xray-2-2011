////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bullet_manager_input_handler.h"
#include "bullet_manager.h"
#include "game.h"
#include "game_world.h"
#include "game_camera.h"
#include <xray/input/world.h>
#include <xray/input/keyboard.h>

namespace stalker2{

bullet_manager_input_handler::bullet_manager_input_handler	( bullet_manager& bullet_manager )
:m_bullet_manager( bullet_manager )
{
	m_bullet_manager.get_game( ).input_world( ).add_handler( *this );
}

bullet_manager_input_handler::~bullet_manager_input_handler	( )
{
}

bool bullet_manager_input_handler::on_keyboard_action( xray::input::world* input_world, xray::input::enum_keyboard key, xray::input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETER		(input_world);

	if( action!=xray::input::kb_key_down )
		return false;

	switch (key)
	{
		case xray::input::key_f:
		{
			float4x4 view_transform = m_bullet_manager.get_game( ).get_game_world( ).get_camera_director( )->get_active_camera( )->get_inverted_view_matrix( );
			m_bullet_manager.fire( view_transform.c.xyz( ), view_transform.k.xyz( ) * 900 );
			return			true;
		}
		case xray::input::key_g:
		{
#ifndef MASTER_GOLD

			m_bullet_manager.toggle_is_fixed( );
			return			true;

#endif // #ifndef MASTER_GOLD
		}
	}
	return		false;
}

bool bullet_manager_input_handler::on_gamepad_action	( xray::input::world* input_world, xray::input::gamepad_button button, xray::input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETERS		(action, button, input_world);
	return false;
}

bool bullet_manager_input_handler::on_mouse_key_action	( xray::input::world* input_world, xray::input::mouse_button button, xray::input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS		(action, button, input_world);
	return false;
}

bool bullet_manager_input_handler::on_mouse_move		( xray::input::world* input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS		(input_world, x, y, z);
	return false;
}

} //namespace stalker2