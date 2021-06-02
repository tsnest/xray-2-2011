////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_part_collision.h"
#include "object_patrol_graph_part.h"
#include <xray/editor/base/collision_object_types.h>

namespace xray{
namespace editor	{

object_patrol_graph_part_collision::object_patrol_graph_part_collision ( collision::geometry_instance* geometry, object_patrol_graph_part^ obj ):
	super		( g_allocator, xray::editor_base::collision_object_type_touch, geometry ),
	m_owner		( obj )
{
}

bool object_patrol_graph_part_collision::touch ( ) const
{
	m_owner->select( );

	return true;
}

} // namespace editor
} // namespace xray
