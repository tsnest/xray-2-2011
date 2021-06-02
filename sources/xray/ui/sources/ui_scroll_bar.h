////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef UI_SCROLL_BAR_H_INCLUDED
#define UI_SCROLL_BAR_H_INCLUDED

#include "ui_image.h"

namespace xray {
namespace ui {


class ui_scroll_bar :	public ui_image
{
	typedef ui_image	super;
public:
					ui_scroll_bar			( memory::base_allocator& a );
			void	init					(scroll_source* src, bool b_vertical);
			void	update_self				();
			void	move					(float size);
			void	move_end				();
			void	move_begin				();
protected:
			void	step_up					();
			void	step_down				();
			bool	on_self_size_changed	(window* w, int p1, int p2);

	ui_image		m_track_button;

	scroll_source*	m_source;
	ui_window		m_btn_lt;	//replace next with button class
	ui_window		m_btn_rb;
	bool			m_vertical;

};

} // namespace xray
} // namespace ui

#endif //UI_SCROLL_BAR_H_INCLUDED