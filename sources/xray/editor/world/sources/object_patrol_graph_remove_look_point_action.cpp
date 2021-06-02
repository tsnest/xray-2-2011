////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_remove_look_point_action.h"

#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_look_point.h"

#include "object_patrol_graph_add_look_point_command.h"

#include "tool_base.h"
#include "level_editor.h"

namespace xray{
namespace editor{

object_patrol_graph_remove_look_point_action::object_patrol_graph_remove_look_point_action ( String^ name, editor_base::scene_view_panel^ view ) :
	super			( name ),
	m_window_view	( view )
{	
}

bool object_patrol_graph_remove_look_point_action::do_it ( )
{
	if( !m_window_view->is_mouse_in_view( ) )
		return false;

	if( object_patrol_graph::s_selected_patrol_graphs->Count != 1 )
		return false;

	object_patrol_graph^ patrol_graph = object_patrol_graph::s_selected_patrol_graphs[0];

	if( patrol_graph->selected_graph_part == nullptr || !dot_net::is<object_patrol_graph_node^>( patrol_graph->selected_graph_part ) )
		return false;

	collision::object const* object		= NULL;

	math::float3 pos;

	if( m_window_view->ray_query( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain, &object, &pos ) )
	{
		ASSERT( object->get_type( ) & ( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain ) );

		pos.y += 0.9f;
		patrol_graph->owner_tool( )->get_level_editor( )->get_command_engine( )->run( gcnew object_patrol_graph_add_look_point_command( patrol_graph, safe_cast<object_patrol_graph_node^>( patrol_graph->selected_graph_part ), Float3( pos ) ) );

		return true;
	}
	else
		return false;
}
