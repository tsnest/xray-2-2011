////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_text_edit.h"
#include "ui_text_edit_action.h"
#include <xray/input/keyboard.h>

namespace xray {
namespace ui {

shift_state& shift_state::set_any()
{
	m_data.d.alt		= 2; //0-off, 1-on, 2-on or off
	m_data.d.ctrl		= 2;
	m_data.d.shift		= 2;
	m_data.d.force		= 2;
	return				*this;
}

typedef functor_edit_action::action_functor	action_functor;

bool action_less(base_edit_action const* a1, base_edit_action const* a2)
{
	XRAY_UNREFERENCED_PARAMETERS	(a1, a2);
	UNREACHABLE_CODE(return false);
}

static base_edit_action* create_functor(action_functor const& functor,
										ui_text_edit* parent,
										input::enum_keyboard key, 
										input::enum_keyboard_action action, 
										shift_state const& state)
{
	return XRAY_NEW_IMPL(parent->allocator(), functor_edit_action)(functor, parent, key, action, state);
}

static base_edit_action* create_char_action(ui_text_edit* parent,
										input::enum_keyboard key, 
										input::enum_keyboard_action action, 
										char c, char c_shift, bool b_translate=false)
{
	static shift_state	st;
	st.set_none();
	st.m_data.d.shift	= 2;

	return XRAY_NEW_IMPL(parent->allocator(), insert_char_action)(parent, key, action, st, c, c_shift, b_translate);
}

static base_edit_action* create_shift_action(ui_text_edit* parent,
										input::enum_keyboard key, 
										enum_shift_state state)
{
	return XRAY_NEW_IMPL(parent->allocator(), shift_state_action)(parent, key, state);
}

void  ui_text_edit::add_action(base_edit_action* action)
{
	m_edit_actions.push_back(action);
}

void ui_text_edit::init_internals(enum_text_edit_mode mode)
{
	shift_state							any_sh_state;
	any_sh_state.set_any				();

	shift_state							none_sh_state;
	none_sh_state.set_none				();

	shift_state							ctrl_sh_state;
	ctrl_sh_state.set_none				();
	ctrl_sh_state.m_data.d.ctrl			= 1;

	shift_state							shift_sh_state;
	shift_sh_state.set_none				();
	shift_sh_state.m_data.d.shift		= 1;

	shift_state							ctrl_shift_sh_state;
	ctrl_shift_sh_state.set_none		();
	ctrl_shift_sh_state.m_data.d.ctrl	= 1;
	ctrl_shift_sh_state.m_data.d.shift	= 1;

	add_action(create_shift_action(this, input::key_lshift, ks_Shift));
	add_action(create_shift_action(this, input::key_rshift, ks_Shift));

	add_action(create_shift_action(this, input::key_lcontrol, ks_Ctrl));
	add_action(create_shift_action(this, input::key_rcontrol, ks_Ctrl));

	add_action(create_shift_action(this, input::key_lalt, ks_Alt));
	add_action(create_shift_action(this, input::key_ralt, ks_Alt));


	add_action(create_functor(action_functor(this, &ui_text_edit::move_cursor_left),this, input::key_left, input::kb_key_down, any_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::move_cursor_right),this, input::key_right, input::kb_key_down, any_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::move_cursor_begin), this, input::key_home, input::kb_key_down, any_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::move_cursor_end), this, input::key_end, input::kb_key_down, any_sh_state));

	add_action(create_functor(action_functor(this, &ui_text_edit::select_all),		this, input::key_a, input::kb_key_down, ctrl_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::copy_to_clipboard), this, input::key_c, input::kb_key_down, ctrl_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::copy_to_clipboard), this, input::key_insert, input::kb_key_down, ctrl_sh_state));


	if(mode==te_read_only)
		return;

	add_action(create_functor(action_functor(this, &ui_text_edit::switch_insert_mode),this, input::key_insert, input::kb_key_down, none_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::undo),this, input::key_z, input::kb_key_down, ctrl_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::redo),this, input::key_z, input::kb_key_down, ctrl_shift_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::cut_to_clipboard),this, input::key_x, input::kb_key_down, ctrl_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::cut_to_clipboard),this, input::key_delete, input::kb_key_down, shift_sh_state));

	add_action(create_functor(action_functor(this, &ui_text_edit::paste_from_clipboard),this, input::key_v, input::kb_key_down, ctrl_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::paste_from_clipboard),this, input::key_insert, input::kb_key_down, shift_sh_state));

	add_action(create_functor(action_functor(this, &ui_text_edit::delete_left),this, input::key_back, input::kb_key_down, none_sh_state));
	add_action(create_functor(action_functor(this, &ui_text_edit::delete_right),this, input::key_delete, input::kb_key_down, none_sh_state));

	add_action(create_functor(action_functor(this, &ui_text_edit::commit_text),this, input::key_return, input::kb_key_down, any_sh_state));


