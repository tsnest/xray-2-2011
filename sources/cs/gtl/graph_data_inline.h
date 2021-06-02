////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_DATA_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_DATA_INLINE_H_INCLUDED

#include <cs/gtl/graph_base.h>

#define TEMPLATE_SPECIALIZATION template <\
	typename _data_type,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	template <\
		typename,\
		typename,\
		typename\
	> class _graph_vertex_type,\
	template <\
		typename,\
		typename\
	> class _graph_edge_type\
>

#define graph gtl::detail::_graph<\
	_data_type,\
	_edge_weight_type,\
	_vertex_id_type,\
	_graph_vertex_type,\
	_graph_edge_type\
>

TEMPLATE_SPECIALIZATION
inline	void graph::add_vertex	(_data_type& data, _vertex_id_type const& vertex_id)
{
	inherited::add_vertex	(vertex_id,create_vertex(vertex_id,data));
}

#undef TEMPLATE_SPECIALIZATION
#undef graph

#endif // #ifndef CS_GTL_GRAPH_DATA_INLINE_H_INCLUDED