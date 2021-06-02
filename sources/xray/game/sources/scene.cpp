////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene.h"
#include "game.h"
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/gamepad.h>

namespace stalker2 {

scene::scene( game& g )
:m_game( g )
{
}

void scene::on_activate( )
{
	m_game.input_world().add_handler( *this );
}

void scene::on_deactivate( )
{
	m_game.input_world().remove_handler( *this );
}

bool scene::on_keyboard_action( xray::input::world& input_world, xray::input::enum_keyboard key, xray::input::enum_keyboard_action action )
{
	XRAY_UNREFERENCED_PARAMETERS( &input_world, key, action );
	return false;
}

bool scene::on_gamepad_action( xray::input::world& input_world, xray::input::gamepad_button button, xray::input::enum_gamepad_action action )
{
	XRAY_UNREFERENCED_PARAMETERS( &input_world, button, action );
	return false;
}

bool scene::on_mouse_key_action( xray::input::world& input_world, xray::input::mouse_button button, xray::input::enum_mouse_key_action action )
{
	XRAY_UNREFERENCED_PARAMETERS( &input_world, button, action );
	return false;
}

bool scene::on_mouse_move( xray::input::world& input_world, int x, int y, int z )
{
	XRAY_UNREFERENCED_PARAMETERS( &input_world, x, y, z );
	return false;
}

} // namespace stalker2
