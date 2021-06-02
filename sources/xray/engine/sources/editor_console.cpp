////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_console.h"

namespace xray{
namespace engine{

	editor_console::editor_console( xray::ui::world& uw, xray::memory::base_allocator& a )
:super			( uw, a )
{
	m_self_deactivate = false;
}

void editor_console::tick( )
{
	super::tick	( );
}

void editor_console::on_activate( )
{
	super::on_activate			( );
}

void editor_console::on_deactivate( )
{
	super::on_deactivate		( );
}

bool editor_console::get_active( ) const
{
	return super::get_active( );
}

bool editor_console::on_keyboard_action( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action )
{
	return super::on_keyboard_action( input_world, key, action );
}

bool editor_console::on_gamepad_action( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action )
{
	return super::on_gamepad_action( input_world, button, action );
}

bool editor_console::on_mouse_key_action( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action )
{
	return super::on_mouse_key_action( input_world, button, action );
}

bool editor_console::on_mouse_move( input::world* input_world, int x, int y, int z )
{
	return super::on_mouse_move( input_world, x, y, z );
}

} // engine
} //namespace xray