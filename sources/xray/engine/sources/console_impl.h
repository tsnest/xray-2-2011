////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef CONSOLE_IMPL_H_INCLUDED
#define CONSOLE_IMPL_H_INCLUDED

#include <xray/input/handler.h>

namespace xray {

namespace ui {
	struct world;
	struct window;
	struct image;
	struct dialog;
	struct scroll_view;
	struct text;
	struct text_edit;
}//namespace ui


enum enum_tips_mode		{ tm_none, tm_history, tm_commands_list, tm_arg_list };

class console_impl :private boost::noncopyable
{
public:
	console_impl		( ui::world& uw, xray::memory::base_allocator& a );
	virtual				~console_impl		( );

	virtual	void			on_activate			( );
	virtual	void			on_deactivate		( );
			void			tick				( );

			bool			on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
			bool			on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
			bool			on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
			bool			on_mouse_move		( input::world* input_world, int x, int y, int z );
			int				input_priority		( ) { return 5; }

private:
			bool			on_text_commit		( ui::window* w, int p1, int p2 );
			bool			on_text_changed		( ui::window* w, int p1, int p2 );
			void			fill_tips_view		( );
			ui::text*		get_item			( );

protected:
			bool			get_active			( )	const;
			bool			m_self_deactivate;
private:
	u32						m_last_item;
	float					m_last_position;
	xray::memory::base_allocator& m_allocator;
	ui::dialog*				m_ui_dialog;
	ui::scroll_view*		m_ui_view;
	ui::text_edit*			m_text_edit;
	ui::world&				m_ui_world;
	vectora<ui::text*>		m_text_items;
	u32						m_last_log_count;

	ui::image*				m_ui_tips_view;
	ui::image*				m_ui_tips_view_hl;

	vectora<pcstr>			m_executed_history;
	string512				m_current_command_info;

	vectora<pcstr>			m_tips;
	enum_tips_mode			m_tips_mode;
	bool					m_active;
};//class console_impl

}//namespace xray

#endif //CONSOLE_IMPL_H_INCLUDED