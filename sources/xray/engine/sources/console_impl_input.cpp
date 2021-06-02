////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "console_impl.h"
#include <xray/console_command_processor.h>
#include <xray/console_command.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include <xray/input/gamepad.h>
#include <xray/input/world.h>
#include <xray/ui/ui.h>

namespace xray {

static pcstr extract_tip_name( pcstr prev_text, enum_tips_mode mode )
{
	if(mode == tm_arg_list)
	{
		pcstr tip = strchr(prev_text, ' ');
		R_ASSERT(tip);
		return tip+1;
	}else
	{
		return prev_text;
	}
}

static pcstr make_next_tip(vectora<pcstr>const& v, pcstr text, u16& tip_index, bool b_next, enum_tips_mode mode)
{
	tip_index = 0;

	if( strings::length(text) )
	{
	
		pcstr tip = extract_tip_name(text, mode);

		if(b_next)	
		{
			vectora<pcstr>::const_iterator it	= v.begin();
			vectora<pcstr>::const_iterator it_e	= v.end();
			
			for(; it!=it_e; ++it,++tip_index)
			{
				pcstr s			= *it;
				if(0==strings::compare(s, tip))
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
			vectora<pcstr>::const_reverse_iterator rit	= v.rbegin();
			vectora<pcstr>::const_reverse_iterator rit_e	= v.rend();
			
			for(; rit!=rit_e; ++rit,--tip_index)
			{
				pcstr s			= *rit;
				if(0==strings::compare(s, tip))
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

void apply_new_tip(ui::text_edit* text_edit, enum_tips_mode mode, pcstr new_tip)
{
	if(mode==tm_arg_list)
	{
		fixed_string<512> str			= text_edit->text()->get_text();
		u32 tip_idx						= str.find(' ');
		str.set_length					( tip_idx+1 );
		str.append						( new_tip );
		text_edit->text()->set_text		( str.c_str() );
	}else
	{
		text_edit->text()->set_text		( new_tip );
	}
}

// input handler interface
bool console_impl::on_keyboard_action( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action )
{
	if(action==xray::input::kb_key_down)
	{
		if(m_self_deactivate && (key==input::key_grave || key==input::key_escape) )
		{
			R_ASSERT		( get_active() );
			on_deactivate	( );
			return true;
		}

		pcstr current_text = m_text_edit->text()->get_text();
		
		if(key==input::key_up || key==input::key_down || key==input::key_tab) 
		{
			if(	m_tips_mode==tm_none							&& 
				!m_executed_history.empty()						&& 
				0==strings::length(current_text)
				)
				m_tips_mode			= tm_history;
			
			if(m_tips_mode == tm_history)
				m_tips.assign		(m_executed_history.begin(), m_executed_history.end());
			else
			if(m_tips_mode == tm_none )
			{
				u32 len = strings::length(current_text);
				if(current_text[len-1]==' ')
				{
					pstr str = strings::duplicate( m_allocator, current_text);
					while(len && str[--len]==' ')
						str[len] = 0;

					if(len)
					{
						console_commands::console_command* c = console_commands::find(str);
						if(c)
						{
							c->fill_command_args_list( m_tips );
							if(!m_tips.empty())
								m_tips_mode				= tm_arg_list;
						}
					}
					XRAY_FREE_IMPL	( m_allocator,  str );
				}
			}

			if(m_tips_mode == tm_none )
				m_tips_mode			= tm_commands_list;

			u16 tip_index			= 0xff;

			bool b_next				= (key!=input::key_up);
			pcstr tip				= make_next_tip(m_tips, current_text, tip_index, b_next, m_tips_mode);
			
			if( tip!=m_current_command_info ) //tmp
			{
				apply_new_tip		(m_text_edit, m_tips_mode, tip);

				m_text_edit->set_caret_position((u16)strings::length(m_text_edit->text()->get_text()));
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

bool console_impl::on_gamepad_action( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action )
{
	m_ui_dialog->input_handler()->on_gamepad_action( input_world, button, action );
	return true;
}

bool console_impl::on_mouse_key_action( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action )
{
	m_ui_dialog->input_handler()->on_mouse_key_action( input_world, button, action );
	return true;
}

bool console_impl::on_mouse_move( input::world* input_world, int x, int y, int z )
{
	m_ui_dialog->input_handler()->on_mouse_move( input_world, x, y, z );
	return true;
}

} //namespace xray

