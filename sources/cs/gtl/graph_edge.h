////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_edge.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph edge class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_EDGE_H_INCLUDED
#define CS_GTL_GRAPH_EDGE_H_INCLUDED

namespace gtl { namespace detail {

template <
	typename _edge_weight_type,
	typename _graph_type
>
class graph_edge {
public:
	typedef _edge_weight_type						_edge_weight_type;
	typedef typename _graph_type::vertex_type		_vertex_type;
	typedef typename _graph_type::_vertex_id_type	_vertex_id_type;

private:
	_edge_weight_type				m_weight;
	_vertex_type*					m_vertex;

public:
	inline							graph_edge		(_edge_weight_type const& weight, _vertex_type* vertex);
	inline	_edge_weight_type const&	weight			() const;
	inline	_vertex_type*			vertex			() const;
	inline	bool					operator==		(_vertex_id_type const& vertex_id) const;
	inline	bool					operator==		(graph_edge const& obj) const;
};

};};

#include <cs/gtl/graph_edge_inline.h>

#endif // #ifndef CS_GTL_GRAPH_EDGE_H_INCLUDED