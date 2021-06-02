////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "game_dialog_container.h"
#include "dialog.h"
#include "dialogs_manager.h"
#include "dialog_editor_resource.h"

#pragma unmanaged
using xray::dialog_editor::game_dialog_container;
using xray::dialog_editor::dialog;

game_dialog_container::game_dialog_container(pstr new_text)
:m_text(new_text)
{
	m_resource = NEW(game_dialog_ptr)();//NULL;
}

game_dialog_container::~game_dialog_container()
{
	(*m_resource) = NULL;
	DELETE(m_resource);
}

void game_dialog_container::set_text(pcstr new_text)
{
	XRAY_UNREFERENCED_PARAMETER(new_text);
}

pcstr game_dialog_container::text()
{
	if(!(*m_resource))
		return m_text;
	else
		return (*m_resource)->text();
}

void game_dialog_container::request_dialog()
{
	dialog_manager::get_dialogs_manager()->request_game_dialog(m_text, boost::bind(&game_dialog_container::on_dialog_loaded, this, _1));
}

void game_dialog_container::on_dialog_loaded(xray::dialog_editor::game_dialog_ptr p)
{
	(*m_resource) = p.c_ptr();
}

xray::dialog_editor::dialog_node_base* game_dialog_container::get_root(u32 index) const
{
	if(!(*m_resource))
		return NULL;
	else
		return (*m_resource)->get_root(index);
}

u32 game_dialog_container::get_roots_count() const
{
	if(!(*m_resource))
		return 0;
	else
		return (*m_resource)->get_roots_count();
}
