////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_dialog.h"
#include <xray/input/gamepad.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>

namespace xray {
namespace ui {

bool ui_dialog::on_keyboard_action(input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action)
{
	XRAY_UNREFERENCED_PARAMETERS	( input_world );

	//if(key==input::key_tab && action==input::kb_key_down)
	//{
	//	focus_next	();
	//	return		true;
	//}else
	//if(key==input::key_escape && action==input::kb_key_down)
	//{
	//	get_holder()->hide_dialog(this);
	//	return	true;
	//}else
	if(key==input::key_scroll && action==input::kb_key_down)
	{
		set_visible(!get_visible());
		return	true;
	}else
	{
		if(process_event(ev_keyboard, key, action))
			return true;
	}
	return false;
}

bool ui_dialog::on_gamepad_action(input::world* , input::gamepad_button , input::enum_gamepad_action )
{
	return	false;
}

bool ui_dialog::on_mouse_key_action(input::world* , input::mouse_button , input::enum_mouse_key_action )
{
	return	false;
}

bool ui_dialog::on_mouse_move(input::world* , int x, int y, int z)
{
	XRAY_UNREFERENCED_PARAMETERS	(x, y, z);
	return	false;
}

} // namespace xray
} // namespace ui
