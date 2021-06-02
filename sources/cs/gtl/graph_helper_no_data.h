////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_helper_no_data.h
//	Created 	: 14.01.2004
//  Modified 	: 03.03.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph helper no data class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_HELPER_NO_DATA_H_INCLUDED
#define CS_GTL_GRAPH_HELPER_NO_DATA_H_INCLUDED

#include <loki/emptytype.h>
#include <cs/gtl/graph_helper_base.h>

namespace gtl { namespace detail {

template <
	typename _vertex_type,
	typename _vertex_id_type
>
class graph_helper<
		_vertex_type,
		_vertex_id_type,
		Loki::EmptyType
	> : 
	public graph_helper_base<
		_vertex_type,
		_vertex_id_type,
		Loki::EmptyType
	>
{
public:
	typedef _vertex_type	_vertex_type;
	typedef _vertex_id_type	_vertex_id_type;
	typedef Loki::EmptyType	_data_type;

	inline	_vertex_type*	create_vertex	(_vertex_id_type const& vertex_id);
	inline	_vertex_type*	load_vertex	(_vertex_id_type const& vertex_id,	fs::reader stream);
	inline	void			save_vertex		(_vertex_type const& vertex,		fs::writer stream) const;
};

};};

#include <cs/gtl/graph_helper_no_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_HELPER_NO_DATA_H_INCLUDED