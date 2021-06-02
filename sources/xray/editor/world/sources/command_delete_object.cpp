
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_delete_object.h"

#include "level_editor.h"
#include "tool_base.h"
#include "object_base.h"
#include "project.h"
#include "command_select.h"
#include "project_items.h"

namespace xray {
namespace editor {


command_delete_objects::command_delete_objects( level_editor^ le, int id )
:m_level_editor	( le )
{
	m_ids.Add	( id );
}

command_delete_objects::command_delete_objects( level_editor^ le, item_id_list^ objects_ids )
:m_level_editor	( le )
{
	m_ids.AddRange	( objects_ids );
}

bool command_delete_objects::commit( )
{
	m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project()) );

	return	m_level_editor->get_command_engine()->run( gcnew command_delete_object_impl(m_level_editor, %m_ids ));
}

command_delete_object_impl::command_delete_object_impl( level_editor^ le, item_id_list^ objects_id ):
m_level_editor		( le )
{
	m_ids		= objects_id;
	m_configs	= NEW (configs::lua_config_ptr)();
	*m_configs	= configs::create_lua_config();
}

command_delete_object_impl::~command_delete_object_impl( )
{
	DELETE (m_configs);
}

bool command_delete_object_impl::commit( )
{
	configs::lua_config_value root = (*m_configs)->get_root();

	item_id_list^ idlist	= m_ids;

	project_items_list		all_items;

	for each (System::UInt32^ it in idlist)
		all_items.Add				( project_item_base::object_by_id(*it) );

	project_items_list		items_to_delete;

	extract_topmost_items_int	( %all_items, items_to_delete );

	for each (project_item_base^ itm in items_to_delete)
	{
		R_ASSERT(itm->m_guid!=System::Guid::Empty);
		unmanaged_string sguid(itm->m_guid.ToString());
		root["entrypoint"][sguid.c_str()]		= itm->get_object() ? "object" : "folder";

		itm->save								( root, true, true );
		root["full_path"][sguid.c_str()]		= unmanaged_string(itm->get_full_path()).c_str();
		itm->on_before_manual_delete();
		m_level_editor->get_project()->remove	( itm->get_full_name(), true ); 
	}
	return true;
}

void command_delete_object_impl::rollback( )
{
	configs::lua_config_value value	= (*m_configs)->get_root();

	load_items_list( m_level_editor->get_project(), value, nullptr, nullptr, true, true, true );
}

}// namespace editor
}// namespace xray
