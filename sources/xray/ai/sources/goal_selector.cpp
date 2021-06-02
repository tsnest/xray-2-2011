////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "goal_selector.h"
#include "goal_specificator.h"
#include "blackboard.h"

namespace xray {
namespace ai {
namespace planning {

goal_selector::goal_selector	( ) :
	m_specificator				( NEW( goal_specificator )() )
{
}

goal_selector::~goal_selector	( )
{
	DELETE						( m_specificator );
}

void goal_selector::set_current_goal(
		search& search_service,
		specified_problem& problem,
		behaviour_ptr const& current_behaviour,
		blackboard& board
	)
{
 	board.set_current_goal			( 0 );
 	
	for ( goal const* it_goal = current_behaviour->get_goals(); it_goal; it_goal = it_goal->m_next )
 	{
 		if ( m_specificator->can_plan_be_created( problem, it_goal, search_service ) )
 		{
 			board.set_current_goal	( it_goal );
 			break;
 		}
 	}
}

void goal_selector::finalize		( ) const
{
	m_specificator->finalize		( );
}

} // namespace planning
} // namespace ai
} // namespace xray
