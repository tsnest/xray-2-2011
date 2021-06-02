////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_H_INCLUDED

#include <xray/ai/search/propositional_planner.h>

namespace xray {
namespace ai {
namespace path_heuristics {

struct propositional_planner_base
{
	struct vertex_impl			{};
	struct look_up_cell_impl	{};

	class impl : private boost::noncopyable
	{
	protected:
		typedef	planning::propositional_planner			graph_type;
		typedef graph_type::vertex_id_type				vertex_id_type;
		typedef graph_type::distance_type				edge_weight_type;
		typedef graph_type::const_edge_iterator			const_edge_iterator;
		typedef graph_type::edge_type					operator_type;
		typedef graph_type::distance_type				edge_value_type;

	public:
		inline						impl				( graph_type& graph );
		
		inline	void				on_before_search	( );
		inline	void				on_after_search		( bool const success ) const;
		inline	void				on_start_iteration	( vertex_id_type const& vertex_id ) const;
		template < typename VertexType >
		inline	edge_weight_type	evaluate			(
										VertexType const& vertex0,
										VertexType const& vertex1,
										const_edge_iterator const& iterator
									) const;
		inline	bool				metric_euclidian	( ) const;

	protected:
		graph_type&					m_graph;
	}; // class impl
}; // struct propositional_planner_base

} // namespace path_heuristics
} // namespace ai
} // namespace xray

#include <xray/ai/search/path_heuristrics_planner_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PATH_HEURISTRICS_PLANNER_BASE_H_INCLUDED