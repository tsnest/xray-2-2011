////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_link_nodes_action.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_part.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_part_collision.h"
#include "object_patrol_graph_add_edge_command.h"

#include "tool_base.h"
#include "level_editor.h"

#include <xray/editor/base/collision_object_types.h>

#pragma managed ( push, off )
#	include <xray/collision/collision_object.h>
#pragma managed ( push, on )

namespace xray{
namespace editor{


object_patrol_graph_link_nodes_action::object_patrol_graph_link_nodes_action( String^ name, editor_base::scene_view_panel^ view ):
	super			( name ),
	m_window_view	( view )
{}

bool object_patrol_graph_link_nodes_action::do_it ( )
{
	if( !m_window_view->is_mouse_in_view( ) )
		return false;

	if( object_patrol_graph::s_selected_patrol_graphs->Count != 1 )
		return false;

	object_patrol_graph^ patrol_graph = object_patrol_graph::s_selected_patrol_graphs[0];

	if( patrol_graph->selected_graph_part == nullptr )
		return false;

	if( !dot_net::is<object_patrol_graph_node^>( patrol_graph->selected_graph_part ) )
		return false;

	collision::object const* object		= NULL;

	math::float3 pos;

	if( m_window_view->ray_query( editor_base::collision_object_type_touch, &object, &pos ) )
	{
		ASSERT( object->get_type( ) & ( editor_base::collision_object_type_touch ) );

		object_patrol_graph_part_collision const* collision = static_cast<object_patrol_graph_part_collision const*>( object );
		if( dot_net::is<object_patrol_graph_node^>( collision->get_owner( ) ) )
		{
			node^ owner_node = safe_cast<node^>( collision->get_owner( ) );

			if( owner_node->get_parent_graph( ) != patrol_graph )
				return false;

			if( owner_node == patrol_graph->selected_graph_part )
				return false;

			patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( 
				gcnew object_patrol_graph_add_edge_command( 
					patrol_graph, 
					safe_cast<node^>( patrol_graph->selected_graph_part ),
					owner_node
				)
			);
			//m_object_patrol_graph->create_edge_to_node( node )->select( );
			
			return true;
		}

		return false;
	}
	else
		return false;
}

} // namespace editor
} // namespace xray