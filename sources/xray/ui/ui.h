#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {

namespace render {
namespace ui{
	class renderer;
} // namespace ui
} // namespace render

namespace input		{ struct handler; }

namespace ui{

struct world;

enum enum_font
{
	fnt_arial	= 0,
};

enum enum_text_align
{
	al_left		= 0,
	al_center,
	al_right,
};

enum enum_text_mode
{
	tm_default	= 0,
	tm_multiline,
};

enum enum_window_events
{
	ev_size_changed,
	ev_position_changed,
	ev_focus,
	ev_keyboard,
	ev_parent_changed,
	ev_text_changed,
	ev_edit_text_commit,
};


struct window;

typedef fastdelegate::FastDelegate< bool ( window*, int, int ) >	ui_event_handler;

struct XRAY_NOVTABLE window
{
	virtual void			set_position		(float2 const& pos)				= 0;
	virtual float2 const&	get_position		() const						= 0;
	virtual void			set_size			(const float2& size)			= 0;
	virtual float2 const&	get_size			() const						= 0;

	virtual void			set_visible			(bool vis)						= 0;
	virtual bool			get_visible			() const						= 0;
	
	virtual void			draw				(xray::render::ui::renderer& render, xray::render::scene_view_ptr const& scene_view)	= 0;
	virtual void			tick				()								= 0;

	virtual window*			get_parent			() const						= 0;
	virtual void			set_parent			(window* w)						= 0;
	virtual window*			get_root			()								= 0;

	virtual bool			get_orphan			() const						= 0;
	virtual void			set_orphan			(bool val)						= 0;

	virtual void			set_focused			(bool val)						= 0;
	virtual bool			get_focused			() const						= 0;
	virtual bool			get_tab_stop		() const						= 0;

	virtual void			add_child			(window* w, bool adopt)			= 0;
	virtual void			remove_child		(window* w)						= 0;
	virtual void			remove_all_childs	()								= 0;
	virtual window*			get_child			(u32 idx)				const	= 0;
	virtual u32				get_child_count		()						const	= 0;

	virtual void			subscribe_event		(enum_window_events ev, ui_event_handler handler)	= 0;
	virtual void			unsubscribe_event	(enum_window_events ev, ui_event_handler handler)	= 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( window )
}; // struct window

struct XRAY_NOVTABLE dialog
{
	//virtual void			show_dialog			()								= 0;
	//virtual void			hide_dialog			()								= 0;
	virtual window*			w					()								= 0;
	virtual input::handler*	input_handler		()								= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( dialog )
}; // struct dialog

struct XRAY_NOVTABLE text
{
	virtual void			set_font			(enum_font fnt)					= 0;
	virtual void			set_color			(u32 clr)						= 0;
	virtual void			set_text			(pcstr text)					= 0;
	virtual pcstr			get_text			() const						= 0;
	virtual void			set_align			(enum_text_align  al)			= 0;
	virtual void			set_text_mode		(enum_text_mode tm)				= 0;
	virtual float2			measure_string		() const						= 0;
	virtual window*			w					()								= 0;
	virtual void			fit_height_to_text	()								= 0;
	virtual void			split_and_set_text	(pcstr str, float const width, pcstr& ret_str) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( text )
}; // struct text

struct XRAY_NOVTABLE text_edit
{
	virtual	u16				get_caret_position		() const						= 0;
	virtual	void			set_caret_position		(u16 const, bool b_move=false)	= 0;
	virtual text*			text					()								= 0;
	virtual window*			w						()								= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( text_edit )
}; // struct text_edit

struct XRAY_NOVTABLE image
{
	virtual void			init_texture		(pcstr texture_name)			= 0;
	virtual void			set_color			(u32 clr)						= 0;
	virtual window*			w					()								= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( image )
}; // struct image

struct XRAY_NOVTABLE scroll_source
{
public:
	virtual	float			get_length			()	const			= 0;
	virtual	float			get_position		()	const			= 0;
	virtual	void			set_position		(float const value)	= 0;
	virtual	float			get_step_size		()	const			= 0;
	virtual	void			set_step_size		(float const value)	= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( scroll_source )
}; // struct scroll_source

struct XRAY_NOVTABLE scroll_view
{
	virtual void			add_item			(window* w, bool adopt)			= 0;
	virtual void			remove_item			(window* w)						= 0;
	virtual void			clear				()								= 0;
	virtual bool			get_follow_last_line() const						= 0;
	virtual void			set_follow_last_line(bool val)						= 0;

	virtual u32				get_items_count		() const						= 0;
	virtual window* 		get_item			(u32 idx) const					= 0;
	virtual scroll_source*	get_v_scroll		()								= 0;
	virtual scroll_source*	get_h_scroll		()								= 0;
	virtual window*			w					()								= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( scroll_view )
}; // struct scroll_view

struct XRAY_NOVTABLE font
{
	virtual float3 const&	get_char_tc			(u8 const& ch)	const			= 0;
	virtual float3 			get_char_tc_ts		(u8 const& ch)	const			= 0;
	virtual float const&	get_height			()	const						= 0;
	virtual float			get_height_ts		()	const						= 0;
	virtual void			parse_word			(pcstr str, float& out_length, pcstr& out_next_word) const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( font )
}; // struct font

struct XRAY_NOVTABLE progress_bar
{
	virtual void		set_range					( u32 minimum, u32 maximum )	= 0;

	virtual void		set_minimum					( u32 minimum )					= 0;
	virtual u32			get_minimum					( ) const						= 0;

	virtual void		set_maximum					( u32 maximum )					= 0;
	virtual u32			get_maximum					( ) const						= 0;

	virtual void		set_value					( u32 value )					= 0;
	virtual u32			get_value					( ) const						= 0;

	virtual void		reset						( )								= 0;

	virtual void		set_border_width			( u32 width )					= 0;
	virtual u32			get_border_width			( ) const						= 0;

	virtual void		set_border_height			( u32 height )					= 0;
	virtual u32			get_border_height			( ) const						= 0;

	virtual void		set_back_color				( math::color )					= 0;
	virtual math::color	get_back_color				( ) const						= 0;

	virtual void		set_front_color				( math::color )					= 0;
	virtual math::color	get_front_color				( ) const						= 0;

	virtual void		draw_text					( bool value )					= 0;

	virtual void		set_text_color				( math::color color )			= 0;
	virtual math::color	get_text_color				( ) const						= 0;

	virtual window*		get_window					( )								= 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( progress_bar )
}; // struct progress_bar

} // namespace ui
} // namespace xray

#endif //UI_FONT_H_INCLUDED