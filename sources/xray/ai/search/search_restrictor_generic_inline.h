////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace search_restrictor {

#define TEMPLATE_SPECIALIZATION	template < typename vertex_id_type, typename const_edge_iterator, typename distance_type >
#define restrictor				search_restrictor::generic< vertex_id_type, const_edge_iterator, distance_type >

TEMPLATE_SPECIALIZATION
inline restrictor::generic		(
	vertex_id_type const& start_vertex_id,
	vertex_id_type const& target_vertex_id,
	distance_type const& max_range,
	u32 const max_iteration_count,
	u32 const max_visited_vertex_count
	) :
	super						( max_range, max_iteration_count, max_visited_vertex_count ),
	m_start_vertex_id			( start_vertex_id ),
	m_target_vertex_id			( target_vertex_id )
{
}

TEMPLATE_SPECIALIZATION
inline bool restrictor::target_reached	( vertex_id_type const& vertex_id ) const
{
	return						m_target_vertex_id == vertex_id;
}

TEMPLATE_SPECIALIZATION
inline vertex_id_type const& restrictor::start_vertex_id	( u32 const index ) const
{
	XRAY_UNREFERENCED_PARAMETER	( index );
	return						m_start_vertex_id;
}

TEMPLATE_SPECIALIZATION
inline vertex_id_type const& restrictor::target_vertex_id	( ) const
{
	return						m_target_vertex_id;
}

#undef TEMPLATE_SPECIALIZATION
#undef restrictor

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_INLINE_H_INCLUDED