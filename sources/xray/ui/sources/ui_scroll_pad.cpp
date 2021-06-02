////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_scroll_pad.h"

namespace xray {
namespace ui {

struct pred_window_less_position
{
bool operator	()(window* w1, float pos)
	{
		return w1->get_position().y+w1->get_size().y < pos;
	}
bool operator	()(float pos, window* w1)
	{
		return pos < w1->get_position().y+w1->get_size().y;
	}
};

ui_scroll_pad::ui_scroll_pad( memory::base_allocator& a )
:super	( a )
{}

void ui_scroll_pad::draw(xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view)
{
	// no need to call super::draw() !!!
	float2						range(float2(0, get_parent()->get_size().y));
	range.x						-= get_position().y;
	range.y						-= get_position().y;

	pred_window_less_position	p;
	window_list_it it			= std::lower_bound(m_childs.begin(), m_childs.end(), range.x, p);
	window_list_it it_e			= std::upper_bound(m_childs.begin(), m_childs.end(), range.y, p);

	for(; it!=it_e; ++it)
		(*it)->draw				(renderer, scene_view);
}

void ui_scroll_pad::tick()
{
	// no need to call super::draw() !!!
	float2						range(float2(0, get_parent()->get_size().y));
	range.x						-= get_position().y;
	range.y						-= get_position().y;

	pred_window_less_position	p;
	window_list_it it			= std::lower_bound(m_childs.begin(), m_childs.end(), range.x, p);
	window_list_it it_e			= std::upper_bound(m_childs.begin(), m_childs.end(), range.y, p);

	for(; it!=it_e; ++it)
		(*it)->tick			();
}

} // namespace xray
} // namespace ui
