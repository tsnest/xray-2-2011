////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_paste_objects.h"

#include "level_editor.h"
#include "tool_base.h"
#include "object_base.h"
#include "project.h"
#include "command_select.h"
#include "project_items.h"

namespace xray {
namespace editor {

command_paste_objects::command_paste_objects	( level_editor^ le, xray::configs::lua_config_value const& cfg ):
m_level_editor		( le )
{
	m_cfg			= NEW(configs::lua_config_value)(cfg);
}

command_paste_objects::~command_paste_objects()
{
	DELETE (m_cfg);
}

bool command_paste_objects::commit()
{
	m_loaded_names.Clear		( );
	item_id_list				loaded_ids;
	load_items_list				( m_level_editor->get_project(), *m_cfg, %m_loaded_names, %loaded_ids, false, false, false );
	m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project(), %loaded_ids, enum_selection_method_set));
	return						true;
}
void command_paste_objects::rollback()
{
	for each( System::String^ item_name in m_loaded_names )
		m_level_editor->get_project()->remove( item_name, true );
}

}// namespace editor
}// namespace xray
