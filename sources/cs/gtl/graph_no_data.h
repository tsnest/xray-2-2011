////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_no_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_NO_DATA_H_INCLUDED
#define CS_GTL_GRAPH_NO_DATA_H_INCLUDED

#include <cs/gtl/graph_base.h>
#include <loki/emptytype.h>
#include <cs/gtl/graph_helper.h>

namespace gtl { namespace detail {

template <
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
struct helper_no_data {
	typedef _vertex_id_type		_vertex_id_type;

	typedef	_graph_vertex_type<
				Loki::EmptyType,
				_vertex_id_type,
				helper_no_data
			>					vertex_type;

	typedef	_graph_edge_type<
				_edge_weight_type,
				helper_no_data
			>					edge_type;

	typedef graph_helper<
				vertex_type,
				_vertex_id_type,
				Loki::EmptyType
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
class 
	_graph<
		Loki::EmptyType,
		_edge_weight_type,
		_vertex_id_type,
		_graph_vertex_type,
		_graph_edge_type
	> : 
	public 
		helper_no_data<
			_edge_weight_type,
			_vertex_id_type,
			_graph_vertex_type,
			_graph_edge_type
		>::result
{
private:
	typedef helper_no_data<
				_edge_weight_type,
				_vertex_id_type,
				_graph_vertex_type,
				_graph_edge_type
			>										helper_no_data;
	
	typedef typename helper_no_data::result			inherited;

public:
	typedef Loki::EmptyType							_data_type;
	typedef	typename helper_no_data::vertex_type	vertex_type;
	typedef	typename helper_no_data::edge_type		edge_type;

public:
	inline		void				add_vertex			(_vertex_id_type const& vertex_id);
};

};};

#include <cs/gtl/graph_no_data_inline.h>

#endif // #ifndef CS_GTL_GRAPH_NO_DATA_H_INCLUDED