////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_scroll_view.h"
#include <xray/input/keyboard.h>

namespace xray {
namespace ui {

ui_scroll_view::ui_scroll_view( memory::base_allocator& a )
:super			( a ),
m_pad			( a ),
m_scroll_bar_v	( a ),
m_flags			( fl_follow_last )
{
	add_child				( &m_pad, false );
	m_pad.set_visible		( true );
	m_pad.set_size			( float2(20.0f, 20.0f) );
	add_child				( &m_scroll_bar_v, false );
	m_scroll_source_v.set_pad( &m_pad );
	m_scroll_bar_v.init		( &m_scroll_source_v, true );
	m_scroll_bar_v.set_size	( float2(20.0f, 0.0f) );
	m_scroll_bar_v.set_visible( true );

	this->subscribe_event	(ev_size_changed, ui_event_handler(this, &ui_scroll_view::on_self_size_changed));
	this->subscribe_event	(ev_focus, ui_event_handler(this, &ui_scroll_view::on_focus));
	m_pad.subscribe_event	(ev_size_changed, ui_event_handler(this, &ui_scroll_view::on_pad_size_changed));
	m_pad.subscribe_event	(ev_position_changed, ui_event_handler(this, &ui_scroll_view::on_pad_pos_changed));

	m_b_tab_stop			= true;
}

ui_scroll_view::~ui_scroll_view()
{}

void ui_scroll_view::add_item(window* w, bool adopt)
{
	m_pad.add_child			(w, adopt);
	m_flags					|= fl_need_recalc;
}

void ui_scroll_view::remove_item(window* w)
{
	m_pad.remove_child		(w);
	m_flags					|= fl_need_recalc;
}

void ui_scroll_view::clear()
{
	m_pad.remove_all_childs	();
	m_flags					|= fl_need_recalc;
}

u32 ui_scroll_view::get_items_count() const
{
	return m_pad.get_child_count();
}

window* ui_scroll_view::get_item(u32 idx) const
{
	return m_pad.get_child	(idx);
}

void ui_scroll_view::recalc()
{
	u32 count				= get_items_count();
	float2 pos				(0.0f, 0.0f);
	float2 pad_size			(0.0f, 0.0f);
	for(u32 idx=0; idx<count; ++idx)
	{
		window* w			= get_item	(idx);
		w->set_position		(pos);
		pos.y				+= w->get_size().y;
		pad_size.y			= math::max(pos.y, pad_size.y);
		pad_size.x			= math::max(w->get_position().x+w->get_size().x, pad_size.x);
	}
	m_pad.set_size			(pad_size);
	m_flags	&= ~fl_need_recalc;

	if(get_follow_last_line())
		m_scroll_bar_v.move_end		();
}

bool ui_scroll_view::on_focus(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	if(p1)// just focused
		get_root()->subscribe_event(ev_keyboard, ui_event_handler(this, &ui_scroll_view::on_keyb_action));
	else
		get_root()->unsubscribe_event(ev_keyboard, ui_event_handler(this, &ui_scroll_view::on_keyb_action));

	return						true;
}

bool ui_scroll_view::on_pad_size_changed(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	m_scroll_bar_v.update_self	();
	return						false;
}

bool ui_scroll_view::on_pad_pos_changed(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	m_scroll_bar_v.update_self	();
	return						false;
}

bool ui_scroll_view::on_self_size_changed(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	//float2 pad_size 			= m_pad.get_size();
	//pad_size.x					= math::max(pad_size.x, get_size().x);
	//pad_size.y					= math::max(pad_size.y, get_size().y);
	//m_pad.set_size				(pad_size);

	float2 scb_pos				(get_size().x-m_scroll_bar_v.get_size().x, 0);
	m_scroll_bar_v.set_position	(scb_pos);

	float2 scb_size				(m_scroll_bar_v.get_size().x, get_size().y);
	m_scroll_bar_v.set_size		(scb_size);
	return						false;
}


bool ui_scroll_view::on_keyb_action(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETER	(w);
	
	if(p2==input::kb_key_down || p2==input::kb_key_hold) //key down or hold action
	{
		if(p1==input::key_down)
		{
			float step	= m_scroll_source_v.get_step_size();
			if(p2==input::kb_key_hold)
				step	*=	0.1f;

			m_scroll_bar_v.move			(-step);
			set_follow_last_line		(false);
			return						true;
		}

		if(p1==input::key_up)
		{
			float step	= m_scroll_source_v.get_step_size();
			if(p2==input::kb_key_hold)
				step	*=	0.1f;

			m_scroll_bar_v.move			(step);
			set_follow_last_line		(false);
			return						true;
		}

		if(p1==input::key_end)
		{
			m_scroll_bar_v.move_end		();
			set_follow_last_line		(true);
			return						true;
		}

		if(p1==input::key_home)
		{
			m_scroll_bar_v.move_begin	();
			set_follow_last_line		(false);
			return						true;
		}
	}
	return								false;
}

void ui_scroll_view::tick()
{
	if(m_flags&fl_need_recalc)
		recalc				();

	ui_window::tick		();
}

void ui_scroll_view::draw(xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view)
{
	ui_window::draw				(renderer, scene_view);
}

void ui_scroll_view::set_follow_last_line(bool val)
{
	if(val)
		m_flags					|= fl_follow_last;
	else
		m_flags					&=~fl_follow_last;
}


ui_scroll_v_source::ui_scroll_v_source()
:m_pad(NULL),m_step(0.0f)
{}

void ui_scroll_v_source::set_pad(ui_scroll_pad* p)
{
	m_pad = p;
}

float ui_scroll_v_source::get_length()	const
{
	return m_pad->get_size().y;
}

float ui_scroll_v_source::get_position()	const
{
	return m_pad->get_position().y;
}

void ui_scroll_v_source::set_position(float const value)
{
	float2 pos			= m_pad->get_position();
	pos.y				= value;
	m_pad->set_position	(pos);
}

} // namespace xray
} // namespace ui
