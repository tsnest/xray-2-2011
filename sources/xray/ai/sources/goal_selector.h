////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GOAL_SELECTOR_H_INCLUDED
#define GOAL_SELECTOR_H_INCLUDED

#include "behaviour.h"

namespace xray {
namespace ai {

class blackboard;

namespace planning {

class goal_specificator;
class specified_problem;
class search;

class goal_selector : private boost::noncopyable
{
public:
			goal_selector	( );
			~goal_selector	( );

	void	set_current_goal(
				search& search_service,
				specified_problem& problem,
				behaviour_ptr const& current_behaviour,
				blackboard& board
			);
	void	finalize		( ) const;

private:
	goal_specificator*		m_specificator;
}; // class goal_selector

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef GOAL_SELECTOR_H_INCLUDED