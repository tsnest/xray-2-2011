////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_H_INCLUDED

#include <xray/ai/search/propositional_planner.h>

namespace xray {
namespace ai {
namespace graph_wrapper {

struct propositional_planner_base
{
	struct vertex_impl				{};
	struct look_up_cell_impl		{};

	class impl : private boost::noncopyable
	{
	protected:
		typedef planning::propositional_planner			graph_type;

	public:
		typedef graph_type::vertex_id_type				vertex_id_type;
		typedef graph_type::edge_id_type				edge_id_type;
		typedef graph_type::edge_type					edge_type;
		typedef graph_type::const_edge_iterator			const_edge_iterator;

	public:
		inline						impl				( graph_type* graph = 0 );
		
		inline void					on_before_search	( );
		inline void					on_after_search		( bool const success ) const;
		template < typename VertexType >
		inline void					edge_iterators		( VertexType const& vertex, const_edge_iterator& begin, const_edge_iterator& end );
		inline edge_id_type			edge_id				( vertex_id_type const& id, const_edge_iterator const& iterator ) const;
		inline bool					applied				( ) const;
		
		inline void					graph				( graph_type& graph );
		inline graph_type&			graph				( ) const;

	protected:
		vertex_id_type const*		m_current_state;
		vertex_id_type				m_new_state;
		bool						m_applied;

	private:
		graph_type*					m_graph;
		const_edge_iterator			m_begin;
		const_edge_iterator			m_end;
	};
}; // struct propositional_planner_base

} // namespace graph_wrapper
} // namespace ai
} // namespace xray

#include <xray/ai/search/graph_wrapper_planner_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_H_INCLUDED