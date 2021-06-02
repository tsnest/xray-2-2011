////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_reparent_items.h"
#include "project.h"

namespace xray {
namespace editor {

command_reparent_items::command_reparent_items( project^ p, 
											   string_list^ src_names, 
											   System::String^ new_parent )
:m_project( p )
{
	for each(System::String^ src_name in src_names)
	{
		move_item^ mi = gcnew move_item;
		mi->src_name	= src_name;
		mi->dst_name	= new_parent;
		m_storage.Add	( mi );
	}
}

bool command_reparent_items::commit()
{
	for each (move_item^ mi in m_storage)
	{
		project_item_base^ pi			= m_project->find_by_path( mi->src_name, true );
		System::String^ parent_name		= pi->m_parent->get_full_name();

		m_project->move					( mi->src_name, mi->dst_name );

		mi->src_name	= pi->get_full_name();
		mi->dst_name	= parent_name;
	}
	return true;
}

void command_reparent_items::rollback()
{
	commit ();
}

} // namespace editor
} // namespace xray
