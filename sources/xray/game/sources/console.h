////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "scene.h"

namespace xray {
namespace ui {
	struct window;
	struct image;
	struct dialog;
	struct scroll_view;
	struct text;
	struct text_edit;
}//namespace ui
}//namespace xray

namespace stalker2 {

class console : public scene
{
	typedef scene			super;
public:
							console				( game& g);
							~console			( );
			bool			get_active			( )	const;
	virtual void			on_activate			( );
	virtual void			on_deactivate		( );
	virtual void			tick				( );

// input handler interface
	virtual	bool			on_keyboard_action	( input::world& input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool			on_gamepad_action	( input::world& input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool			on_mouse_key_action	( input::world& input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool			on_mouse_move		( input::world& input_world, int x, int y, int z );

private:
		bool				on_text_commit		( ui::window* w, int p1, int p2 );
		bool				on_text_changed		( ui::window* w, int p1, int p2 );
		void				fill_tips_view		( );

	ui::text*				get_item			( );
	u32						m_last_item;
	float					m_last_position;
	ui::dialog*				m_ui_dialog;
	ui::scroll_view*		m_ui_view;
	ui::text_edit*			m_text_edit;
	ui::world&				m_ui_world;
	vector<ui::text*>		m_text_items;
	u32						m_last_log_count;

	ui::image*				m_ui_tips_view;
	ui::image*				m_ui_tips_view_hl;

	vector<pcstr>			m_executed_history;
	string512				m_current_command_info;

	vector<pcstr>			m_tips;
	enum enum_tips_mode		{tm_none, tm_history, tm_list};
	enum_tips_mode			m_tips_mode;
	bool					m_active;
};//class console

}//namespace stalker2

#endif //CONSOLE_H_INCLUDED