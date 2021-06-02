////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

#define	the_search					planning::search

template < typename search_type >
inline bool the_search::search_impl	( propositional_planner& graph, search_type& search )
{
	return							search.search( graph, m_plan ) && !m_plan.empty();
}

inline bool the_search::search_backward	( propositional_planner& graph )
{
	return							search_impl( graph, m_search_backward );
}

inline bool the_search::search_forward	( propositional_planner& graph )
{
	return							search_impl( graph, m_search_forward );
}

inline the_search::plan_type const& the_search::plan	( ) const
{
	return							m_plan;
}

inline u32 the_search::visited_vertex_count				( ) const
{
	return							m_search.m_priority_queue.visited_vertex_count();
}

#undef the_search

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_INLINE_H_INCLUDED