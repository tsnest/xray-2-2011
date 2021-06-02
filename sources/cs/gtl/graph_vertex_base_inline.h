////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_base_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex base class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_BASE_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_BASE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define graph_vertex gtl::detail::graph_vertex_base<\
	_vertex_id_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
inline	graph_vertex::graph_vertex_base					(_vertex_id_type const& vertex_id, size_t* edge_count)
{
	m_vertex_id				= vertex_id;
	ASSERT					(edge_count);
	m_edge_count			= edge_count;
}

TEMPLATE_SPECIALIZATION
inline	graph_vertex::~graph_vertex_base					()
{
	while (!edges().empty())
		remove_edge			(edges().back().vertex()->vertex_id());

	while (!m_vertices.empty())
		m_vertices.back()->remove_edge(vertex_id());
}

TEMPLATE_SPECIALIZATION
inline	const typename graph_vertex::_edge_type* graph_vertex::edge	(_vertex_id_type const& vertex_id) const
{
	EDGES::const_iterator	I = std::find(edges().begin(),edges().end(),vertex_id);
	if (m_edges.end() == I)
		return				0;
	return					&*I;
}

TEMPLATE_SPECIALIZATION
inline	typename graph_vertex::_edge_type* graph_vertex::edge			(_vertex_id_type const& vertex_id)
{
	EDGES::iterator			I = std::find(m_edges.begin(),m_edges.end(),vertex_id);
	if (m_edges.end() == I)
		return				0;
	return					&*I;
}

TEMPLATE_SPECIALIZATION
inline	void graph_vertex::add_edge						(_vertex_type* vertex, _edge_weight_type const& edge_weight)
{
#ifndef NDEBUG
	EDGES::iterator			I = 
#endif // #ifndef NDEBUG
		std::find(m_edges.begin(),m_edges.end(),vertex->vertex_id());
	ASSERT					(m_edges.end() == I);
	vertex->on_edge_addition((_vertex_type*)this);
	m_edges.push_back		(_edge_type(edge_weight,vertex));
	++*m_edge_count;
}

TEMPLATE_SPECIALIZATION
inline	void graph_vertex::remove_edge						(_vertex_id_type const& vertex_id)
{
	EDGES::iterator			I = std::find(m_edges.begin(),m_edges.end(),vertex_id);
	ASSERT					(m_edges.end() != I, "can't find edge to the specified vertex id");
	_vertex_type*			vertex = (*I).vertex();
	vertex->on_edge_removal	((_vertex_type*)this);
	m_edges.erase			(I);
	--*m_edge_count;
}

TEMPLATE_SPECIALIZATION
inline	void graph_vertex::on_edge_addition				(_vertex_type* vertex)
{
#ifndef NDEBUG
	VERTICES::const_iterator	I = 
#endif // #ifndef NDEBUG
		std::find(m_vertices.begin(),m_vertices.end(),vertex);
	ASSERT						(I == m_vertices.end());
	m_vertices.push_back		(vertex);
}

TEMPLATE_SPECIALIZATION
inline	void graph_vertex::on_edge_removal					(_vertex_type const* vertex)
{
	VERTICES::iterator			I = std::find(m_vertices.begin(),m_vertices.end(),vertex);
	ASSERT						(I != m_vertices.end(), "can't find specified vertex");
	m_vertices.erase			(I);
}

TEMPLATE_SPECIALIZATION
inline	_vertex_id_type const& graph_vertex::vertex_id		() const
{
	return			m_vertex_id;
}

TEMPLATE_SPECIALIZATION
inline	typename graph_vertex::EDGES const& graph_vertex::edges	() const
{
	return			m_edges;
}

TEMPLATE_SPECIALIZATION
inline	bool graph_vertex::operator==	(graph_vertex_base const& obj) const
{
	if (vertex_id() != obj.vertex_id())
		return		false;

	return			equal(edges(),obj.edges());
}

#undef TEMPLATE_SPECIALIZATION
#undef graph_vertex

#endif // #ifndef CS_GTL_GRAPH_VERTEX_BASE_INLINE_H_INCLUDED