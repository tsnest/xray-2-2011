////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_base.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex base class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_BASE_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_BASE_H_INCLUDED

namespace gtl { namespace detail {

template <
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex_base {
public:
	typedef _vertex_id_type										_vertex_id_type;
	typedef typename _graph_type::edge_type						_edge_type;
	typedef typename _graph_type::vertex_type					_vertex_type;
	typedef typename _edge_type::_edge_weight_type				_edge_weight_type;
	typedef cs_vector<_edge_type>								EDGES;
	typedef cs_vector<_vertex_type*>							VERTICES;

private:
	_vertex_id_type							m_vertex_id;
	EDGES									m_edges;
	// this container holds vertices, which has edges to us
	// this is needed for the fast vertex removal
	VERTICES								m_vertices;
	// this counter is use for fast edge count computation in graph
	size_t*									m_edge_count;

public:
	inline									graph_vertex_base	(_vertex_id_type const& vertex_id, size_t* edge_count);
	// destructor is not virtual, 
	// since we shouldn't remove vertices,
	// having pointer not to their final class
	inline									~graph_vertex_base	();
	inline		bool						operator==			(graph_vertex_base const& obj) const;
	inline		void						add_edge			(_vertex_type* vertex, _edge_weight_type const& edge_weight);
	inline		void						remove_edge			(_vertex_id_type const& vertex_id);
	inline		void						on_edge_addition	(_vertex_type* vertex);
	inline		void						on_edge_removal		(_vertex_type const* vertex);
	inline		_vertex_id_type const&		vertex_id			() const;
	inline		EDGES const&					edges				() const;
	inline		_edge_type const*			edge				(_vertex_id_type const& vertex_id) const;
	inline		_edge_type*					edge				(_vertex_id_type const& vertex_id);
};

};};

#include <cs/gtl/graph_vertex_base_inline.h>

#endif // #ifndef CS_GTL_GRAPH_VERTEX_BASE_H_INCLUDED