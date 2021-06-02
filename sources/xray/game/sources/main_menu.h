////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MAIN_MENU_H_INCLUDED
#define MAIN_MENU_H_INCLUDED

#include "game_scene.h"

namespace stalker2 {

struct main_menu_ui;
class game;
class main_menu :
	public game_scene,
	public xray::input::handler
{
	typedef			game_scene					super;

public:
					main_menu				( game& w );
	virtual			~main_menu				( );
public:
	virtual void	on_activate				( );
	virtual void	on_deactivate			( );
	virtual void	tick					( );
	virtual	xray::input::handler& input_handler	( ) { return *this; }

	virtual	bool	on_keyboard_action		( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action		( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action		( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move			( input::world* input_world, int x, int y, int z );
	virtual int		input_priority			( ) { return 100; }
	
private:
	input::handler*	dialog_input_handler	();

private:
	stalker2::game&	m_game;
	main_menu_ui*	m_ui;
}; // class main_menu

} // namespace stalker2

#endif // #ifndef MAIN_MENU_H_INCLUDED