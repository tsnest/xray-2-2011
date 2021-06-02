////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/search_restrictor_planner_backward.h>

namespace xray {
namespace ai {
namespace search_restrictor {

typedef planner_backward::impl										restrictor;
typedef planning::world_state::properties_type::const_iterator		const_iterator;

bool restrictor::target_reached		( planner_backward::vertex_id_type const& vertex_id )
{
	graph_type& graph				= m_wrapper.graph();
	const_iterator iter_graph		= graph.current().properties().begin();
	const_iterator iter_end_graph	= graph.current().properties().end();
	const_iterator iter_vertex		= vertex_id.properties().begin();
	const_iterator iter_end_vertex	= vertex_id.properties().end();
	
	for ( ; iter_vertex != iter_end_vertex; )
	{
		if ( ( iter_graph == iter_end_graph ) || ( (*iter_graph).id() > (*iter_vertex).id() ) )
			graph.evaluate	( iter_graph, iter_end_graph, (*iter_vertex).id() );

		if ( (*iter_graph).id() < (*iter_vertex).id() )
		{
			++iter_graph;
			continue;
		}

		if ( (*iter_graph).value() != (*iter_vertex).value() )
			return					false;

		++iter_graph;
		++iter_vertex;
	}

	return							true;
}

} // namespace search_restrictor
} // namespace ai
} // namespace xray