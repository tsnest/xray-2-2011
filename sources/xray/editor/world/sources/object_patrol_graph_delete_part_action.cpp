////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_delete_part_action.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_part.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_look_point.h"
#include "object_patrol_graph_remove_node_command.h"
#include "object_patrol_graph_remove_edge_command.h"
#include "object_patrol_graph_remove_look_point_command.h"

#include "tool_base.h"
#include "level_editor.h"

#include <xray/editor/base/collision_object_types.h>

namespace xray{
namespace editor{

object_patrol_graph_delete_part_action::object_patrol_graph_delete_part_action( String^ name, editor_base::scene_view_panel^ view ):
	super			( name ),
	m_window_view	( view )
{}

bool			object_patrol_graph_delete_part_action::do_it						( )
{
	if( object_patrol_graph::s_selected_patrol_graphs->Count != 1 )
		return false;

	object_patrol_graph^ patrol_graph = object_patrol_graph::s_selected_patrol_graphs[0];

	object_patrol_graph_part^ part = patrol_graph->selected_graph_part;

	if( part == nullptr )
		return false;

	if( dot_net::is<node^>( part ) )
		patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( gcnew object_patrol_graph_remove_node_command( patrol_graph ) ); 

	else if( dot_net::is<edge^>( part ) )
		patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( gcnew object_patrol_graph_remove_edge_command( patrol_graph, safe_cast<edge^>( part ) ) ); 

	else
		patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( gcnew object_patrol_graph_remove_look_point_command( patrol_graph, safe_cast<look_point^>( part ) ) ); 

	return true;
}

} // namespace editor
} // namespace xray