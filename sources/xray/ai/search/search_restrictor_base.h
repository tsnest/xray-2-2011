////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_H_INCLUDED

namespace xray {
namespace ai {
namespace search_restrictor {

template < typename vertex_id_type, typename const_edge_iterator, typename distance_type >
class base
{
public:
	inline						base				(
									distance_type const& max_range = std::numeric_limits< distance_type >::max(),
									u32 const max_iteration_count = u32(-1),
									u32 const max_visited_vertex_count = u32(-1)
								);

public:
	inline void					on_before_search	( );
	inline void					on_after_search		( bool const success ) const;
	inline bool					accessible			( vertex_id_type const& neighbour_vertex_id, vertex_id_type const& current_vertex_id, const_edge_iterator const& edge_iterator ) const;
	
public:
	template < typename priority_queue >
	inline bool					limit_reached		( priority_queue const& queue, u32 const iteration_count ) const;

protected:
	distance_type				m_max_range;
	u32							m_max_iteration_count;
	u32							m_max_visited_vertex_count;
}; // class base

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_restrictor_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_BASE_H_INCLUDED