////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_DATA_H_INCLUDED
#define CS_GTL_GRAPH_DATA_H_INCLUDED

#include <cs/gtl/graph_base.h>
#include <cs/gtl/graph_helper.h>

namespace gtl { namespace detail {

template <
	typename _data_type,
	typename _edge_weight_type,
	typename _vertex_id_type,
	template <
		typename,
		typename,
		typename
	> class _graph_vertex_type,
	template <
		typename,
		typename
	> class _graph_edge_type
>
struct helper_data {
	typedef _vertex_id_type		_vertex_id_type;

	typedef	_graph_vertex_type<
				_data_type,
				_vertex_id_type,
				helper_data
			>					vertex_type;

	typedef	_graph_edge_type<
				_edge_weight_type,
				helper_data
			>					edge_type;

	typedef graph_helper<
				vertex_type,
				_vertex_id_type,
				_data_type
			>					graph_helper_type;

	typedef graph_base<
				_edge_weight_type,
				_vertex_id_type,
				vertex_type,
				edge_type,
				graph_helper_type
			>					result;
};

template <
	typename _data_type,
	typename _edge_weight_type,
	typename _vertex_id_type,
	template <
		typename,
		typename,
		typename
	> class _graph_vertex_type,
	template <
		typename,
		typename
	> class _graph_edge_type
>
class _graph : 
	public 
		helper_data<
			_data_type,
			_edge_weight_type,
			_vertex_id_type,
			_graph_vertex_type,
			_graph_edge_type
		>::result
{
private:
	typedef	helper_data<
				_data_type,
				_edge_weight_type,
				_vertex_id_type,
				_graph_vertex_type,
				_graph_edge_type
			>									helper_data;

	typedef	typename helper_data::result		inherited;

public:
	typedef	typename helper_data::vertex_type	vertex_type;
	typedef	typename helper_data::edge_type		edge_type;

public:
	inline	void				add_vertex			(_data_type& data, _vertex_id_type const& vertex_id);
};

};};

#include <cs/gtl/graph_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_DATA_H_INCLUDED