////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_DATA_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_DATA_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _data_type,\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define _graph_vertex gtl::detail::graph_vertex<\
	_data_type,\
	_vertex_id_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
inline	_graph_vertex::graph_vertex				(_data_type const& data, _vertex_id_type const& vertex_id, size_t* edge_count) :
	inherited		(vertex_id,edge_count)
{
	m_data			= data;
}

TEMPLATE_SPECIALIZATION
inline	_graph_vertex::~graph_vertex			()
{
	gtl::destroy	(m_data);
}

TEMPLATE_SPECIALIZATION
inline	bool _graph_vertex::operator==			(graph_vertex const& obj) const
{
	if (!((inherited const&)(*this) == (inherited const&)(obj)))
		return		false;

	return			equal(data(),obj.data());
}

TEMPLATE_SPECIALIZATION
inline	_data_type const& _graph_vertex::data	() const
{
	return			m_data;
}

TEMPLATE_SPECIALIZATION
inline	_data_type& _graph_vertex::data			()
{
	return			m_data;
}

TEMPLATE_SPECIALIZATION
inline	void _graph_vertex::data				(_data_type const& data)
{
	m_data			= data;
}

#undef TEMPLATE_SPECIALIZATION
#undef _graph_vertex

#endif // #ifndef CS_GTL_GRAPH_VERTEX_DATA_INLINE_H_INCLUDED