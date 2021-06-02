////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_scroll_bar.h"

namespace xray {
namespace ui {

ui_scroll_bar::ui_scroll_bar( memory::base_allocator& a )
:super			( a ),
m_track_button	( a ),
m_btn_lt		( a ),
m_btn_rb		( a ),
m_source		( NULL ),
m_vertical		( true )
{
	m_btn_lt.set_visible		(true);
	m_btn_rb.set_visible		(true);
	m_track_button.set_visible	(true);
	m_btn_lt.set_size			(float2(20.0f, 20.0f));
	m_btn_rb.set_size			(float2(20.0f, 20.0f));
	m_track_button.set_size		(float2(20.0f, 20.0f));
	m_track_button.init_texture	("ui_rect");
	m_track_button.set_color	(0xff393939);
	add_child					(&m_btn_lt, false);
	add_child					(&m_btn_rb, false);
	add_child					(&m_track_button, false);
	this->subscribe_event		(ev_size_changed, ui_event_handler(this, &ui_scroll_bar::on_self_size_changed));
	init_texture				("ui_rect");
	set_color					(0xff303030);
}

void ui_scroll_bar::init(scroll_source* src, bool b_vertical)
{
	m_source					= src;
	m_vertical					= b_vertical;
	
	if(m_vertical)
		m_track_button.set_position	(float2(0.0f, m_btn_lt.get_size().y));
	else
		m_track_button.set_position	(float2(0.0f, m_btn_lt.get_size().x));
}
static void clamp_min(float& val, float const min)
{
	if(val<min) val=min;
}
void ui_scroll_bar::update_self()
{
	// set track-button position & size
	float wrk_area_len			= 0.0f;
	float btn_h					= m_btn_lt.get_size().y;
	float2 track_size			= m_track_button.get_size();
	float2 track_pos			= m_track_button.get_position();

	//size
	if(m_vertical)
	{
		wrk_area_len			= get_size().y;
		math::clamp				(wrk_area_len, 0.0f, wrk_area_len);
		float max_track_len		= wrk_area_len-2*btn_h;
		clamp_min				(max_track_len, 0.0f);
		track_size.y			= max_track_len* (wrk_area_len/m_source->get_length());
		math::clamp				(track_size.y, 0.0f, max_track_len);
	}else{
		UNREACHABLE_CODE();
	}
	
	//position
	float area_pos				= 0.0f;
	if(m_vertical)
	{
		area_pos				= m_source->get_position();
		if(math::is_zero(area_pos))
		{
			track_pos.y			= 0.0f;
		}else
		{
			area_pos				= math::abs(area_pos);
			float max_pos			= wrk_area_len - 2*btn_h - track_size.y;

			float k					= area_pos/(m_source->get_length()-wrk_area_len);

			track_pos.y				= max_pos * k;
		}
		track_pos.y				+= btn_h;

	}else
	{
		UNREACHABLE_CODE();
	}
	m_track_button.set_position	(track_pos);
	m_track_button.set_size		(track_size);

}

bool ui_scroll_bar::on_self_size_changed(window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	if(m_vertical)
	{
		m_btn_lt.set_position	(float2(get_size().x - m_btn_lt.get_size().x, 0));
		m_btn_rb.set_position	(float2(get_size().x - m_btn_rb.get_size().x, get_size().y - m_btn_rb.get_size().y));
	}else
	{
		UNREACHABLE_CODE();
	}

	update_self					();
	return						false;
}

void ui_scroll_bar::step_up()
{
	move				(-m_source->get_step_size());
}

void ui_scroll_bar::step_down()
{
	move				(m_source->get_step_size());
}

void ui_scroll_bar::move_end()
{
	move				(-m_source->get_length());
}

void ui_scroll_bar::move_begin()
{
	move				(m_source->get_length());
}

void ui_scroll_bar::move(float size)
{
	float2 track_pos = m_track_button.get_position();

	if(m_vertical)
	{
		float pad_pos				= m_source->get_position();
		pad_pos						+= size;
		float source_len			= m_source->get_length();
		float min_pos				= (source_len>get_size().y)? -(m_source->get_length() - get_size().y) : 0.0f;
		float max_pos				= 0.0f;

		math::clamp					(pad_pos, min_pos, max_pos);
		m_source->set_position		(pad_pos);
	}else
	{
		UNREACHABLE_CODE();
	}

}

} // namespace xray
} // namespace ui
