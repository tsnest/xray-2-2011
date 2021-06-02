////////////////////////////////////////////////////////////////////////////
//	Created		: 25.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PLANNER_H_INCLUDED
#define PDDL_PLANNER_H_INCLUDED

#include "specified_action.h"
#include <xray/ai/search/propositional_planner.h>
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

class pddl_problem;
class specified_problem;
class world_state;
class search;
class action_instance;
class plan_tracker;

class pddl_planner : private boost::noncopyable
{
public:
					pddl_planner	( );
					~pddl_planner	( );
	
			bool	build_plan		( search& search_service, specified_problem& actual_problem, pcstr caption, bool verbose );
			void	finalize		( ) const;
	inline	bool	failed			( ) const	{ return m_failed; }

private:
	typedef	vector< specified_action >		specified_actions_type;

private:
	void	set_current_world_state	( specified_problem const& actual_problem );
	void	specify_actions			( specified_actions_type& destination, specified_problem const& actual_problem );
	void	add_actions				( specified_actions_type const& specified_actions, specified_problem const& actual_problem );
	void	add_world_state_property(
				pddl_world_state_property_impl const& property,
				world_state& state,
				specified_problem const& actual_problem
			);
	void	set_target_world_state	( specified_problem const& actual_problem );
	u32		calculate_property_index( pddl_world_state_property_impl const& property, specified_problem const& actual_problem ) const;
	void	specify_actions			(
				generalized_action const& prototype,
				specified_actions_type& destination,
				specified_problem const& actual_problem
			);
	void	reset					( );
	inline	pddl_planner*	return_this		( ) { return this; }

	static	bool	forward_search_required	( specified_actions_type const& specified_actions );

private:
	propositional_planner			m_planner;
	plan_tracker*					m_plan_tracker;	
	plan_type						m_current_plan;
	bool							m_failed;
}; // class pddl_planner

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_PLANNER_H_INCLUDED