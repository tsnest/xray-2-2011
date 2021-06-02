////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_delete_in_scene_logic_hypergraph.h"
#include "object_scene.h"
#include "logic_editor_panel.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"
namespace xray {
namespace editor {

command_delete_in_scene_logic_hypergraph::command_delete_in_scene_logic_hypergraph( object_scene^  job )
:m_parent_scene( job )
{
	
}

bool command_delete_in_scene_logic_hypergraph::commit							( )
{

	for each( wpf_controls::hypergraph::link^ link in m_parent_scene->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->selected_links )
	{
		if (m_parent_scene->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->links->ContainsValue(link))
			m_parent_scene->remove_link( link );
	}
	for each( wpf_controls::hypergraph::node^ node in m_parent_scene->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->selected_nodes )
	{
		object_scene^ scene  = safe_cast<object_scene^>( node->Tag );
		project^ proj = scene->tool->get_level_editor()->get_project();		
		scene->on_before_manual_delete();
		proj->remove( project_item_base::object_by_id(scene->id())->get_full_name() , true );
	}
 	
	return true;
}
void command_delete_in_scene_logic_hypergraph::rollback							( )
{

}

} // namespace editor
} // namespace xray
