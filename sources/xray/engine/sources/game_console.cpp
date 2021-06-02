////////////////////////////////////////////////////////////////////////////
//	Created		: 05.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_console.h"
#include <xray/input/world.h>


namespace xray{
namespace engine{

game_console::game_console( ui::world& uw, input::world& iw, xray::memory::base_allocator& a )
:super			( uw, a ),
m_input_world	( iw )
{
	m_self_deactivate = true;
}

void game_console::tick( )
{
	super::tick	( );
}

void game_console::on_activate( )
{
	super::on_activate			( );
	m_input_world.add_handler	( *this );

}

void game_console::on_deactivate( )
{
	super::on_deactivate		( );
	m_input_world.remove_handler( *this );
}

bool game_console::get_active( ) const
{
	return super::get_active( );
}

bool game_console::on_keyboard_action( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action )
{
	return super::on_keyboard_action( input_world, key, action );
}

bool game_console::on_gamepad_action( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action )
{
	return super::on_gamepad_action( input_world, button, action );
}

bool game_console::on_mouse_key_action( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action )
{
	return super::on_mouse_key_action( input_world, button, action );
}

bool game_console::on_mouse_move( input::world* input_world, int x, int y, int z )
{
	return super::on_mouse_move( input_world, x, y, z );
}

} // engine
} //namespace xray