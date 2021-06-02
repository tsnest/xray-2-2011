////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef UI_DIALOG_H_INCLUDED
#define UI_DIALOG_H_INCLUDED

#include "ui_window.h"
#include <xray/input/handler.h>

namespace xray {
namespace ui {


class ui_world;
class ui_dialog_holder;

class ui_dialog :	public dialog,
					public ui_window,
					public input::handler  
{
	typedef ui_window					super;
public:
					ui_dialog			( memory::base_allocator& a );
	virtual			~ui_dialog			( );

	virtual	bool	on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move		( input::world* input_world, int x, int y, int z );

	virtual int		input_priority		( ) {return 10;}

	virtual window*			w			()								{return this;}
	virtual input::handler*	input_handler()								{return this;}

protected:
			void	focus_next			();
			bool	calc_focus_next		(window_list& childs, window*& prev_focused, window*& new_focused);

};

} // namespace xray
} // namespace ui

#endif //UI_DIALOG_H_INCLUDED