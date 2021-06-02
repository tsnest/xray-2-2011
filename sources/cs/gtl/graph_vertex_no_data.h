////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_no_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_NO_DATA_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_NO_DATA_H_INCLUDED

#include <loki/emptytype.h>
#include <cs/gtl/graph_vertex_base.h>

namespace gtl { namespace detail {

template <
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex<
		Loki::EmptyType,
		_vertex_id_type,
		_graph_type
	> : 
	public	
		graph_vertex_base<
			_vertex_id_type,
			_graph_type
		> 
{
private:
	typedef graph_vertex_base<_vertex_id_type,_graph_type>	inherited;

public:
	inline		graph_vertex	(_vertex_id_type const& vertex_id, size_t* edge_count);
};

};};

#include <cs/gtl/graph_vertex_no_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_VERTEX_NO_DATA_H_INCLUDED