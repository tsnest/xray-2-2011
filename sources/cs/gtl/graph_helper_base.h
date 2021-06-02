////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_helper_base.h
//	Created 	: 14.01.2004
//  Modified 	: 03.03.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph helper base class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_HELPER_BASE_H_INCLUDED
#define CS_GTL_GRAPH_HELPER_BASE_H_INCLUDED

namespace gtl { namespace detail {

template <
	typename _vertex_type,
	typename _vertex_id_type,
	typename _data_type
>
struct graph_helper_base {
	size_t						m_edge_count;
};

};};

#endif // #ifndef CS_GTL_GRAPH_H_INCLUDEDELPER_BASE_H_INCLUDED