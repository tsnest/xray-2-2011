////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_VERTEX_DATA_H_INCLUDED
#define CS_GTL_GRAPH_VERTEX_DATA_H_INCLUDED

#include <cs/gtl/graph_vertex_base.h>

namespace gtl { namespace detail {

template <
	typename _data_type,
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex : 
	public graph_vertex_base<
		_vertex_id_type,
		_graph_type
	> 
{
private:
	typedef graph_vertex_base<
				_vertex_id_type,
				_graph_type
			>					inherited;

private:
	_data_type					m_data;

public:
	inline						graph_vertex	(_data_type const& data, _vertex_id_type const& vertex_id, size_t* edge_count);
	// destructor is not virtual, 
	// since we shouldn't remove vertices,
	// having pointer not to their final class
	inline						~graph_vertex	();
	inline	bool				operator==		(graph_vertex const& obj) const;
	inline	_data_type const&	data			() const;
	inline	_data_type&			data			();
	inline	void				data			(_data_type const& data);
};

};};

#include <cs/gtl/graph_vertex_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_VERTEX_DATA_H_INCLUDED