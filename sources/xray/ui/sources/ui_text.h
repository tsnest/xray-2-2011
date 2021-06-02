#ifndef UI_TEXT_H_INCLUDED
#define UI_TEXT_H_INCLUDED

#include "ui_world.h"
#include <xray/render/world.h>
#include <xray/render/facade/ui_renderer.h>

#include "ui_font.h"
#include "ui_window.h"

namespace xray {
namespace ui {


struct static_text
{
	inline pcstr	c_str	( )	const		{ return m_text.c_str();}
	inline u16		length	( ) const		{ return (m_text) ? (u16)m_text.length() : 0;}
	inline void		set		( pcstr str)	{ m_text=str; }
private:
	shared_string	m_text;
};

template < class T >
class ui_text :	public ui::text,
				public ui_window
{
public:
						ui_text			( ui_world& w );
	virtual				~ui_text		( );
	virtual void		set_font		( enum_font fnt );
	virtual void		set_color		( u32 clr );
	virtual void		set_text		( pcstr text );
	virtual pcstr		get_text		( ) const						{return m_text.c_str();};
	virtual void		set_align		( enum_text_align al );
	virtual void		set_text_mode	( enum_text_mode tm );
	virtual float2		measure_string	( ) const;
	virtual void		fit_height_to_text( );
	virtual void		split_and_set_text( pcstr str, float const width, pcstr& ret_str );

	virtual void		draw			( xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view );
	virtual window*		w				( )								{return this;}
protected:
			void		draw_internal	( xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view, u32 idx1, u32 idx2, u32 sel_clr );
			bool		on_text_changed	( window* w, int p1, int p2 );

//	shared_string						m_text_;
	T									m_text;
	ui_font const*						m_font;
	enum_text_align						m_align;
	enum_text_mode						m_mode;
	u32									m_color;
	ui_world&							m_ui_world;
};

void parse_word(pcstr str, ui_font const* font, float& length, pcstr& next_word);

#include "ui_text_inline.h"

} // namespace xray
} // namespace ui

#endif //UI_TEXT_H_INCLUDED