////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_no_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_NO_DATA_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_NO_DATA_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define _graph_vertex gtl::detail::graph_vertex<\
	Loki::EmptyType,\
	_vertex_id_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
inline	_graph_vertex::graph_vertex	(_vertex_id_type const& vertex_id, size_t* edge_count) :
	inherited	(vertex_id,edge_count)
{
}
	
#undef TEMPLATE_SPECIALIZATION
#undef _graph_vertex

#endif // #ifndef CS_GTL_GRAPH_VERTEX_NO_DATA_INLINE_H_INCLUDED