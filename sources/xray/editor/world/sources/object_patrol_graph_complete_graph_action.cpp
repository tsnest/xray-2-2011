////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_complete_graph_action.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_part.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_add_node_command.h"

#include "tool_base.h"
#include "level_editor.h"

#include <xray/editor/base/collision_object_types.h>
#include "collision_object_dynamic.h"

namespace xray{
namespace editor{

object_patrol_graph_complete_graph_action::object_patrol_graph_complete_graph_action( String^ name, editor_base::scene_view_panel^ view )
:super			( name ),
m_window_view	( view )
{}

bool			object_patrol_graph_complete_graph_action::do_it						( )
{
	if( !m_window_view->is_mouse_in_view( ) )
		return false;

	if( object_patrol_graph::s_selected_patrol_graphs->Count != 1 )
		return false;

	object_patrol_graph^ patrol_graph = object_patrol_graph::s_selected_patrol_graphs[0];

	collision::object const* object		= NULL;

	math::float3 pos;

	if( m_window_view->ray_query( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain, &object, &pos ) )
	{
		ASSERT( object->get_type( ) & ( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain ) );

		pos.y += 0.9f;

		if(		patrol_graph->selected_graph_part != nullptr && 
				dot_net::is<node^>( patrol_graph->selected_graph_part ) && 
				math::is_zero( ( safe_cast<node^>( patrol_graph->selected_graph_part )->position - pos ).length( ) )	)

				return false;

		patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( gcnew object_patrol_graph_add_node_command( patrol_graph, Float3( pos ) ) );

		return true;
	}
	else
		return false;
}

} // namespace editor
} // namespace xray