////////////////////////////////////////////////////////////////////////////
//	Created		: 05.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_CONSOLE_H_INCLUDED
#define GAME_CONSOLE_H_INCLUDED

#include <xray/engine/console.h>
#include "console_impl.h"

namespace xray{
namespace engine{

class game_console :public console_impl,
					public engine::console
{
	typedef	console_impl		super;
public:
				game_console				( ui::world& uw, input::world& iw, xray::memory::base_allocator& a );

// engine::console interface
	virtual bool			get_active			( ) const;
	virtual void			on_activate			( );
	virtual void			on_deactivate		( );
	virtual void			tick				( );

	virtual	bool			on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool			on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool			on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool			on_mouse_move		( input::world* input_world, int x, int y, int z );
	virtual int				input_priority		( ) { return super::input_priority(); }
private:
	input::world&			m_input_world;
}; // class game_console

} // namespace engine
} // namespace xray

#endif // #ifndef GAME_CONSOLE_H_INCLUDED