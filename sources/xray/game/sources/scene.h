////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2 {
class game;

class scene :	public xray::input::handler,
				private boost::noncopyable
{
public:
						scene				( game& w );
	virtual void		on_activate			( );
	virtual void		on_deactivate		( );
	virtual void		tick				( )									= 0;
// input handler interface
	virtual	bool	on_keyboard_action	( input::world& input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action	( input::world& input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action	( input::world& input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move		( input::world& input_world, int x, int y, int z );
	virtual int		input_priority		( ) {return 100;}

protected:
	game&				m_game;
}; // class scene

} // namespace stalker2

#endif // #ifndef SCENE_H_INCLUDED