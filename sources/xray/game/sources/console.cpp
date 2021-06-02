////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "console.h"
#include "game.h"

#include <xray/ui/ui.h>
#include <xray/ui/world.h>
#include <xray/ui/sources/ui_font.h>

#include <xray/logging_log_file.h>
#include <xray/console_command_processor.h>
#include <xray/console_command.h>

using xray::console_commands::console_command;

namespace stalker2{

console::console( game& g )
:super			( g ),
m_ui_world		( g.ui_world() ),
m_last_item		( 0 ),
m_last_position	( 0.0f ),
m_last_log_count( 0 ),
m_tips_mode		( tm_none ),
m_active		( false )
{
	m_ui_dialog						= m_ui_world.create_dialog();
	m_ui_dialog->w()->set_position	(float2(0,0));
	m_ui_dialog->w()->set_size		(float2(1020,530));

	ui::image* img					= m_ui_world.create_image();
	img->init_texture				("ui_rect");
	img->set_color					(0xff404040);
	img->w()->set_size				(m_ui_dialog->w()->get_size());
	img->w()->set_position			(float2(0,0));
	img->w()->set_visible			(true);
	m_ui_dialog->w()->add_child		(img->w(), true);

	m_ui_view						= m_ui_world.create_scroll_view();
	m_ui_view->w()->set_position	(float2(1,1));
	m_ui_view->w()->set_size		(float2(1018,500));
	m_ui_view->w()->set_visible		(true);
	m_ui_dialog->w()->add_child		(m_ui_view->w(), true);

	m_text_edit						= m_ui_world.create_text_edit();
	float2 edit_pos					(1,510);
	float2 edit_size				(1018,18);
	m_text_edit->w()->set_position	(edit_pos);
	m_text_edit->w()->set_size		(edit_size);
	m_text_edit->w()->set_visible	(true);
	m_text_edit->text()->set_font	(ui::fnt_arial);
	m_text_edit->text()->set_color	(0xfff0f0f0);
	m_text_edit->text()->set_text_mode(ui::tm_default);

	img								= m_ui_world.create_image();
	img->init_texture				("ui_rect");
	img->set_color					(0xff353535);
	img->w()->set_size				(m_text_edit->w()->get_size());
	img->w()->set_position			(edit_pos);
	img->w()->set_visible			(true);
	m_ui_dialog->w()->add_child		(img->w(), true);
	m_ui_dialog->w()->add_child		(m_text_edit->w(), true);

	m_text_edit->text()->set_text	("");
	m_text_edit->w()->subscribe_event(ui::ev_edit_text_commit, ui::ui_event_handler(this, &console::on_text_commit));
	m_text_edit->w()->subscribe_event(ui::ev_text_changed, ui::ui_event_handler(this, &console::on_text_changed));

	m_ui_tips_view					= m_ui_world.create_image();
	m_ui_tips_view->init_texture	("ui_rect");
	m_ui_tips_view->set_color		(0xff353535);
	
	m_ui_tips_view->w()->set_position(edit_pos+float2(0, edit_size.y));
	m_ui_tips_view->w()->set_size	(float2(100,100));
	m_ui_tips_view->w()->set_visible(false);
	m_ui_dialog->w()->add_child		(m_ui_tips_view->w(), true);

	m_ui_tips_view_hl				= m_ui_world.create_image();
	m_ui_tips_view_hl->init_texture	("ui_rect");
	m_ui_tips_view_hl->set_color	(0x7f200000);
	
	m_ui_tips_view_hl->w()->set_position(float2(0.0f, 0.0f));
	m_ui_tips_view_hl->w()->set_size	(float2(100,100));
	m_ui_tips_view_hl->w()->set_visible	(true);
}

console::~console()
{
	m_ui_world.destroy_window		(m_ui_dialog->w());
	m_ui_world.destroy_window		(m_ui_tips_view_hl->w());

	vector<ui::text*>::iterator it		= m_text_items.begin();
	vector<ui::text*>::iterator it_e	= m_text_items.end();
	
	for(; it!=it_e; ++it)
		m_ui_world.destroy_window		((*it)->w());

	vector<pcstr>::iterator it2			= m_executed_history.begin();
	vector<pcstr>::iterator it2_e		= m_executed_history.end();

	for(; it2!=it2_e; ++it2)
	{
		pvoid p		= (pvoid)*it2;
		FREE		(p);
	}
}

ui::text* console::get_item()
{
	ui::text* result	= NULL;

	if(m_last_item>=m_text_items.size())
	{
		result			= m_ui_world.create_text();
		m_text_items.push_back	(result);
		++m_last_item;
		return result;
	}else
	{
		result = m_text_items[m_last_item];
		++m_last_item;
		return result;
	}
}

void console::on_activate()
{
	super::on_activate				( );
//	m_ui_dialog->show_dialog		( );
	m_text_edit->w()->set_focused	( true );
	m_active						= true;
}

void console::on_deactivate()
{
	super::on_deactivate			( );
//	m_ui_dialog->hide_dialog		( );
	m_text_edit->w()->set_focused	( false );
	m_ui_view->w()->set_focused		( false );
	m_active						= false;
}

bool console::get_active()	const
{
	return m_active;
}

static u32 string_color(pcstr str)
{
	if(strstr(str, "<info>"))
		return 0xfff0f0f0;
	if(strstr(str, "<Warning>"))
		return 0xffff8080;
	if(strstr(str, "<ERROR>"))
		return 0xffff0000;
	
	return 0xffffffff;
}

struct logging_transaction
{
	logging_transaction(logging::log_file* l) : l_(l)	{ l_->start_transaction(); }
	~logging_transaction()								{ l_->end_transaction(); }
	logging::log_file*	l_;
};

struct dialog_guard :private boost::noncopyable
{
	dialog_guard(ui::window& d, game& g):m_ui_window(d), m_game(g){}
	~dialog_guard(){
		m_ui_window.tick();
		m_ui_window.draw(m_game.render_world());
	}
private:	
	ui::window&	m_ui_window;
	game&		m_game;
};

void console::tick( )
{
	ASSERT (get_active() );

	float line_height					= 20.0f;
	float line_width					= 1000.0f;
	logging::log_file* l				= logging::get_log_file();

	logging_transaction		transaction (l);
	dialog_guard			dialog_updater(*m_ui_dialog->w(), m_game);
	u32 log_cnt							= l->get_lines_count();
	if(log_cnt==0)
		return;

	ui::scroll_source* scroll_v			= m_ui_view->get_v_scroll();
	float scroll_pos					= math::abs(scroll_v->get_position());
	bool follow_last_line				= m_ui_view->get_follow_last_line();
	bool pos_changed					= !math::similar(scroll_pos, m_last_position);

	if(!pos_changed && !follow_last_line)
	{
		//update only pad size
		if(log_cnt!=m_last_log_count)
		{
			u32 ic					= m_ui_view->get_items_count();
			ui::window* w			= m_ui_view->get_item(ic-1);//last
			float2 sz				= w->get_size();
			sz.y					+= (log_cnt-m_last_log_count)*line_height;
			w->set_size				(sz);
			m_ui_view->remove_item	(w);
			m_ui_view->add_item		(w, false);

			m_last_log_count		= log_cnt;
		}
		return;
	}

	if(!pos_changed && log_cnt==m_last_log_count)
		return;

	m_ui_view->clear			();
	m_last_item					= 0;
	
	float visible_height		= m_ui_view->w()->get_size().y;
	u32 need_lines_count		= math::floor(visible_height/line_height);

	if(follow_last_line && log_cnt>need_lines_count)
		scroll_pos				= log_cnt*line_height - visible_height; //make last line visible

	ui::text* txt				= get_item();
	txt->w()->set_size			(float2(line_width, scroll_pos));
	m_ui_view->add_item			(txt->w(), false);
	
	u32 log_pos					= math::floor(scroll_pos/line_height);
	math::clamp<u32>			(need_lines_count, 0, log_cnt);

	math::clamp<u32>			(log_pos, 0, log_cnt-need_lines_count);
	
	l->goto_line				(log_pos);

	string1024					log_str_buffer;
	pcstr line					= NULL;

	for(u32 i=0; i<need_lines_count; ++i)
	{
		txt						= get_item();
		txt->w()->set_position	(float2(0,0));
		txt->w()->set_size		(float2(line_width, line_height));
		txt->w()->set_visible	(true);
		txt->set_text_mode		(ui::tm_default);
		txt->set_color			(string_color(log_str_buffer));
		txt->set_font			(ui::fnt_arial);
		if(line==NULL)
		{
			l->read_next_line	(log_str_buffer);
			txt->split_and_set_text(log_str_buffer, line_width, line);
		}
		else
			txt->split_and_set_text( line, line_width, line );

		m_ui_view->add_item		( txt->w(), false );
	}
	
	float len					= log_cnt*line_height;
	float2 tail_size			( line_width, len-scroll_pos-visible_height );
	if(tail_size.y>0.0f)
	{
		txt						= get_item( );
		txt->w()->set_size		( tail_size );
		m_ui_view->add_item		( txt->w(), false );
	}

	m_last_log_count			= log_cnt;
	m_last_position				= scroll_pos;
	scroll_v->set_step_size		( line_height );
}

struct compare_pcstr_pred
{
	pcstr	str;
			compare_pcstr_pred(pcstr s):str(s){}
	bool operator ()(pcstr s) const
	{
		return 0==strings::compare(str, s);
	}
};
bool console::on_text_commit(ui::window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	pcstr text							= m_text_edit->text()->get_text();
	LOG_INFO							("~%s", text);
	console_commands::execute			(text);

	compare_pcstr_pred					pred(text);

	
	vector<pcstr>::iterator it			= std::find_if(m_executed_history.begin(), m_executed_history.end(), pred);
	if(it!=m_executed_history.end())
	{
		vector<pcstr>::iterator it_b	= m_executed_history.begin();
		std::swap						(*it, *it_b);
	}else
		m_executed_history.push_back	(strings::duplicate(*g_allocator, text));

	m_text_edit->text()->set_text		("");
	return								true;
}

bool console::on_text_changed(ui::window* w, int p1, int p2)
{
	XRAY_UNREFERENCED_PARAMETERS	(w, p1, p2);

	pcstr text				= m_text_edit->text()->get_text();
	u32 text_len			= strings::length(text);

	if(m_tips_mode!=tm_none && text_len>0)
		return				false;

	console_command*		storage [10];

	u32 res					= (text_len>0) ? console_commands::get_similar(text, storage, 10) : 0;
	m_tips.clear			();
	for(u32 idx=0; idx<res; ++idx)
		m_tips.push_back	(storage[idx]->name());

	if(res==0)
	{
		string512			cmd_name;
		strings::copy		(cmd_name, sizeof(cmd_name), m_text_edit->text()->get_text());

		u32 len				= strings::length(cmd_name);

		while(len>0)
			if(cmd_name[--len]==' ')
				cmd_name[len] = 0;

		console_commands::console_command* current = console_commands::find(cmd_name);
		if(current)
		{
			console_command::info_str	info_str;
			console_command::status_str status_str;
			current->info				(info_str);
			current->status				(status_str);
			xray::sprintf				(m_current_command_info, "%s current is <%s>", info_str, status_str);
			m_tips.clear				();
			m_tips.push_back			(m_current_command_info);
		}
	}

	fill_tips_view			();
	return					false;
}

void console::fill_tips_view()
{
	float2 line_size		(100.0f, 20.0f);
	float pos				= 0.0f;

	m_ui_tips_view->w()->remove_all_childs();
	
	u32 tips_count			 = m_tips.size();

	for (u32 idx=0; idx<tips_count; ++idx)
	{
		pcstr str				= m_tips[idx];

		ui::text* txt			= get_item();
		txt->set_text			(str);
		txt->set_font			(ui::fnt_arial);
		txt->set_text_mode		(ui::tm_default);
		line_size.x				= math::max(line_size.x, txt->measure_string().x);
		txt->w()->set_size		(line_size);
		txt->w()->set_position	(float2(0.0f, pos));
		txt->w()->set_visible	(true);
		txt->set_color			(0xff00ff00);
		m_ui_tips_view->w()->add_child(txt->w(), false);
		pos						+= line_size.y;
	}
	m_ui_tips_view->w()->set_size	(float2(line_size.x, pos));
	m_ui_tips_view->w()->set_visible	(tips_count>0);
}

} // namespace stalker2
