////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BACKWARD_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BACKWARD_H_INCLUDED

#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/graph_wrapper_planner_base.h>

namespace xray {
namespace ai {
namespace search_restrictor {

struct planner_backward
{
	typedef planning::propositional_planner					graph_type;
	typedef graph_type::vertex_id_type						vertex_id_type;
	typedef graph_type::target_id_type						target_id_type;
	typedef graph_type::distance_type						distance_type;
	typedef graph_wrapper::propositional_planner_base::impl	graph_wrapper_type;
	
	class impl : private boost::noncopyable
	{
	public:
		inline							impl					(
											graph_wrapper_type const& wrapper,
											target_id_type const& start_vertex_id,
											vertex_id_type const& target_vertex_id,
											distance_type const& max_range = std::numeric_limits< distance_type >::max(),
											u32 const max_iteration_count = u32(-1),
											u32 const max_visited_vertex_count = u32(-1)
										);
				bool					target_reached			( vertex_id_type const& vertex_id );
		inline	void					on_before_search		( );
		inline	void					on_after_search			( bool const success ) const;
		inline	u32						get_start_vertices_count( ) const;
		inline vertex_id_type			start_vertex_id			( u32 const index = u32(-1) ) const;
		inline vertex_id_type const&	target_vertex_id		( ) const;
		template <typename VertexType>
		inline	bool					accessible				(
											vertex_id_type const& neighbour_vertex_id,
											VertexType const& current_vertex_id,
											graph_type::const_edge_iterator const& edge_iterator
										) const;
		
		template < typename priority_queue >
		inline	bool					limit_reached			( priority_queue const& queue, u32 const iteration_count ) const;

	private:
		graph_wrapper_type const&		m_wrapper;
		target_id_type const&			m_start_vertex_id;
		vertex_id_type const&			m_target_vertex_id;
		distance_type					m_max_range;
		u32								m_max_iteration_count;
		u32								m_max_visited_vertex_count;
	}; // class impl
}; // struct planner_backward

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_restrictor_planner_backward_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_PLANNER_BACKWARD_H_INCLUDED