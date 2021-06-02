////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_DIJKSTRA_H_INCLUDED
#define XRAY_AI_SEARCH_DIJKSTRA_H_INCLUDED

namespace xray {
namespace ai {

class dijkstra
{
public:
	template < typename distance_type >
	struct vertex_helper
	{
		struct vertex_impl
		{
			typedef distance_type		distance_type;
			distance_type	m_f;

			inline distance_type&		f		( )			{ return m_f; }
			inline distance_type const&	f		( ) const	{ return m_f; }
		};
	};

public:
	struct look_up_cell_impl	{};

private:
	template <
		typename priority_queue_type,
		typename graph_wrapper_type,
		typename path_constructor_type,
		typename path_heuristics_type,
		typename search_restrictor_type
	>
	static inline void	on_before_search	(
		priority_queue_type& queue,
		graph_wrapper_type& graph,
		path_constructor_type& path,
		path_heuristics_type& heuristics,
		search_restrictor_type& restrictor
	);
	
	template <
		typename priority_queue_type,
		typename graph_wrapper_type,
		typename path_constructor_type,
		typename path_heuristics_type,
		typename search_restrictor_type
	>
	static inline void	on_after_search		(
		priority_queue_type& queue,
		graph_wrapper_type& graph,
		path_constructor_type& path,
		path_heuristics_type& heuristics,
		search_restrictor_type& restrictor,
		bool const success
	);
	
	template <
		typename priority_queue_type,
		typename graph_wrapper_type,
		typename path_constructor_type,
		typename path_heuristics_type,
		typename search_restrictor_type
	>
	static inline void	process_neighbour	(
		priority_queue_type& queue,
		graph_wrapper_type& graph,
		path_constructor_type& path,
		path_heuristics_type& heuristics,
		search_restrictor_type& restrictor,
		typename priority_queue_type::vertex_type& best,
		typename graph_wrapper_type::const_edge_iterator const& i
	);
	
	template <
		typename priority_queue_type,
		typename graph_wrapper_type,
		typename path_constructor_type,
		typename path_heuristics_type,
		typename search_restrictor_type
	>
	static inline bool	step				(
		priority_queue_type& queue,
		graph_wrapper_type& graph,
		path_constructor_type& path,
		path_heuristics_type& heuristics,
		search_restrictor_type& restrictor
	);

public:
	template <
		typename priority_queue_type,
		typename graph_wrapper_type,
		typename path_constructor_type,
		typename path_heuristics_type,
		typename search_restrictor_type
	>
	static inline bool	find				(
		priority_queue_type& queue,
		graph_wrapper_type& graph,
		path_constructor_type& path,
		path_heuristics_type& heuristics,
		search_restrictor_type& restrictor
	);
};

} // namespace ai
} // namespace xray

#include <xray/ai/search/dijkstra_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_DIJKSTRA_H_INCLUDED