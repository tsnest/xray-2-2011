////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "options_page_project.h"
#include "editor_world.h"
#include "level_editor.h"
#include "project.h"

namespace xray{
namespace editor{

bool options_page_project::accept_changes( )
{
	return true;
}

void options_page_project::cancel_changes( )
{
	m_editor_world.get_level_editor()->get_project()->track_active_item = System::Convert::ToBoolean(m_backup_values["track_active_item"]);
}

void options_page_project::activate_page( )
{
	bool current_value						= m_editor_world.get_level_editor()->get_project()->track_active_item;
	m_backup_values["track_active_item"]	= current_value;
	track_active_item_check->Checked		= current_value;
}

void options_page_project::deactivate_page( )
{
}

bool options_page_project::changed( )
{
	bool current_value		= m_editor_world.get_level_editor()->get_project()->track_active_item;
	bool backup_value		= System::Convert::ToBoolean(m_backup_values["track_active_item"]);
	bool changed			= (current_value!=backup_value);
	return					changed;
}

void options_page_project::track_active_item_check_CheckedChanged( System::Object^, System::EventArgs^ )
{
	m_editor_world.get_level_editor()->get_project()->track_active_item = track_active_item_check->Checked;
}


}
}