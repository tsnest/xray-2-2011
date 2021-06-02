////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

#define PLANNER	planning::propositional_planner_base

inline PLANNER::propositional_planner_base			( propositional_planner* actual_planner ) :
	m_operators				( *actual_planner ),
	m_oracles				( *actual_planner ),
	m_actual				( true )
{
}

inline bool PLANNER::actual							( ) const
{
	return					m_actual;
}

inline void PLANNER::make_inactual					( )
{
	m_actual				= false;
}

inline void PLANNER::clear							( )
{
	operators().clear		( );
	oracles().clear			( );
}

inline operator_holder& PLANNER::operators			( )
{
	return					m_operators;
}

inline operator_holder const& PLANNER::operators	( ) const
{
	return					m_operators;
}

inline oracle_holder& PLANNER::oracles				( )
{
	return					m_oracles;
}

inline oracle_holder const& PLANNER::oracles		( ) const
{
	return					m_oracles;
}

#undef PLANNER

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_INLINE_H_INCLUDED