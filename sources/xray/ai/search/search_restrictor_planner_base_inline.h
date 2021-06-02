////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace search_restrictor {

#define RESTRICTOR			search_restrictor::propositional_planner_base::impl

inline RESTRICTOR::impl		(
		graph_wrapper_type const& wrapper,
		vertex_id_type const& start_vertex_id,
		vertex_id_type const& target_vertex_id,
		distance_type const& max_range,
		u32 const max_iteration_count,
		u32 const max_visited_vertex_count
	) :
	m_wrapper				( wrapper ),
	super					(
		start_vertex_id,
		target_vertex_id,
		max_range,
		max_iteration_count,
		max_visited_vertex_count
	)
{
}

template < typename VertexType >
inline bool RESTRICTOR::accessible	( vertex_id_type const& neighbour_vertex_id, VertexType const& current_vertex, graph_type::const_edge_iterator const& edge_iterator ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( &neighbour_vertex_id, &current_vertex, &edge_iterator );
	return							m_wrapper.applied();
}

#undef RESTRICTOR

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BASE_INLINE_H_INCLUDED