////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "console.h"
#include "game.h"
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/gamepad.h>
#include <xray/input/world.h>
#include <xray/ui/ui.h>

namespace stalker2 {

static pcstr get_next_tip(vector<pcstr>const& v, pcstr prev_tip, u16& tip_index, bool b_next)
{
	tip_index = 0;

	u32 prev_len = strings::length(prev_tip);

if(b_next)
{
	vector<pcstr>::const_iterator it	= v.begin();
	vector<pcstr>::const_iterator it_e	= v.end();
	
	for(; it!=it_e && prev_len>0; ++it,++tip_index)
	{
		pcstr s			= *it;
		if(0==strings::compare(s, prev_tip))
		{
			++it;
			++tip_index;
			if(it!=it_e)
				return *it;
			else
			{
				tip_index = 0;
				return v.front();
			}
		}
	}
}else
{
	tip_index = (u16)v.size()-1;
	vector<pcstr>::const_reverse_iterator rit	= v.rbegin();
	vector<pcstr>::const_reverse_iterator rit_e	= v.rend();
	
	for(; rit!=rit_e && prev_len>0; ++rit,--tip_index)
	{
		pcstr s			= *rit;
		if(0==strings::compare(s, prev_tip))
		{
			++rit;
			--tip_index;
			if(rit!=rit_e)
				return *rit;
			else
			{
				tip_index = (u16)v.size()-1;
				return v.back();
			}
		}
	}

}
	if(v.empty()) 
	{
		tip_index	= 0xff;
		return		"";
	}else
	{
		tip_index	= 0;
		return v.front();
	}
}

// input handler interface
//bool console::on_keyb_action(ui::window* , int p1, int p2)
bool console::on_keyboard_action	( input::world& input_world, input::enum_keyboard key, input::enum_keyboard_action action )
{
	if(action==xray::input::kb_key_down)
	{
		if(key==input::key_grave || key==input::key_escape)
		{
			m_game.toggle_console ( );
			return true;
		}

		if(key==input::key_up || key==input::key_down || key==input::key_tab) 
		{
			if(	m_tips_mode==tm_none							&& 
				!m_executed_history.empty()						&& 
				0==strings::length(m_text_edit->text()->get_text())
				)
				m_tips_mode			= tm_history;
			
			if(m_tips_mode == tm_history)
				m_tips.assign		(m_executed_history.begin(), m_executed_history.end());
			else
				m_tips_mode			= tm_list;

			u16 tip_index			= 0xff;

			bool b_next				= (key!=input::key_up);
			pcstr tip				= get_next_tip(m_tips, m_text_edit->text()->get_text(), tip_index, b_next);
			
			if(tip!=m_current_command_info) //tmp
			{
				m_text_edit->text()->set_text(tip);
				m_text_edit->set_caret_position((u16)strings::length(tip));
				fill_tips_view			();

				if(tip && tip_index!=0xff)
				{
					float2 line_size						(100.0f, 20.0f);
					m_ui_tips_view->w()->add_child			(m_ui_tips_view_hl->w(), false);
					m_ui_tips_view_hl->w()->set_size		(line_size);
					m_ui_tips_view_hl->w()->set_position	(float2(0.0f, tip_index*line_size.y));
				}
			}
			return					true;
		}else
				m_tips_mode			= tm_none;

		if(key==input::key_capslock)
		{
			if(m_ui_view->w()->get_focused())
			{
				m_ui_view->w()->set_focused		(false);
				m_text_edit->w()->set_focused	(true);
				return true;
			}
			if(m_text_edit->w()->get_focused())
			{
				m_text_edit->w()->set_focused	(false);
				m_ui_view->w()->set_focused		(true);
				return true;
			}
			return true;
		}
	} // key_down

	m_ui_dialog->input_handler()->on_keyboard_action(input_world, key, action );
	return true;
}

bool console::on_gamepad_action( input::world& input_world, input::gamepad_button button, input::enum_gamepad_action action )
{
	m_ui_dialog->input_handler()->on_gamepad_action( input_world, button, action );
	return true;
}

bool console::on_mouse_key_action( input::world& input_world, input::mouse_button button, input::enum_mouse_key_action action )
{
	m_ui_dialog->input_handler()->on_mouse_key_action( input_world, button, action );
	return true;
}

bool console::on_mouse_move( input::world& input_world, int x, int y, int z )
{
	m_ui_dialog->input_handler()->on_mouse_move( input_world, x, y, z );
	return true;
}

} // namespace stalker2

