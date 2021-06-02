////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SPECIFIED_ACTION_H_INCLUDED
#define SPECIFIED_ACTION_H_INCLUDED

#include "pddl_world_state_property_impl.h"
#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class specified_problem;
class generalized_action;

class specified_action
{
public:
	typedef pddl_world_state_property_impl::indices_type instances_type;

public:
			specified_action	( );
	
	bool	specify				( generalized_action const& prototype, instances_type const& parameters );
	bool	specify_property	( pddl_world_state_property_impl const& property_prototype, world_state_type& state );
	void	add_this_to_plan	( plan_type& plan, specified_problem const& specific_problem ) const;
	void	debug_output		( specified_problem const& problem ) const;

	inline	u32	get_preconditions_count	( ) const	{ return m_preconditions.size(); }
	inline	u32		get_effects_count	( ) const	{ return m_effects.size(); }
			
	pddl_world_state_property_impl const&	get_precondition	( u32 const index ) const;
	pddl_world_state_property_impl const&	get_effect			( u32 const index ) const;
			u32		get_cost			( ) const;

	static	bool	can_be_specified	( generalized_action const& prototype, specified_problem const& problem );
	
private:
	static	bool	are_instances_indices_equal	( pddl_world_state_property_impl const& first_property, pddl_world_state_property_impl const& second_property );

private:
	world_state_type			m_preconditions;
	world_state_type			m_effects;
	instances_type				m_parameters_instances;
	generalized_action const*	m_prototype;
}; // class specified_action

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef SPECIFIED_ACTION_H_INCLUDED