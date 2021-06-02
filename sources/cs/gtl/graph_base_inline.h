////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_base_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph base class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_BASE_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_BASE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _graph_vertex_type,\
	typename _graph_edge_type,\
	typename inherited\
>

#define graph gtl::detail::graph_base<\
	_edge_weight_type,\
	_vertex_id_type,\
	_graph_vertex_type,\
	_graph_edge_type,\
	inherited\
>

TEMPLATE_SPECIALIZATION
inline	graph::graph_base			()
{
	m_edge_count				= 0;
}

TEMPLATE_SPECIALIZATION
inline	graph::~graph_base			()
{
	clear						();
}

TEMPLATE_SPECIALIZATION
inline	void graph::add_vertex		(_vertex_id_type const& vertex_id, vertex_type* new_vertex)
{
	ASSERT						(!vertex(vertex_id));
	m_vertices.insert			(std::make_pair(vertex_id,new_vertex));
}

TEMPLATE_SPECIALIZATION
inline	void graph::remove_vertex	(_vertex_id_type const& vertex_id)
{
	vertex_iterator				I = m_vertices.find(vertex_id);
	ASSERT						(m_vertices.end() != I, "can't find vertex to remove");
	VERTICES::value_type		v = *I;
	gtl::destroy				(v);
	m_vertices.erase			(I);
}

TEMPLATE_SPECIALIZATION
inline	void graph::add_edge		(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1, _edge_weight_type const& edge_weight)
{
	vertex_type*					_vertex0 = vertex(vertex_id0);
	ASSERT						(_vertex0);
	vertex_type*					_vertex1 = vertex(vertex_id1);
	ASSERT						(_vertex1);
	_vertex0->add_edge			(_vertex1,edge_weight);
}

TEMPLATE_SPECIALIZATION
inline	void graph::add_edge		(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1, _edge_weight_type const& edge_weight0, _edge_weight_type const& edge_weight1)
{
	add_edge					(vertex_id0,vertex_id1,edge_weight0);
	add_edge					(vertex_id1,vertex_id0,edge_weight1);
}

TEMPLATE_SPECIALIZATION
inline	void graph::remove_edge		(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1)
{
	vertex_type*					_vertex = vertex(vertex_id0);
	ASSERT						(_vertex);
	ASSERT						(vertex(vertex_id1));
	_vertex->remove_edge		(vertex_id1);
}

TEMPLATE_SPECIALIZATION
inline	size_t graph::vertex_count	() const
{
	return						m_vertices.size();
}

TEMPLATE_SPECIALIZATION
inline	size_t graph::edge_count	() const
{
	return						m_edge_count;
}

TEMPLATE_SPECIALIZATION
inline	bool graph::empty			() const
{
	return						m_vertices.empty();
}

TEMPLATE_SPECIALIZATION
inline	void graph::clear			()
{
	while (!vertices().empty())
		remove_vertex			(vertices().begin()->first);

	ASSERT						(!m_edge_count, "graph is corrupted");
}

TEMPLATE_SPECIALIZATION
inline	const typename graph::vertex_type* graph::vertex	(_vertex_id_type const& vertex_id) const
{
	const_vertex_iterator		I = vertices().find(vertex_id);
	if (vertices().end() == I)
		return					0;
	return						(*I).second;
}

TEMPLATE_SPECIALIZATION
inline	typename graph::vertex_type* graph::vertex		(_vertex_id_type const& vertex_id)
{
	vertex_iterator				I = m_vertices.find(vertex_id);
	if (m_vertices.end() ==	I)
		return					0;
	return						(*I).second;
}

TEMPLATE_SPECIALIZATION
inline	const typename graph::edge_type* graph::edge		(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1) const
{
	vertex_type const*			_vertex = vertex(vertex_id0);
	if (!_vertex)
		return					0;
	return						_vertex->edge(vertex_id1);
}

TEMPLATE_SPECIALIZATION
inline	typename graph::edge_type* graph::edge			(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1)
{
	vertex_type*					_vertex = vertex(vertex_id0);
	if (!_vertex)
		return					0;
	return						_vertex->edge(vertex_id1);
}

TEMPLATE_SPECIALIZATION
inline	typename graph::VERTICES const& graph::vertices	() const
{
	return						m_vertices;
}

TEMPLATE_SPECIALIZATION
inline	graph const& graph::header	() const
{
	return						*this;
}

#if 0
TEMPLATE_SPECIALIZATION
inline	void graph::save			(fs::writer stream)
{
	save_data					(vertex_count(),*stream);
	{
		const_vertex_iterator	I = vertices().begin();
		const_vertex_iterator	E = vertices().end();
		for ( ; I != E; ++I) {
			save_data			((*I).second->vertex_id(),stream);
			save_vertex			(*(*I).second,stream);
		}
	}
	
	save_data					(edge_count(),*stream);
 	{
 		const_vertex_iterator	I = vertices().begin();
 		const_vertex_iterator	E = vertices().end();
 		for ( ; I != E; ++I) {
			const_edge_iterator	i = (*I).second->edges().begin();
			const_edge_iterator	e = (*I).second->edges().end();
			for ( ; i != e; ++i) {
				save_data		((*I).second->vertex_id(),stream);
				save_data		((*i).vertex()->vertex_id(),stream);
				save_data		((*i).weight(),stream);
			}
		}
	}
}

TEMPLATE_SPECIALIZATION
inline	void graph::load			(fs::reader stream)
{
	clear						();

	size_t						_vertex_count;
	load_data					(_vertex_count,stream);
	for (size_t i=0; i<_vertex_count; ++i) {
		_vertex_id_type			vertex_id;
		load_data				(vertex_id,stream);
		
		add_vertex				(vertex_id,load_vertex(vertex_id,stream));
	}
	ASSERT						(vertex_count() == _vertex_count);
	
	size_t						_edge_count;
	load_data					(_edge_count,stream);
	for (size_t i=0; i<_edge_count; ++i) {
		_vertex_id_type			vertex_id0;
		load_data				(vertex_id0,stream);

		_vertex_id_type			vertex_id1;
		load_data				(vertex_id1,stream);

		_edge_weight_type		edge_weight;
		load_data				(edge_weight,stream);

		add_edge				(vertex_id0,vertex_id1,edge_weight);
	}
	ASSERT						(edge_count() == _edge_count);
}
#endif

TEMPLATE_SPECIALIZATION
inline	bool graph::operator==		(graph_base const& obj) const
{
	if (vertex_count() != obj.vertex_count())
		return					false;

	if (edge_count() != obj.edge_count())
		return					false;

	return						equal(vertices(),obj.vertices());
}

#undef TEMPLATE_SPECIALIZATION
#undef graph

#endif // #ifndef CS_GTL_GRAPH_BASE_INLINE_H_INCLUDED