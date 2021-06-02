////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_DIJKSTRA_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_DIJKSTRA_INLINE_H_INCLUDED

namespace xray {
namespace ai {

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename priority_queue_type,\
		typename graph_wrapper_type,\
		typename path_constructor_type,\
		typename path_heuristics_type,\
		typename search_restrictor_type\
	>

TEMPLATE_SPECIALIZATION
inline void dijkstra::on_before_search	(
		priority_queue_type&		queue,
		graph_wrapper_type&			graph,
		path_constructor_type&		path,
		path_heuristics_type&		heuristics,
		search_restrictor_type&		restrictor
	)
{
	typedef typename priority_queue_type::vertex_type	vertex_type;
	typedef typename vertex_type::vertex_id_type		vertex_id_type;
	typedef typename vertex_type::distance_type			distance_type;

	queue.on_before_search			( );
	graph.on_before_search			( );
	path.on_before_search			( );
	heuristics.on_before_search		( );
	restrictor.on_before_search		( );

// 	vertex_id_type const& start_vertex_id	= restrictor.start_vertex_id();
// 	vertex_type& start						= queue.new_vertex( start_vertex_id );
// 
// 	start.f()						= distance_type( 0 );
// 
// 	path.assign_parent				( start, 0 );
// 	queue.add_to_opened_list		( start );

	u32 const vertices_count		= restrictor.get_start_vertices_count();
	for ( u32 i = 0; i < vertices_count; ++i )
	{
		vertex_id_type const& start_vertex_id	= restrictor.start_vertex_id( i );
		vertex_type& start						= queue.new_vertex( start_vertex_id );

		start.f()					= distance_type( 0 );

		path.assign_parent			( start, 0 );
		queue.add_to_opened_list	( start );
	}
}

TEMPLATE_SPECIALIZATION
inline void dijkstra::on_after_search	(
		priority_queue_type&		queue,
		graph_wrapper_type&			graph,
		path_constructor_type&		path,
		path_heuristics_type&		heuristics,
		search_restrictor_type&		restrictor,
		bool const					success
	)
{
	queue.on_after_search			( success );
	graph.on_after_search			( success );
	path.on_after_search			( success );
	heuristics.on_after_search		( success );
	restrictor.on_after_search		( success );
}

TEMPLATE_SPECIALIZATION
inline void dijkstra::process_neighbour	(
		priority_queue_type&		queue,
		graph_wrapper_type&			graph,
		path_constructor_type&		path,
		path_heuristics_type&		heuristics,
		search_restrictor_type&		restrictor,
		typename priority_queue_type::vertex_type& best,
		const typename graph_wrapper_type::const_edge_iterator& i
	)
{
	typedef typename priority_queue_type::vertex_type	vertex_type;
	typedef typename vertex_type::vertex_id_type		vertex_id_type;
	typedef typename vertex_type::distance_type			distance_type;

	vertex_id_type const& neighbour_id	= graph.vertex_id(best.id(), i);
	if ( !restrictor.accessible( neighbour_id, best, i ) )
		return;

	if ( !queue.visited( neighbour_id ) )
	{
		vertex_type& neighbour			= queue.new_vertex( neighbour_id );
		neighbour.f()					= best.f() + heuristics.evaluate( best, neighbour, i );
		path.assign_parent				( neighbour, &best, graph.edge_id( best.id(), i ) );
		queue.add_to_opened_list		( neighbour );
		return;
	}

	vertex_type& neighbour				= queue.vertex( neighbour_id );
	if ( !queue.opened( neighbour ) )
		return;

	distance_type const& f				= best.f() + heuristics.evaluate( best, neighbour, i );
	if ( neighbour.f() <= f )
		return;

	distance_type distance				= neighbour.f();
	neighbour.f()						= f;
	path.assign_parent					( neighbour, &best, graph.edge_id( best.id(), i ) );
	queue.decrease_opened				( neighbour, distance );
}

TEMPLATE_SPECIALIZATION
inline bool dijkstra::step			(
		priority_queue_type&		queue,
		graph_wrapper_type&			graph,
		path_constructor_type&		path,
		path_heuristics_type&		heuristics,
		search_restrictor_type&		restrictor
	)
{
	typedef typename priority_queue_type::vertex_type	vertex_type;
	typedef graph_wrapper_type::const_edge_iterator		const_edge_iterator;

	vertex_type& best				= queue.best();

	if ( restrictor.target_reached( best.id() ) )
	{
		path.construct_path			( best );
		return						true;
	}

	queue.move_best_to_closed		( );

	const_edge_iterator				i, e;
	graph.edge_iterators			( best, i, e );
	heuristics.on_start_iteration	( best.id() );
	for ( ; i != e; ++i )
		process_neighbour			( queue, graph, path, heuristics, restrictor, best, i );

	return							false;
}

TEMPLATE_SPECIALIZATION
inline bool dijkstra::find			(
		priority_queue_type&		queue,
		graph_wrapper_type&			graph,
		path_constructor_type&		path,
		path_heuristics_type&		heuristics,
		search_restrictor_type&		restrictor
	)
{
	on_before_search				( queue, graph, path, heuristics, restrictor );

	for ( u32 i = 0; !queue.opened_list_empty(); ++i )
	{
		if ( restrictor.limit_reached( queue, i ) )
		{
			on_after_search			( queue, graph, path, heuristics, restrictor, false );
			return					false;
		}
		
		if ( !step( queue, graph, path, heuristics, restrictor ) )
			continue;

		on_after_search				( queue, graph, path, heuristics, restrictor, true );
		return						true;
	}

	on_after_search					( queue, graph, path, heuristics, restrictor, false );
	return							false;
}

#undef TEMPLATE_SPECIALIZATION

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_DIJKSTRA_INLINE_H_INCLUDED
