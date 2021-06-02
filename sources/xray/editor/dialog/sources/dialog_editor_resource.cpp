////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor_resource.h"
#include "dialog.h"
#include "dialog_graph_node_layout.h"

#pragma unmanaged

using xray::dialog_editor::dialog_resources;
using xray::dialog_editor::dialog;

dialog_resources::dialog_resources()
{
	m_dialog = dialog_ptr();
	m_layout = NULL;
}

dialog_resources::~dialog_resources()
{
	m_dialog = NULL;
	DELETE(m_layout);
}

void dialog_resources::save(pcstr path)
{
	string512 file_path;
	sprintf_s(file_path, "%s.dlg", path);
	configs::lua_config_ptr const& cfg = configs::create_lua_config(file_path);
	get_dialog()->save(cfg->get_root());
	cfg->save(configs::target_sources);

	sprintf_s(file_path, "%s.lt", path);
	configs::lua_config_ptr const& cfg2 = configs::create_lua_config(file_path);
	m_layout->save(cfg2->get_root());
	cfg2->save(configs::target_sources);
}

dialog* dialog_resources::get_dialog()
{
	return m_dialog.c_ptr();
}
