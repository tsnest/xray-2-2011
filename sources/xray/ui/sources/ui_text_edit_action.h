////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef UI_TEXT_EDIT_ACTION_H_INCLUDED
#define UI_TEXT_EDIT_ACTION_H_INCLUDED

#include "ui_text_edit_defs.h"
#include <xray/input/keyboard.h>

namespace xray {

namespace input{
	enum enum_keyboard;
	enum enum_keyboard_action;
};

namespace ui {

class ui_text_edit;
enum enum_shift_state;


class base_edit_action : private boost::noncopyable
{
public:
							base_edit_action		(	ui_text_edit* parent,
														input::enum_keyboard key, 
														input::enum_keyboard_action action,
														shift_state const& state);
	virtual					~base_edit_action		();
	virtual	bool			execute					(	input::enum_keyboard_action action)			= 0;
			bool			similar					(	input::enum_keyboard key, 
														input::enum_keyboard_action action, 
														shift_state const& state);
protected:
	ui_text_edit*				m_parent;
	input::enum_keyboard		m_key;
	input::enum_keyboard_action	m_key_action;
	shift_state					m_shift_state;
};

class functor_edit_action : public base_edit_action
{
	typedef base_edit_action						super;
public:
	typedef	fastdelegate::FastDelegate0<void>		action_functor;

public:
							functor_edit_action		(	action_functor const& functor,
														ui_text_edit* parent, 
														input::enum_keyboard key, 
														input::enum_keyboard_action action, 
														shift_state const& state);
	virtual					~functor_edit_action	();
	virtual	bool			execute					(input::enum_keyboard_action action);

protected:
	action_functor			m_functor;

};

class insert_char_action : public base_edit_action
{
	typedef base_edit_action						super;
public:
							insert_char_action		(	ui_text_edit* parent, 
														input::enum_keyboard key, 
														input::enum_keyboard_action action, 
														shift_state const& state,
														char c, char c_shift, bool b_translate);

	virtual					~insert_char_action		();
	virtual	bool			execute					(input::enum_keyboard_action action);

private:
	bool					m_b_translate;
	char					m_char;
	char					m_char_shift;
};

class shift_state_action : public base_edit_action
{
	typedef base_edit_action						super;
public:
							shift_state_action		(	ui_text_edit* parent,
														input::enum_keyboard key, 
														enum_shift_state state);
	virtual					~shift_state_action	();
	virtual	bool			execute				(input::enum_keyboard_action action);
private:
	enum_shift_state		m_shift_switch_state;
};

} // namespace xray
} // namespace ui

#endif //UI_TEXT_EDIT_ACTION_H_INCLUDED