////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_part.h"
#include "object_patrol_graph_part_collision.h"

#include <xray/collision/geometry_instance.h>
#include <xray/collision/space_partitioning_tree.h>

namespace xray{
namespace editor{

object_patrol_graph_part::object_patrol_graph_part ( object_patrol_graph^ graph, collision::geometry_instance& geometry )
{
	m_parent_graph			= graph;
	m_collision				= NEW( object_patrol_graph_part_collision )( &geometry, this );
}

object_patrol_graph_part::~object_patrol_graph_part( )
{
	m_collision->destroy	( g_allocator );
	DELETE					( m_collision );
	m_parent_graph			= nullptr;
}

object_patrol_graph^ object_patrol_graph_part::get_parent_graph	( )
{
	return m_parent_graph;
}

void object_patrol_graph_part::set_parent_graph	( object_patrol_graph^ graph )
{
	m_parent_graph = graph;
}

aabb object_patrol_graph_part::get_aabb	( )
{
	return m_collision->get_aabb( );
}

part_collision* object_patrol_graph_part::get_collision ( )
{
	return m_collision;
}

void object_patrol_graph_part::attach_collision	( )
{
	m_parent_graph->m_collision->m_collision_tree->insert	( m_collision, m_collision->get_matrix( ) );
}

void object_patrol_graph_part::detach_collision	( )
{
	m_parent_graph->m_collision->m_collision_tree->erase	( m_collision );
}

} // namespace editor
} // namespace xray