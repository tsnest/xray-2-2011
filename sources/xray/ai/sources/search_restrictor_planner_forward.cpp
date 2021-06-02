////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/search_restrictor_planner_forward.h>

namespace xray {
namespace ai {
namespace search_restrictor {

typedef planner_forward::impl									restrictor;
typedef planning::world_state::properties_type::const_iterator	const_iterator;

bool restrictor::target_reached				( planner_forward::vertex_id_type const& vertex_id )
{
	graph_type& graph						= m_wrapper.graph();
	u32 const offsets_count					= graph.get_offsets_count() + 1;
	if ( offsets_count == 1 )
		return								target_reached_impl(
												vertex_id,
												graph.target().properties().begin(),
												graph.target().properties().end()
											);

	for ( u32 i = 0; i < offsets_count; ++i )
	{
		vertex_id_type const& target		= graph.target( i );
		
		if ( target_reached_impl( vertex_id, target.properties().begin(), target.properties().end() ) )
			return							true;
	}
	return									false;
}

bool restrictor::target_reached_impl(
		planner_forward::vertex_id_type const& vertex_id,
		const_iterator it_target,
		const_iterator it_target_end
	)
{
	graph_type& graph				= m_wrapper.graph();
	const_iterator iter_vertex		= vertex_id.properties().begin();
	const_iterator iter_end_vertex	= vertex_id.properties().end();
	const_iterator iter_current		= graph.current().properties().begin();
	const_iterator iter_end_current	= graph.current().properties().end();
	
	for ( ; ( it_target != it_target_end ) && ( iter_vertex != iter_end_vertex ); )
	{
		if ( (*iter_vertex).id() < (*it_target).id() )
		{
			++iter_vertex;
			continue;
		}

		if ( (*iter_vertex).id() <= (*it_target).id() )
		{
			if ( (*iter_vertex).value() != (*it_target).value() )
				return				false;

			++iter_vertex;
			++it_target;
			continue;
		}

		for ( ; ( iter_current != iter_end_current ) && ( (*iter_current).id() < (*it_target).id() ); )
			++iter_current;

		if ( ( iter_current == iter_end_current ) || ( (*iter_current).id() > (*it_target).id() ) )
			graph.evaluate			( iter_current, iter_end_current, (*it_target).id() );

		if ( (*iter_current).value() != (*it_target).value() )
			return					false;

		++iter_current;
		++it_target;
	}

	if ( iter_vertex != iter_end_vertex )
		return						true;

	iter_vertex						= iter_current;
	iter_end_vertex					= iter_end_current;

	for ( ; it_target != it_target_end; )
	{
		if ( ( iter_vertex == iter_end_vertex ) || ( (*iter_vertex).id() > (*it_target).id() ) )
			graph.evaluate			( iter_vertex, iter_end_vertex, (*it_target).id() );

		if ( (*iter_vertex).id() < (*it_target).id() )
		{
			++iter_vertex;
			continue;
		}

		R_ASSERT					( (*iter_vertex).id() == (*it_target).id(), "error in algorithm" );

		if ( (*iter_vertex).value() != (*it_target).value() )
			return					false;

		++iter_vertex;
		++it_target;
	}

	return							true;
}

} // namespace search_restrictor
} // namespace ai
} // namespace xray

