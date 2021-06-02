////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_helper_data.h
//	Created 	: 14.01.2004
//  Modified 	: 03.03.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph helper data class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_HELPER_DATA_H_INCLUDED
#define CS_GTL_GRAPH_HELPER_DATA_H_INCLUDED

#include <cs/gtl/graph_helper_base.h>

namespace gtl { namespace detail {

template <
	typename _vertex_type,
	typename _vertex_id_type,
	typename _data_type
>
class graph_helper : 
	public graph_helper_base<
		_vertex_type,
		_vertex_id_type,
		_data_type
	>
{
public:
	typedef _vertex_type	_vertex_type;
	typedef _vertex_id_type	_vertex_id_type;
	typedef _data_type		_data_type;

public:
	inline	_vertex_type*	create_vertex	(_vertex_id_type const& vertex_id,	_data_type& data);
	inline	_vertex_type*	load_vertex	(_vertex_id_type const& vertex_id,	fs::reader stream);
	inline	void			save_vertex		(_vertex_type const& vertex,		fs::writer stream) const;
};

};};

#include <cs/gtl/graph_helper_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_HELPER_DATA_H_INCLUDED