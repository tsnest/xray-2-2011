////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_text_edit.h"
#include "ui_text_edit_action.h"
#include <xray/input/keyboard.h>

namespace xray {
namespace ui {

	static void clear_history_container(history& v, memory::base_allocator& a )
{
	history::iterator it	= v.begin();
	history::iterator it_e	= v.end();
	for(; it!=it_e; ++it)
	{
		pvoid p				= (pvoid)(*it).text;
		XRAY_FREE_IMPL		( a, p );
	}

	v.clear();
};

ui_text_edit::ui_text_edit(ui_world& w, enum_text_edit_mode mode, memory::base_allocator& a )
:ui_text			( w ),
m_caret_pos			( 0 ),
m_sel_start			( 0 ),
m_sel_end			( 0 ),
m_b_insert_mode		( false ),
m_last_action		( NULL ),
m_last_action_time	( 0.0f ),
m_b_undo			( false ),
m_b_redo			( false ),
m_edit_actions		( a ),
m_undo_history		( a ),
m_redo_history		( a )
{
	m_shift_state.set_none	();
	this->subscribe_event	(ev_focus, ui_event_handler(this, &ui_text_edit::on_focus));
	m_b_tab_stop			= true;

	init_internals			(mode);
}

ui_text_edit::~ui_text_edit()
{
	edit_actions_it it = m_edit_actions.begin();
	edit_actions_it it_e = m_edit_actions.end();
	for(; it!=it_e; ++it)
		XRAY_FREE_IMPL( allocator(), *it);

	clear_history_container( m_undo_history, allocator() );
	clear_history_container( m_redo_history, allocator() );
}

bool ui_text_edit::on_focus(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	if(p1)// just focused
		get_root()->subscribe_event(ev_keyboard, ui_event_handler(this, &ui_text_edit::on_keyb_action));
	else
		get_root()->unsubscribe_event(ev_keyboard, ui_event_handler(this, &ui_text_edit::on_keyb_action));

	return						true;
}

void ui_text_edit::set_text( pcstr text )
{
	if( 0!= strings::compare_insensitive(m_text.c_str(), text) )
	{
		m_text.clear	( );
		m_text.set		( text );
		process_event	(ev_text_changed, 0, 0);
		u16 str_len		= m_text.length();

		if(m_caret_pos>str_len )
			set_caret_position(str_len);

		if(!m_b_undo)
		{
			m_undo_history.resize		(m_undo_history.size()+1);
			m_undo_history.back().text	= strings::duplicate(allocator(), text);
			m_undo_history.back().caret = m_caret_pos;
			
			if(!m_b_redo)
				clear_history_container	( m_redo_history, allocator() );
		}
	}
}

bool ui_text_edit::is_shift_state(enum_shift_state const state) const
{
	switch(state)
	{
	case ks_Shift:
		return !!m_shift_state.m_data.d.shift;

	case ks_Ctrl:
		return !!m_shift_state.m_data.d.ctrl;

	case ks_Alt:
		return !!m_shift_state.m_data.d.alt;

	case ks_CtrlShift:
		return !!m_shift_state.m_data.d.ctrl && !!m_shift_state.m_data.d.shift;
	default:
		NODEFAULT(return false);
	}
}

void ui_text_edit::set_shift_state(enum_shift_state const state, bool b_set)
{
	switch(state)
	{
	case ks_Shift:
		m_shift_state.m_data.d.shift = (b_set?1:0);
		break;

	case ks_Ctrl:
		m_shift_state.m_data.d.ctrl	= (b_set?1:0);
		break;

	case ks_Alt:
		m_shift_state.m_data.d.alt	= (b_set?1:0);
		break;

	case ks_CtrlShift:
		m_shift_state.m_data.d.ctrl	= (b_set?1:0);
		m_shift_state.m_data.d.shift	= (b_set?1:0);
		break;
	default:
		NODEFAULT();
	}
}

void ui_text_edit::draw(xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view)
{
	super::draw_internal	(render, scene_view, m_sel_start, m_sel_end, 0xff010101); //argb
	draw_highlighted_rect	(render, scene_view);
	draw_cursor				(render, scene_view);
}

void ui_text_edit::draw_highlighted_rect(xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view)
{
	if(m_sel_start==m_sel_end)
		return;

	float2 pos1				= get_position();
	client_to_screen		(*this, pos1);		//need absolute position
	float2 pos2				= pos1;

	float h					= get_size().y;

	pos1.x					+= get_local_position(m_sel_start);
	pos2.x					+= get_local_position(m_sel_end);

	u32 clr					= 0x30ffffff;

	xray::render::ui::vertex const vertices[] = {
		xray::render::ui::vertex( pos1.x,	pos1.y+h,	0.0f, clr, 0,	1 ),
		xray::render::ui::vertex( pos1.x,	pos1.y,		0.0f, clr, 0,	0 ),
		xray::render::ui::vertex( pos2.x,	pos2.y+h,	0.0f, clr, 1,	1 ),
		xray::render::ui::vertex( pos2.x,	pos2.y,		0.0f, clr, 1,	0 )
	};

	u32 const primitives_type	= 0; // 0-tri-list, 1-line-list
	u32 const points_type		= 2; // rect
	render.draw_vertices	( scene_view, vertices, primitives_type, points_type );
}

void ui_text_edit::draw_cursor(xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view)
{
	float2 pos				= get_position();
	client_to_screen		(*this, pos);		//need absolute position
	float delta				= 2.0f;
	float h					= get_size().y - 2*delta; //tmp
	pos.x					+= get_local_position(m_caret_pos);
	pos.y					+= delta; //tmp

	u32 clr = 0xffff7f00;
	xray::render::ui::vertex const vertices[] = {
		xray::render::ui::vertex( pos.x,	pos.y,		0, clr, 0,0 ),
		xray::render::ui::vertex( pos.x,	pos.y+h,	0, clr, 0,0 ),
	};

	u32 const primitives_type	= 1; // 0-tri-list, 1-line-list
	u32 const points_type		= 1; // rect
	render.draw_vertices	( scene_view, vertices, primitives_type, points_type );
}

float ui_text_edit::get_local_position	(u16 const pos) const
{
	float result = (pos)? calc_string_length_n(*m_font, get_text(), pos) : 0.0f;

	return result;
}

void ui_text_edit::select_all()
{
	m_sel_start			= 0;
	m_sel_end			= m_text.length();
}

void ui_text_edit::reset_selection()
{
	m_sel_start			= m_caret_pos;
	m_sel_end			= m_caret_pos;
}

void ui_text_edit::switch_insert_mode()
{
	m_b_insert_mode = !m_b_insert_mode;
}

void ui_text_edit::insert_character(char const ch)
{
	text_editor_string		result;

	if(m_b_insert_mode)
	{
		result				= m_text.text();
		if(m_caret_pos<result.length())
			result.set			(m_caret_pos, ch);
		else
			result				+= ch;

	}else
	{
		m_text.text().substr	(0, m_caret_pos, & result);
		result					+= ch;
		result.append			(m_text.text().begin()+m_caret_pos, m_text.text().end());
	}
	set_text				( result.c_str() );
	set_caret_position		( m_caret_pos+1, false );
}

void ui_text_edit::copy_to_clipboard()
{
	LOG_WARNING("ui_text_edit::copy_to_clipboard not implemented");
}

void ui_text_edit::cut_to_clipboard()
{
	LOG_WARNING("ui_text_edit::cut_to_clipboard not implemented");
}

void ui_text_edit::paste_from_clipboard()
{
	LOG_WARNING("ui_text_edit::paste_from_clipboard not implemented");
}

void ui_text_edit::commit_text()
{
	if(m_text.length()>0)
		process_event		(ev_edit_text_commit, 0, 0);
}

void ui_text_edit::move_cursor_begin()
{
	move_cursor(cr_begin);
}

void ui_text_edit::move_cursor_end()
{
	move_cursor(cr_end);
}

void ui_text_edit::move_cursor_left()
{
	move_cursor(cr_left);
}

void ui_text_edit::move_cursor_right()
{
	move_cursor(cr_right);
}

void ui_text_edit::undo()
{
	if(m_undo_history.empty())
		return;

	m_b_undo	= true;
	
	m_redo_history.push_back(m_undo_history.back());

	m_undo_history.pop_back	();

	if(!m_undo_history.empty())
	{
		undo_ const& u		= m_undo_history.back();
		set_text			(u.text);
		set_caret_position	(u.caret, false);
	}

	m_b_undo	= false;
}

void ui_text_edit::redo()
{
	if(m_redo_history.empty())
		return;

	m_b_redo	= true;
	undo_ const& u		= m_redo_history.back();

	set_text			(u.text);
	set_caret_position	(u.caret, false);

	pvoid p				= (pvoid)m_redo_history.back().text;
	XRAY_FREE_IMPL		( allocator(), p );

	m_redo_history.pop_back();

	m_b_redo	= false;
}

void ui_text_edit::delete_left()
{
	if(get_selection())
	{
		delete_selection	();
		return;
	}

	if(m_caret_pos==0)
		return;

	text_editor_string		result;
	
	u16 pos					= m_caret_pos-1;
	m_text.text().substr	( 0, pos, & result );
	result.append			( m_text.text().begin()+m_caret_pos, m_text.text().end() );
	set_text				( result.c_str() );
	
	set_caret_position		( pos, true );
}

void ui_text_edit::delete_right()
{
	if(get_selection())
	{
		delete_selection	();
		return;
	}

	if(m_caret_pos==m_text.length())
		return;

	text_editor_string		result;
	
	u16 pos					= m_caret_pos;
	m_text.text().substr	( 0, pos, & result );
	result.append			( m_text.text().begin()+m_caret_pos+1, m_text.text().end() );
	set_text				( result.c_str() );
	
	set_caret_position		( pos, true );
}

void ui_text_edit::delete_selection()
{
	text_editor_string		result;

	u16 pos					= m_sel_start;
	m_text.text().substr	( 0, m_sel_start, & result );
	result.append			( m_text.text().begin()+m_sel_end, m_text.text().end() );
	set_text				( result.c_str() );
	
	set_caret_position		(pos, true);
	reset_selection			();
}


static bool is_delim(u8 ch)
{
	return( (ch) == ' ' || (ch) == '\t' || (ch) == '\r' || (ch) == '\n' || 
			(ch) == ',' || (ch) == '.' || (ch) == ':' || (ch) == '!' ||
			(ch) == '(' || (ch) == ')' || (ch) == '-' || (ch) == '+'|| (ch) == '*' );

}

static bool seq_delim(u8 ch)
{
	return( (ch) == ' ' || (ch) == '\t');
}

u16 ui_text_edit::calc_left_word_position	(u16 caret) const
{
	if(m_caret_pos==0)
		return 0;

	u16 tmp = 0;
	for(;;)
	{
		u16 res = calc_right_word_position(tmp);

		if(caret<=res)
			return tmp;
		else
			tmp = res;
	}
}

u16 ui_text_edit::calc_right_word_position(u16 caret) const
{
	pcstr str_begin = get_text();
	pcstr str		= get_text() + caret + 1;
	
	pcstr cur		= str;
	pcstr prev		= str-1;
	
	while( *cur )
	{
		bool b_cur		= is_delim(*cur);
		bool b_prev		= is_delim(*prev);

		if( (!b_cur && b_prev) || (b_cur && b_prev && !seq_delim(*cur)) )
			break;

		prev = cur;
		++cur;
	}

	return (u16)(cur-str_begin);
}

void ui_text_edit::set_caret_position(u16 const pos, bool b_move)
{
	bool b_move_right	= (pos>m_caret_pos);
	u16 prev_pos		= m_caret_pos;
	
	m_caret_pos			= pos;
	math::clamp<u16>	(m_caret_pos, 0, m_text.length());
	
	if(m_caret_pos==prev_pos)
		return;

	if(is_shift_state(ks_Shift) && b_move)
	{
		if(m_caret_pos>m_sel_end)
			m_sel_end = m_caret_pos;
		else
		if(m_caret_pos<m_sel_start)
			m_sel_start = m_caret_pos;
		else
		if(b_move_right)
			m_sel_start = m_caret_pos;
		else
			m_sel_end = m_caret_pos;
	}else
		reset_selection	();
}

void ui_text_edit::move_cursor(enum_cursor_moving action)
{
	switch(action)
	{
	case cr_left:
		if(m_caret_pos!=0)
			set_caret_position((is_shift_state(ks_Ctrl))?calc_left_word_position(m_caret_pos) :m_caret_pos-1, true);
		break;

	case cr_right:
		set_caret_position((is_shift_state(ks_Ctrl))?calc_right_word_position(m_caret_pos) :m_caret_pos+1, true);
		break;

	case cr_begin:
		set_caret_position(0, true);
		break;

	case cr_end:
		set_caret_position(m_text.length(), true);
		break;
	};
}

bool ui_text_edit::on_keyb_action(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);
	input::enum_keyboard_action action = (input::enum_keyboard_action)p2;
	
	if(action==input::kb_key_hold)
		return true;

	m_last_action			= NULL;

	edit_actions_it it		= m_edit_actions.begin();
	edit_actions_it it_e	= m_edit_actions.end();
	
	bool result = false;
	
	for(;it!=it_e;++it)
	{
		base_edit_action* a = *it;
		if(a->similar((input::enum_keyboard)p1, action, m_shift_state))
		{
			result				= a->execute(action);
			if(result)
			{
				m_last_action			= a;
				m_last_action_time		= m_ui_world.timer().get_elapsed_sec();
			}

		}
	}

	return result;
}

void ui_text_edit::tick()
{
	super::tick();
	
	if(m_last_action && (m_last_action_time+0.3f < m_ui_world.timer().get_elapsed_sec()) )
	{
		m_last_action_time = m_ui_world.timer().get_elapsed_sec()-0.2f;
		m_last_action->execute(input::kb_key_hold);
	}
}

} // namespace xray
} // namespace ui
