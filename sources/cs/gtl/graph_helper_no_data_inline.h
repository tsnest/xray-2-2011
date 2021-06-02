////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_helper_no_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 03.03.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph helper no data class template inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_HELPER_NO_DATA_INLINE_H_INCLUDED
#define CS_GTL_GRAPH_HELPER_NO_DATA_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _vertex_type,\
	typename _vertex_id_type\
>

#define _graph_helper gtl::detail::graph_helper<\
	_vertex_type,\
	_vertex_id_type,\
	Loki::EmptyType\
>

TEMPLATE_SPECIALIZATION
inline	_vertex_type* _graph_helper::create_vertex	(_vertex_id_type const& vertex_id)
{
	return			cs_new<_vertex_type>(vertex_id,&m_edge_count);
}

TEMPLATE_SPECIALIZATION
inline	_vertex_type* _graph_helper::load_vertex	(_vertex_id_type const& vertex_id, fs::reader stream)
{
	return			create_vertex(vertex_id);
}

TEMPLATE_SPECIALIZATION
inline	void _graph_helper::save_vertex				(_vertex_type const& vertex, fs::writer stream) const
{
}

#undef TEMPLATE_SPECIALIZATION
#undef _graph_helper

#endif // #ifndef CS_GTL_GRAPH_HELPER_NO_DATA_INLINE_H_INCLUDED