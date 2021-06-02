////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_text_edit_action.h"
#include "ui_text_edit.h"
#include <xray/input/keyboard.h>

namespace xray {
namespace ui {

bool base_edit_action::similar(	input::enum_keyboard key, 
								input::enum_keyboard_action action, 
								shift_state const& state)
{
	if (m_key!=key)
		return false;

	if (m_key_action!=input::kb_key_unknown && m_key_action!=action)
		return false;


	if (state.m_data.d.alt==m_shift_state.m_data.d.alt || m_shift_state.m_data.d.alt==2)//any
		if (state.m_data.d.shift==m_shift_state.m_data.d.shift || m_shift_state.m_data.d.shift==2)//any
			if (state.m_data.d.ctrl==m_shift_state.m_data.d.ctrl || m_shift_state.m_data.d.ctrl==2)//any
				return true;

	return false;
}


base_edit_action::base_edit_action(	ui_text_edit* parent, 
									input::enum_keyboard key, 
									input::enum_keyboard_action action, 
									shift_state const& state)
:m_parent(parent),
m_key(key),
m_key_action(action),
m_shift_state(state)
{}

base_edit_action::~base_edit_action()
{}

functor_edit_action::functor_edit_action(	action_functor const& functor,
											ui_text_edit* parent, 
											input::enum_keyboard key, 
											input::enum_keyboard_action action, 
											shift_state const& state)
:super(parent, key, action, state),
m_functor(functor)
{}

functor_edit_action::~functor_edit_action()
{}

bool functor_edit_action::execute(input::enum_keyboard_action action)
{
	XRAY_UNREFERENCED_PARAMETER	(action);
	m_functor		();
	return			true;
}

insert_char_action::insert_char_action( ui_text_edit* parent, 
										input::enum_keyboard key, 
										input::enum_keyboard_action action, 
										shift_state const& state,
										char c, char c_shift, bool b_translate)
:super(parent, key, action, state),
m_char(c),
m_char_shift(c_shift),
m_b_translate(b_translate)
{}

insert_char_action::~insert_char_action()
{}

bool insert_char_action::execute(input::enum_keyboard_action action)
{
	XRAY_UNREFERENCED_PARAMETER	(action);
	char c = (m_parent->is_shift_state(ks_Shift))? m_char_shift : m_char;

	//if( m_b_translate )
	//{
	//	string128		buff;
	//	buff[0]			= 0;
	//	
	//	static _locale_t current_locale = _create_locale(LC_ALL, "");
	//	
	//	if ( pInput->get_dik_name( m_dik, buff, sizeof(buff) ) )
	//	{
	//		if ( _isalpha_l(buff[0], current_locale) || buff[0] == char(-1) ) // "ÿ" = -1
	//		{
	//			_strlwr_l	(buff, current_locale);
	//			c			= buff[0];
	//		}
	//	}
	//}
	m_parent->delete_selection();
	m_parent->insert_character( c );
	return			true;
}

shift_state_action::shift_state_action(	ui_text_edit* parent,
										input::enum_keyboard key, 
										enum_shift_state state)
:super(parent, key, input::kb_key_unknown, shift_state().set_any()),
m_shift_switch_state(state)
{}

shift_state_action::~shift_state_action()
{}

bool shift_state_action::execute(input::enum_keyboard_action action)
{
	bool b_what_to_do			= (action==input::kb_key_up) ? false : true;
	m_parent->set_shift_state	(m_shift_switch_state, b_what_to_do);
	return						false;
}

} // namespace xray
} // namespace ui
