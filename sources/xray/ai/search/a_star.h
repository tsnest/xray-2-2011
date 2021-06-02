////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_A_STAR_H_INCLUDED
#define XRAY_AI_SEARCH_A_STAR_H_INCLUDED

namespace xray {
namespace ai {

class a_star
{
public:
	template < typename DistanceType >
	struct vertex_helper
	{
		struct vertex_impl
		{
			typedef DistanceType		distance_type;
			distance_type	m_g;
			distance_type	m_h;

			inline distance_type const&	g	( ) const	{ return m_g; }
			inline distance_type&		g	( )			{ return m_g; }
			inline distance_type const& h	( ) const	{ return m_h; }
			inline distance_type&		h	( )			{ return m_h; }
			inline distance_type		f	( ) const	{ return m_g + m_h; }
		};
	};

	struct look_up_cell_impl {};

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
		const typename graph_wrapper_type::const_edge_iterator& i
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

#include <xray/ai/search/a_star_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_A_STAR_H_INCLUDED
