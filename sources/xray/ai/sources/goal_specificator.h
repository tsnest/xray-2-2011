////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GOAL_SPECIFICATOR_H_INCLUDED
#define GOAL_SPECIFICATOR_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

class goal;
class pddl_planner;
class specified_problem;
class search;

class goal_specificator : private boost::noncopyable
{
public:
					goal_specificator	( );
					~goal_specificator	( );

			bool	can_plan_be_created	(
						specified_problem& specific_problem,
						goal const* goal_for_specification,
						search& search_service
					);
			void	finalize			( ) const;

public:
	typedef fixed_vector< u32, 4 >		indices_type;

private:
			void	fill_problem		( specified_problem& specific_problem, goal const* current_goal, indices_type const& targets_indices );
			bool are_parameters_suitable( specified_problem& specific_problem, goal const* goal_for_specification, indices_type const& targets_indices ) const;

private:	
	pddl_planner*						m_planner;
}; // class goal_specificator

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef GOAL_SPECIFICATOR_H_INCLUDED