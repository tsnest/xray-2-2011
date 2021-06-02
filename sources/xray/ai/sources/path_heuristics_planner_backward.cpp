////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/path_heuristics_planner_backward.h>

namespace xray {
namespace ai {
namespace path_heuristics {

typedef planner_backward::impl									heuristics;
typedef planning::world_state::properties_type::const_iterator	const_iterator;

heuristics::edge_weight_type heuristics::estimate	( vertex_id_type const* current_vertex_id_ptr, vertex_id_type const& neighbour_vertex_id ) const
{
	XRAY_UNREFERENCED_PARAMETER			( current_vertex_id_ptr );

	edge_value_type result				= 0;
	const_iterator iter_graph			= m_graph.current().properties().begin();
	const_iterator iter_end_graph		= m_graph.current().properties().end();
	const_iterator iter_vertex			= neighbour_vertex_id.properties().begin();
	const_iterator iter_end_vertex		= neighbour_vertex_id.properties().end();

	for ( ; ( iter_vertex != iter_end_vertex ); )
	{
		if ( ( iter_graph == iter_end_graph ) || ( (*iter_graph).id() > (*iter_vertex).id() ) ) {
			m_graph.evaluate			( iter_graph, iter_end_graph, (*iter_vertex).id() );
			R_ASSERT					( (*iter_graph).id() == (*iter_vertex).id(), "error in algorithm" );
		}

 		if ( (*iter_graph).id() < (*iter_vertex).id() )
 		{
 			++iter_graph;
 			continue;
 		}

		R_ASSERT						( (*iter_graph).id() == (*iter_vertex).id(), "error in algorithm" );
		if ( (*iter_graph).value() != (*iter_vertex).value() )
			++result;

		++iter_graph;
		++iter_vertex;
	}
	return								result;
}

} // namespace path_heuristics
} // namespace ai
} // namespace xray