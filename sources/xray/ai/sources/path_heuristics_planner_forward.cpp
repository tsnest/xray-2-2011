////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/path_heuristics_planner_forward.h>

namespace xray {
namespace ai {
namespace path_heuristics {

typedef planner_forward::impl									heuristics;
typedef planning::world_state::properties_type::const_iterator	const_iterator;

heuristics::edge_weight_type heuristics::estimate	( vertex_id_type const* current_vertex_id_ptr, vertex_id_type const& neighbour_vertex_id ) const
{
	XRAY_UNREFERENCED_PARAMETER				( current_vertex_id_ptr );
	u32 const offsets_count					= m_graph.get_offsets_count() + 1;
	if ( offsets_count == 1 )
		return								estimate_impl(
												neighbour_vertex_id,
												m_graph.target().properties().begin(),
												m_graph.target().properties().end()
											);

	edge_value_type min_result				= u32(-1);
	for ( u32 i = 0; i < offsets_count; ++i )
	{
		vertex_id_type const& target		= m_graph.target( i );
		edge_value_type const estimation	= estimate_impl(
												neighbour_vertex_id,
												target.properties().begin(),
												target.properties().end()
											);
		min_result							= ( min_result == u32(-1) || min_result > estimation ) ?
											estimation : min_result;
	}
	R_ASSERT								( min_result != u32(-1), "error in algorithm" );
	return									min_result;
}

heuristics::edge_weight_type heuristics::estimate_impl	(
		vertex_id_type const& vertex_id,
		const_iterator it_target_begin,
		const_iterator it_target_end
	) const
{
	edge_value_type result				= 0;
	const_iterator iter_vertex			= vertex_id.properties().begin();
	const_iterator iter_end_vertex		= vertex_id.properties().end();
	const_iterator iter_current			= m_graph.current().properties().begin();
	const_iterator iter_end_current		= m_graph.current().properties().end();

	for ( ; ( it_target_begin != it_target_end ) && ( iter_vertex != iter_end_vertex ); )
	{
		if ( (*iter_vertex).id() < (*it_target_begin).id() )
		{
			++iter_vertex;
			continue;
		}

		if ( (*iter_vertex).id() == (*it_target_begin).id() )
		{
			if ( (*iter_vertex).value() != (*it_target_begin).value() )
				++result;

			++iter_vertex;
			++it_target_begin;
			continue;
		}

		for ( ; ( iter_current != iter_end_current ) && ( (*iter_current).id() < (*it_target_begin).id() ); )
			++iter_current;
		
		if ( ( iter_current == iter_end_current ) || ( (*iter_current).id() > (*it_target_begin).id() ) )
			m_graph.evaluate			( iter_current, iter_end_current, (*it_target_begin).id() );
		
		if ( (*iter_current).value() != (*it_target_begin).value() )
			++result;

		++iter_current;
		++it_target_begin;
	}

	if ( iter_vertex != iter_end_vertex )
		return					result;

	iter_vertex					= iter_current;
	iter_end_vertex				= iter_end_current;

	for ( ; it_target_begin != it_target_end; )
	{
		if ( ( iter_vertex == iter_end_vertex ) || ( (*iter_vertex).id() > (*it_target_begin).id() ) )
			m_graph.evaluate	( iter_vertex, iter_end_vertex, (*it_target_begin).id() );

		if ( (*iter_vertex).id() < (*it_target_begin).id() )
		{
			++iter_vertex;
			continue;
		}

		R_ASSERT				( (*iter_vertex).id() == (*it_target_begin).id(), "error in algorithm" );

		if ( (*iter_vertex).value() != (*it_target_begin).value() )
			++result;

		++iter_vertex;
		++it_target_begin;
	}

	return						result;
}

} // namespace path_heuristics
} // namespace ai
} // namespace xray