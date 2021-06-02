////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_H_INCLUDED

#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/graph_wrapper_planner_base.h>
#include <xray/ai/search/search_restrictor_generic.h>

namespace xray {
namespace ai {
namespace search_restrictor {

struct propositional_planner_base
{
	typedef planning::propositional_planner	graph_type;
	typedef graph_type::vertex_id_type		vertex_id_type;
	typedef graph_type::distance_type		distance_type;

	class impl : public generic< vertex_id_type, graph_type::const_edge_iterator, distance_type >
	{
	private:
		typedef generic< vertex_id_type, graph_type::const_edge_iterator, distance_type >	super;

	protected:
		typedef graph_wrapper::propositional_planner_base::impl graph_wrapper_type;

	public:
		inline		impl			(
						graph_wrapper_type const& wrapper,
						vertex_id_type const& start_vertex_id,
						vertex_id_type const& target_vertex_id,
						distance_type const& max_range = std::numeric_limits< distance_type >::max(),
						u32 const max_iteration_count = u32(-1),
						u32 const max_visited_vertex_count = u32(-1)
					);
		template < typename VertexType >
		inline bool	accessible		( vertex_id_type const& neighbour_vertex_id, VertexType const& current_vertex, graph_type::const_edge_iterator const& edge_iterator ) const;

	protected:
		graph_wrapper_type const&	m_wrapper;
	}; // class impl
}; // struct propositional_planner_base

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_restrictor_planner_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_H_INCLUDED
