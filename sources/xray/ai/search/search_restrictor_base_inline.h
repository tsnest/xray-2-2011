////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace search_restrictor {

#define TEMPLATE_SPECIALIZATION	template < typename vertex_id_type, typename const_edge_iterator, typename distance_type >
#define restrictor				search_restrictor::base< vertex_id_type, const_edge_iterator, distance_type >

TEMPLATE_SPECIALIZATION
inline restrictor::base			(
	distance_type const& max_range,
	u32 const max_iteration_count,
	u32 const max_visited_vertex_count
	) :
	m_max_range					( max_range ),
	m_max_iteration_count		( max_iteration_count ),
	m_max_visited_vertex_count	( max_visited_vertex_count )
{
}

TEMPLATE_SPECIALIZATION
inline void restrictor::on_before_search	( )
{
}

TEMPLATE_SPECIALIZATION
inline void restrictor::on_after_search		( bool const success ) const
{
	XRAY_UNREFERENCED_PARAMETER				( success );
}

TEMPLATE_SPECIALIZATION
inline bool restrictor::accessible			( vertex_id_type const& neighbour_vertex_id, vertex_id_type const& current_vertex_id, const_edge_iterator const& edge_iterator ) const
{
	XRAY_UNREFERENCED_PARAMETERS			( &current_vertex_id, &neighbour_vertex, &edge_id );
	return									true;
}

TEMPLATE_SPECIALIZATION
template < typename priority_queue >
inline bool restrictor::limit_reached		(
		priority_queue const& queue,
		u32 const iteration_count
	) const
{
	if ( iteration_count >= m_max_iteration_count )
		return								true;

	distance_type const& current_range		= queue.best().f();
	if ( current_range >= m_max_range )
		return								true;

	u32 const visited_vertex_count			= queue.visited_vertex_count();
	if ( visited_vertex_count >= m_max_visited_vertex_count )
		return								true;

	return									false;
}

#undef TEMPLATE_SPECIALIZATION
#undef restrictor

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_INLINE_H_INCLUDED