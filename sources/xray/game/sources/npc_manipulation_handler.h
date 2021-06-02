////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_MANIPULATION_HANDLER_H_INCLUDED
#define NPC_MANIPULATION_HANDLER_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2 {

class game;

class npc_manipulation_handler :
	public xray::input::handler,
	private boost::noncopyable
{
public:
					npc_manipulation_handler	( game& environment );
	virtual			~npc_manipulation_handler	( );

	virtual	bool	on_keyboard_action			( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action			( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action			( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move				( input::world* input_world, int x, int y, int z );
	virtual int		input_priority				( ) { return 9999; }

private:
	game&			m_game;
}; // class npc_manipulation_handler

} // namespace stalker2

#endif // #ifndef NPC_MANIPULATION_HANDLER_H_INCLUDED