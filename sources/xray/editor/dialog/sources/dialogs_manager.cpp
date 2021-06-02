////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialogs_manager.h"
#include "dialog_editor_resource.h"
#include "dialog.h"
#include "game_dialog.h"

#pragma unmanaged

namespace boost {
	void throw_exception(std::exception const & e)
	{
		FATAL("boost exception caught: %s", e.what());
	}
} // namespace boost

using xray::dialog_editor::dialog_manager::dialogs_manager;

static xray::uninitialized_reference<dialogs_manager> g_dialogs_manager;
void xray::dialog_editor::dialog_manager::create_dialogs_manager()
{
	XRAY_CONSTRUCT_REFERENCE(g_dialogs_manager, dialogs_manager)();
}

void xray::dialog_editor::dialog_manager::destroy_dialogs_manager()
{
	XRAY_DESTROY_REFERENCE(g_dialogs_manager);
}

dialogs_manager* xray::dialog_editor::dialog_manager::get_dialogs_manager()
{
	return (&*g_dialogs_manager);
}
////////////////////////////////////////////////////////////////////////////
//- class dialogs_manager -------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
dialogs_manager::dialogs_manager()
{
	m_editor_dialogs_list = editor_dialogs_list();
	m_editor_callbacks = editor_callbacks_list();
	m_game_dialogs_list = game_dialogs_list();
	m_game_callbacks = game_callbacks_list();
}

dialogs_manager::~dialogs_manager()
{
	editor_dialogs_list::iterator ed_dial_it = m_editor_dialogs_list.begin();
	for(; ed_dial_it!=m_editor_dialogs_list.end(); ++ed_dial_it)
		ed_dial_it->second = NULL;
	m_editor_dialogs_list.clear();

	editor_callbacks_list::iterator ed_cb_it = m_editor_callbacks.begin();
	for(; ed_cb_it!=m_editor_callbacks.end(); ++ed_cb_it)
		FREE(ed_cb_it->first);
	m_editor_callbacks.clear();

	game_dialogs_list::iterator game_dial_it = m_game_dialogs_list.begin();
	for(; game_dial_it!=m_game_dialogs_list.end(); ++game_dial_it)
		game_dial_it->second = NULL;
	m_game_dialogs_list.clear();

	game_callbacks_list::iterator game_cb_it = m_game_callbacks.begin();
	for(; game_cb_it!=m_game_callbacks.end(); ++game_cb_it)
		FREE(game_cb_it->first);
	m_game_callbacks.clear();
}

void dialogs_manager::tick()
{
	editor_dialogs_list::iterator ed_dial_it = m_editor_dialogs_list.begin();
	for(; ed_dial_it!=m_editor_dialogs_list.end(); ++ed_dial_it)
	{
		if(ed_dial_it->second->reference_count()==1)
		{
			ed_dial_it->second = NULL;
			m_editor_dialogs_list.erase(ed_dial_it);
			break;
		}
	}

	game_dialogs_list::iterator game_dial_it = m_game_dialogs_list.begin();
	for(; game_dial_it!=m_game_dialogs_list.end(); ++game_dial_it)
	{
		if(game_dial_it->second->reference_count()==1)
		{
			game_dial_it->second = NULL;
			m_game_dialogs_list.erase(game_dial_it);
			break;
		}
	}
}

void dialogs_manager::request_editor_dialog(pcstr name, xray::dialog_editor::dialog_manager_callback const& callback)
{	
	editor_dialogs_list::const_iterator it = m_editor_dialogs_list.find(name);
	if(it!=m_editor_dialogs_list.end())
		callback(it->second);
	else
	{
		m_editor_callbacks[strings::duplicate(g_allocator, name)] = callback;
		fs_new::virtual_path_string dialog_path;
		dialog_path	+= name;
		dialog_path	+= ".resource";

		xray::resources::query_resource(
			dialog_path.c_str(),
			xray::resources::dialog_resources_class,
			boost::bind(&dialogs_manager::on_editor_dialog_loaded, this, _1),
			g_allocator,
			0,
			0,
			assert_on_fail_false
			);
	}
}

void dialogs_manager::on_editor_dialog_loaded(xray::resources::queries_result& result)
{
	R_ASSERT(result[0].is_successful());

	dialog_resources_ptr res = static_cast_resource_ptr<dialog_resources_ptr>(result[0].get_unmanaged_resource());
	pcstr name = res->get_dialog()->name();
	editor_dialogs_list::iterator it = (m_editor_dialogs_list.insert(std::pair<pcstr, dialog_resources_ptr>(name, res.c_ptr()))).first;
	editor_callbacks_list::iterator f_it = m_editor_callbacks.find(name);
	if(f_it!=m_editor_callbacks.end())
	{
		f_it->second(it->second);
		FREE(f_it->first);
		m_editor_callbacks.erase(f_it);
	}
}

void dialogs_manager::request_game_dialog(pcstr name, xray::dialog_editor::game_dialog_callback const& callback)
{	
	game_dialogs_list::const_iterator it = m_game_dialogs_list.find(name);
	if(it!=m_game_dialogs_list.end())
		callback(it->second);
	else
	{
		m_game_callbacks[strings::duplicate(g_allocator, name)] = callback;
		xray::resources::query_resource(
			name,
			xray::resources::game_dialog_class,
			boost::bind(&dialogs_manager::on_game_dialog_loaded, this, _1),
			g_allocator
			);
	}
}

void dialogs_manager::on_game_dialog_loaded(xray::resources::queries_result& result)
{
	R_ASSERT(result[0].is_successful());

	game_dialog_ptr res = static_cast_resource_ptr<game_dialog_ptr>(result[0].get_unmanaged_resource());
	pcstr name = res.c_ptr()->text();
	game_dialogs_list::iterator it = (m_game_dialogs_list.insert(std::pair<pcstr, game_dialog_ptr>(name, res.c_ptr()))).first;
	game_callbacks_list::iterator f_it = m_game_callbacks.find(name);
	if(f_it!=m_game_callbacks.end())
	{
		f_it->second(it->second);
		FREE(f_it->first);
		m_game_callbacks.erase(f_it);
	}
}