// fill alphabet
//	if(mode==im_number_only)----implement this !!!
//	if(mode==im_file_name_mode)
	{
	add_action(create_char_action(this,input::key_0,		input::kb_key_down, '0',')'));
	add_action(create_char_action(this,input::key_numpad0,	input::kb_key_down, '0','0'));
	
	add_action(create_char_action(this,input::key_1,		input::kb_key_down,	'1','!'));
	add_action(create_char_action(this,input::key_numpad1,	input::kb_key_down,	'1','1'));
	
	add_action(create_char_action(this,input::key_2,		input::kb_key_down,	'2','@'));
	add_action(create_char_action(this,input::key_numpad2,	input::kb_key_down,	'2','2'));
	
	add_action(create_char_action(this,input::key_3,		input::kb_key_down,	'3','#'));
	add_action(create_char_action(this,input::key_numpad3,	input::kb_key_down,	'3','3'));
	
	add_action(create_char_action(this,input::key_4,		input::kb_key_down,	'4','$'));
	add_action(create_char_action(this,input::key_numpad4,	input::kb_key_down,	'4','4'));
	
	add_action(create_char_action(this,input::key_5,		input::kb_key_down,	'5','%'));
	add_action(create_char_action(this,input::key_numpad5,	input::kb_key_down,	'5','5'));
	
	add_action(create_char_action(this,input::key_6,		input::kb_key_down,	'6','^'));
	add_action(create_char_action(this,input::key_numpad6,	input::kb_key_down,	'6','6'));
	
	add_action(create_char_action(this,input::key_7,		input::kb_key_down,	'7','&'));
	add_action(create_char_action(this,input::key_numpad7,	input::kb_key_down,	'7','7'));
	
	add_action(create_char_action(this,input::key_8,		input::kb_key_down,	'8','*'));
	add_action(create_char_action(this,input::key_numpad8,	input::kb_key_down,	'8','8'));
	
	add_action(create_char_action(this,input::key_9,		input::kb_key_down,	'9','('));
	add_action(create_char_action(this,input::key_numpad9,	input::kb_key_down,	'9','9'));
	}
	{
	add_action(create_char_action(this,input::key_a,		input::kb_key_down,	'a','A',true));
	add_action(create_char_action(this,input::key_b,		input::kb_key_down,	'b','B',true));
	add_action(create_char_action(this,input::key_c,		input::kb_key_down,	'c','C',true));
	add_action(create_char_action(this,input::key_d,		input::kb_key_down,	'd','D',true));
	add_action(create_char_action(this,input::key_e,		input::kb_key_down,	'e','E',true));
	add_action(create_char_action(this,input::key_f,		input::kb_key_down,	'f','F',true));
	add_action(create_char_action(this,input::key_g,		input::kb_key_down,	'g','G',true));
	add_action(create_char_action(this,input::key_h,		input::kb_key_down,	'h','H',true));
	add_action(create_char_action(this,input::key_i,		input::kb_key_down,	'i','I',true));
	add_action(create_char_action(this,input::key_j,		input::kb_key_down,	'j','J',true));
	add_action(create_char_action(this,input::key_k,		input::kb_key_down,	'k','K',true));
	add_action(create_char_action(this,input::key_l,		input::kb_key_down,	'l','L',true));
	add_action(create_char_action(this,input::key_m,		input::kb_key_down,	'm','M',true));
	add_action(create_char_action(this,input::key_n,		input::kb_key_down,	'n','N',true));
	add_action(create_char_action(this,input::key_o,		input::kb_key_down,	'o','O',true));
	add_action(create_char_action(this,input::key_p,		input::kb_key_down,	'p','P',true));
	add_action(create_char_action(this,input::key_q,		input::kb_key_down,	'q','Q',true));
	add_action(create_char_action(this,input::key_r,		input::kb_key_down,	'r','R',true));
	add_action(create_char_action(this,input::key_s,		input::kb_key_down,	's','S',true));
	add_action(create_char_action(this,input::key_t,		input::kb_key_down,	't','T',true));
	add_action(create_char_action(this,input::key_u,		input::kb_key_down,	'u','U',true));
	add_action(create_char_action(this,input::key_v,		input::kb_key_down,	'v','V',true));
	add_action(create_char_action(this,input::key_w,		input::kb_key_down,	'w','W',true));
	add_action(create_char_action(this,input::key_x,		input::kb_key_down,	'x','X',true));
	add_action(create_char_action(this,input::key_y,		input::kb_key_down,	'y','Y',true));
	add_action(create_char_action(this,input::key_z,		input::kb_key_down,	'z','Z',true));

	add_action(create_char_action(this,input::key_numpadminus,		input::kb_key_down,'-','-'));
	add_action(create_char_action(this,input::key_numpadplus,		input::kb_key_down,'+','+'));
	add_action(create_char_action(this,input::key_numpadperiod,		input::kb_key_down,'.','.'));

	add_action(create_char_action(this,input::key_minus,		input::kb_key_down,'-','_'));
	add_action(create_char_action(this,input::key_space,		input::kb_key_down,' ',' '));
	add_action(create_char_action(this,input::key_grave,		input::kb_key_down,'`','~'));

	add_action(create_char_action(this,input::key_backslash,	input::kb_key_down,'\\'	,'|',true));
	add_action(create_char_action(this,input::key_lbracket,		input::kb_key_down,'['	,'{',true));
	add_action(create_char_action(this,input::key_rbracket,		input::kb_key_down,']'	,'}',true));
	add_action(create_char_action(this,input::key_apostrophe,	input::kb_key_down,'\''	,'\"',true));
	add_action(create_char_action(this,input::key_comma,		input::kb_key_down,','	,'<',true));
	add_action(create_char_action(this,input::key_period,		input::kb_key_down,'.'	,'>',true));
	add_action(create_char_action(this,input::key_equals,		input::kb_key_down,'='	,'+',true));
	add_action(create_char_action(this,input::key_semicolon,	input::kb_key_down,';'	,':',true));
	add_action(create_char_action(this,input::key_slash,		input::kb_key_down,'/'	,'?',true));
	add_action(create_char_action(this,input::key_numpadstar,	input::kb_key_down,'*'	,'*',true));
	add_action(create_char_action(this,input::key_numpadslash,	input::kb_key_down,'/'	,'/',true));
	}

//	std::sort(m_edit_actions.begin(), m_edit_actions.end(), action_less);
}

} // namespace xray
} // namespace ui
