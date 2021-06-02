////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_edge_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph edge class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_EDGE_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_EDGE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _edge_weight_type,\
	typename _graph_type\
>

#define _graph_edge gtl::detail::graph_edge<\
	_edge_weight_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
inline	_graph_edge::graph_edge			(_edge_weight_type const& weight, _vertex_type* vertex)
{
	m_weight		= weight;
	ASSERT			(vertex);
	m_vertex		= vertex;
}

TEMPLATE_SPECIALIZATION
inline	typename _graph_edge::_edge_weight_type const& _graph_edge::weight	() const
{
	return			m_weight;
}

TEMPLATE_SPECIALIZATION
inline	typename _graph_edge::_vertex_type* _graph_edge::vertex				() const
{
	return			m_vertex;
}

TEMPLATE_SPECIALIZATION
inline	bool _graph_edge::operator==	(_vertex_id_type const& vertex_id) const
{
	return			vertex()->vertex_id() == vertex_id;
}

TEMPLATE_SPECIALIZATION
inline	bool _graph_edge::operator==	(graph_edge const& obj) const
{
	if (weight() != obj.weight())
		return		false;

	return			vertex()->vertex_id() == obj.vertex()->vertex_id();
}

#undef TEMPLATE_SPECIALIZATION
#undef _graph_edge

#endif // #ifndef CS_GTL_GRAPH_EDGE_INLINE_H_INCLUDED