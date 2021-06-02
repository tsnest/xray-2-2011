////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_FORWARD_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_FORWARD_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace search_restrictor {

inline planner_forward::impl::impl	(
		graph_wrapper_type const& wrapper,
		vertex_id_type const& start_vertex_id,
		target_id_type const& target_vertex_id,
		distance_type const& max_range,
		u32 const max_iteration_count,
		u32 const max_visited_vertex_count
	) :
	m_wrapper						( wrapper ),
	m_start_vertex_id				( start_vertex_id ),
	m_target_vertex_id				( target_vertex_id ),
	m_max_range						( max_range ),
	m_max_iteration_count			( max_iteration_count ),
	m_max_visited_vertex_count		( max_visited_vertex_count )
{
}

template < typename priority_queue >
inline bool planner_forward::impl::limit_reached	( priority_queue const& queue, u32 const iteration_count ) const
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

inline void planner_forward::impl::on_before_search	( )
{
}

inline void planner_forward::impl::on_after_search	( bool const success ) const
{
	XRAY_UNREFERENCED_PARAMETER				( success );
}

template < typename VertexType >
inline bool planner_forward::impl::accessible		( vertex_id_type const& neighbour_vertex_id, VertexType const& current_vertex, graph_type::const_edge_iterator const& edge_iterator ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( &neighbour_vertex_id, &current_vertex, &edge_iterator );
	return							m_wrapper.applied();
}

inline u32 planner_forward::impl::get_start_vertices_count	( ) const
{
	return									1;
}

inline planner_forward::vertex_id_type planner_forward::impl::start_vertex_id	( u32 const index ) const
{
	R_ASSERT								( index == 0 );
	
	vertex_id_type							result;

	vertex_id_type::properties_type::const_iterator it_property	= m_start_vertex_id.properties().begin();
	vertex_id_type::properties_type::const_iterator it_end		= m_start_vertex_id.properties().end();

	for ( ; it_property != it_end; ++it_property )
		result.add							( *it_property );
	
	return									result;
}

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_FORWARD_INLINE_H_INCLUDED