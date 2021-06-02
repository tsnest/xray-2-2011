////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_action.h"
#include "object_patrol_graph.h"

namespace xray{
namespace editor{

object_patrol_graph_action::object_patrol_graph_action ( String^ name ):super( name )
{
}

bool object_patrol_graph_action::enabled ( )
{
	return object_patrol_graph::is_in_edit_mode( );
}

} // namespace editor
} // namespace xray