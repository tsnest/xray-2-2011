////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef GAME_EXIT_HANDLER_H_INCLUDED
#define GAME_EXIT_HANDLER_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2{

class game;

class exit_handler:	public xray::input::handler,
					private boost::noncopyable
{
public:
					exit_handler		( game& game );
	virtual			~exit_handler		( );

	virtual	bool	on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move		( input::world* input_world, int x, int y, int z );
	virtual int		input_priority		( ) {return 100000;}

protected:
	game&			m_game;
};//class exit_handler

}//namespace stalker2

#endif //GAME_EXIT_HANDLER_H_INCLUDED