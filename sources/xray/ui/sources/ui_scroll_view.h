////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef UI_SCROLL_VIEW_H_INCLUDED
#define UI_SCROLL_VIEW_H_INCLUDED

#include "ui_window.h"
#include "ui_scroll_bar.h"
#include "ui_scroll_pad.h"

namespace xray {
namespace ui {


class ui_scroll_v_source :public scroll_source
{
public:
						ui_scroll_v_source		();
			void		set_pad					(ui_scroll_pad* p);
	virtual	float		get_length				()	const;
	virtual	float		get_position			()	const;
	virtual	void		set_position			(float const value);
	virtual	float		get_step_size			()	const			{return m_step;}
	virtual	void		set_step_size			(float const value)	{m_step=value;}

private:
	ui_scroll_pad*		m_pad;
	float				m_step;
};

class ui_scroll_view :	public ui::scroll_view,
						public ui_window
{
	typedef ui_window	super;
public:
	ui_scroll_view			( memory::base_allocator& a );
	virtual				~ui_scroll_view			();
	virtual void		add_item				(window* w, bool adopt);
	virtual void		remove_item				(window* w);
	virtual void		clear					();
	
	virtual bool		get_follow_last_line	() const				{return !!(m_flags&fl_follow_last);}
	virtual void		set_follow_last_line	(bool val);

	virtual u32			get_items_count			() const;
	virtual window* 	get_item				(u32 idx) const;
	virtual scroll_source*	get_v_scroll		() 			{return &m_scroll_source_v;};
	virtual scroll_source*	get_h_scroll		() 			{return NULL;};

	virtual void		draw					(xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view);
	virtual void		tick					();
	virtual window*		w						()								{return this;}
protected:
			bool		on_self_size_changed	(window* w, int p1, int p2);
			bool		on_pad_size_changed		(window* w, int p1, int p2);
			bool		on_pad_pos_changed		(window* w, int p1, int p2);
			bool		on_focus				(window* w, int p1, int p2);
			bool		on_keyb_action			(window* w, int p1, int p2); //tmp
			void		recalc					();

	enum enum_flags{
		fl_need_recalc	= (1<<0),
		fl_follow_last	= (1<<1),
	};
	u16					m_flags;
	ui_scroll_pad		m_pad;
	ui_scroll_bar		m_scroll_bar_v;
	ui_scroll_v_source	m_scroll_source_v;
};


} // namespace xray
} // namespace ui

#endif //UI_SCROLL_VIEW_H_INCLUDED