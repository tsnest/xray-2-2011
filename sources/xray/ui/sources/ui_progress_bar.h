////////////////////////////////////////////////////////////////////////////
//	Created		: 14.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UI_PROGRESS_BAR_H_INCLUDED
#define UI_PROGRESS_BAR_H_INCLUDED

#include "ui_world.h"
#include "ui_window.h"

namespace xray {
namespace ui {

class ui_progress_bar : public progress_bar,
						public ui_window
{
public:
						ui_progress_bar		( ui_world& world );
	virtual 			~ui_progress_bar	( );
	
	virtual void		set_range					( u32 minimum, u32 maximum );

	virtual void		set_minimum					( u32 minimum );
	virtual u32			get_minimum					( ) const;

	virtual void		set_maximum					( u32 maximum );
	virtual u32			get_maximum					( ) const;

	virtual void		set_value					( u32 value );
	virtual u32			get_value					( ) const;

	virtual void		reset						( );

	virtual void		set_border_width			( u32 width );
	virtual u32			get_border_width			( ) const;

	virtual void		set_border_height			( u32 height );
	virtual u32			get_border_height			( ) const;

	virtual void		set_back_color				( math::color color );
	virtual math::color	get_back_color				( ) const;

	virtual void		set_front_color				( math::color color );
	virtual math::color	get_front_color				( ) const;

	virtual void		draw_text					( bool value );

	virtual void		set_text_color				( math::color color );
	virtual math::color	get_text_color				( ) const;

	virtual void		draw						(
														xray::render::ui::renderer& renderer,
														xray::render::scene_view_ptr const& scene_view
													);

	virtual window*		get_window					( ) { return this; }
private:
			void		draw_back_rectangle			(
														xray::render::ui::renderer& renderer,
														xray::render::scene_view_ptr const& scene_view
													) const;
			void		draw_front_rectangle		(
														xray::render::ui::renderer& renderer,
														xray::render::scene_view_ptr const& scene_view
													) const;
			void		draw_text					(
														xray::render::ui::renderer& renderer,
														xray::render::scene_view_ptr const& scene_view
													) const;
private:
	ui_world&			m_ui_world;
	math::color			m_back_color;
	math::color			m_front_color;
	math::color			m_text_color;
	u32					m_border_width;
	u32					m_border_height;
	u32					m_minimum;
	u32					m_maximum;
	u32					m_value;
	bool				m_draw_text;
}; // class progress_bar

} // namespace ui
} // namespace xray

#endif // #ifndef UI_PROGRESS_BAR_H_INCLUDED