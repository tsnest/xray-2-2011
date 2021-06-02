////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_dialog.h"
#include "ui_world.h"

namespace xray {
namespace ui {

ui_dialog::ui_dialog( memory::base_allocator& a )
:super		( a )
{}

ui_dialog::~ui_dialog()
{}

//void ui_dialog::show_dialog()
//{
////	m_ui_world.get_current_holder().show_dialog(this);
//	focus_next		();
//}
//
//void ui_dialog::hide_dialog()
//{
////	get_holder()->hide_dialog	(this);
//}

void ui_dialog::focus_next()
{
	window* prev_focused		= NULL;
	window* new_focused			= NULL;

	calc_focus_next				(m_childs, prev_focused, new_focused);
	if(prev_focused)
		prev_focused->set_focused	(false);

	if(new_focused)
		new_focused->set_focused	(true);
}

bool ui_dialog::calc_focus_next(window_list& childs, window*& prev_focused, window*& new_focused)
{
	window_list_it	it		= childs.begin();
	window_list_it	it_e	= childs.end();

	for(; it!=it_e; ++it)
	{
		ui_window* w = static_cast_checked<ui_window*>(*it);
		if(!w->m_childs.empty())
			if(calc_focus_next(w->m_childs, prev_focused, new_focused))
				break;

		if(w->get_focused())
			prev_focused	= w;
		else
		if(w->get_tab_stop() && prev_focused)
		{
			new_focused = w;
			break;
		}
	}

	return true;
}

} // namespace xray
} // namespace ui
