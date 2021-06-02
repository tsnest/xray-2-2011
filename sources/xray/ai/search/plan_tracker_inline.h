////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PLAN_TRACKER_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PLAN_TRACKER_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline plan_tracker::plan_tracker	( pddl_planner const& planner ) :
	m_planner						( planner ),
	m_first_time					( true ),
	m_verbose						( false )
{
}

inline void plan_tracker::verbose	( bool const value )
{
	m_verbose						= value;
}

inline bool plan_tracker::verbose	( ) const
{
	return							m_verbose;
}

// inline plan_tracker::operator_id_type const& plan_tracker::current_operator_id	( ) const
// {
// 	R_ASSERT						( !m_first_time, "plan tracking hasn't been started yet" );
// 	return							m_current_operator_id;
// }

// inline plan_tracker::operator_type& plan_tracker::current_operator				( ) const
// {
// 	return							operator_object( current_operator_id() );
// }

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PLAN_TRACKER_INLINE_H_INCLUDED