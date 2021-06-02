////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_heuristics {

#define HEURISTICS	path_heuristics::propositional_planner_base::impl

inline HEURISTICS::impl	( graph_type& graph ) :
	m_graph				( graph )
{
}

inline void HEURISTICS::on_before_search	( )
{
}

inline void HEURISTICS::on_after_search		( bool const success ) const
{
	XRAY_UNREFERENCED_PARAMETER				( success );
}

inline void HEURISTICS::on_start_iteration	( vertex_id_type const& vertex_id ) const
{
	XRAY_UNREFERENCED_PARAMETER				( vertex_id );
}

template < typename VertexType >
inline HEURISTICS::edge_weight_type HEURISTICS::evaluate	(
		VertexType const& vertex0,
		VertexType const& vertex1,
		const_edge_iterator const& iterator
	) const
{
	operator_type& i						= *(*iterator).get_operator();
	edge_value_type result					= i.weight( vertex1.id(), vertex0.id() );

#ifdef DEBUG
	edge_value_type	min_weight				= i.min_weight();
	R_ASSERT								( result >= min_weight, "minimum weight rule is broken" );
#endif // #ifdef DEBUG

	return									vertex0.g() + result;
}

inline bool HEURISTICS::metric_euclidian	( ) const
{
	return									true;
}

#undef HEURISTICS

} // namespace search
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_INLINE_H_INCLUDED