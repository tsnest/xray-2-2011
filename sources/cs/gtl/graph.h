////////////////////////////////////////////////////////////////////////////
//	Module 		: graph.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_H_INCLUDED
#define CS_GTL_GRAPH_H_INCLUDED

#include <cs/gtl/graph_data.h>
#include <cs/gtl/graph_no_data.h>
#include <cs/gtl/graph_vertex.h>
#include <cs/gtl/graph_edge.h>

namespace gtl {

template <
	typename _data_type = Loki::EmptyType,
	typename _edge_weight_type = float,
	typename _vertex_id_type = u32,
	template <
		typename,
		typename,
		typename
	> class _graph_vertex_type = detail::graph_vertex,
	template <
		typename,
		typename
	> class _graph_edge_type = detail::graph_edge
>
struct graph : 
	public detail::_graph<
		_data_type,
		_edge_weight_type,
		_vertex_id_type,
		_graph_vertex_type,
		_graph_edge_type
	>
{
	typedef _vertex_id_type		vertex_id_type;
	typedef _edge_weight_type	edge_weight_type;
};

};

#endif // #ifndef CS_GTL_GRAPH_H_INCLUDED