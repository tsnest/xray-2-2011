////////////////////////////////////////////////////////////////////////////
//	Created		: 25.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pddl_planner.h"
#include "pddl_domain.h"
#include "pddl_problem.h"
#include <xray/ai/search/const_oracle.h>
#include "generalized_action.h"
#include "pddl_predicate_redirector.h"
#include "specified_problem.h"
#include <xray/ai/search/plan_tracker.h>
#include <xray/ai/search/search.h>

namespace xray {
namespace ai {
namespace planning {

pddl_planner::pddl_planner	( ) :
	m_failed				( false ),
	m_plan_tracker			( NEW( plan_tracker )( *return_this() ) )
{
}

pddl_planner::~pddl_planner	( ) 
{
	DELETE					( m_plan_tracker );
}

bool pddl_planner::forward_search_required	( specified_actions_type const& specified_actions )
{
	u32 preconditions_count	= 0;
	u32 effects_count		= 0;

	for ( u32 i = 0; i < specified_actions.size(); ++i )
	{
		preconditions_count	+= specified_actions[i].get_preconditions_count();
		effects_count		+= specified_actions[i].get_effects_count();
	}
	return					preconditions_count < effects_count;
}

u32 pddl_planner::calculate_property_index( pddl_world_state_property_impl const& property, specified_problem const& actual_problem ) const
{
	u32 const instances_count	= property.get_indices_count();
	u32 property_index			= actual_problem.get_predicate_offset( property.get_predicate()->get_type() );
	R_ASSERT					( property_index != u32(-1), "invalid calculated world state size" );
	u32 combinations_count		= 1;
	
	for ( u32 i = 0; i < instances_count; ++i )
	{
		u32 const other_side_i	= instances_count - 1 - i;
		property_index			+= property.get_index( other_side_i ) * combinations_count;
		combinations_count		*= actual_problem.get_count_of_objects_by_type( property.get_predicate()->get_parameter( other_side_i ) );
	}			

	return						property_index;
}

bool pddl_planner::build_plan	( search& search_service, specified_problem& actual_problem, pcstr caption, bool verbose )
{
	actual_problem.calculate_predicates_offsets();
	
	reset						( );
	
	set_current_world_state		( actual_problem );

	specified_actions_type		specified_actions;
	specify_actions				( specified_actions, actual_problem );
	
	if ( specified_actions.size() == 0 )
	{
		m_failed				= true;
		return					false;
	}

	add_actions					( specified_actions, actual_problem );

	set_target_world_state		( actual_problem );

	m_plan_tracker->verbose		( verbose );
	m_planner.forward_search	( forward_search_required( specified_actions ) );
	
	bool const was_actual		= m_planner.update( search_service, caption );

	u32 const plan_size			= search_service.plan().size();
	m_failed					= m_planner.failed() || !plan_size;

	if ( !m_failed )
		for ( u32 i = 0; i < plan_size; ++i )
			specified_actions[search_service.plan()[i]].add_this_to_plan( m_current_plan, actual_problem );

	if ( verbose )
	{
		actual_problem.dump_target_world_state();
		LOG_INFO				( "solution plan:" );
		if ( !m_failed )
			for ( u32 i = 0; i < plan_size; ++i )
				specified_actions[search_service.plan()[i]].debug_output( actual_problem );
		else
			LOG_INFO			( "no plan can be built for such conditions" );
	}

	if ( was_actual )
 		m_plan_tracker->track	( );

	else if ( !m_failed )
	{
	 	//m_plan_tracker->finalize( );
		m_plan_tracker->on_plan_changed	( m_current_plan );
	 	m_plan_tracker->track	( );
	}

	return						!m_failed;
}

void pddl_planner::finalize		( ) const
{
	m_plan_tracker->finalize	( );
}

void pddl_planner::set_current_world_state	( specified_problem const& actual_problem )
{
	for ( u32 i = 0; i < actual_problem.get_world_state_size(); ++i )
	{
		string256					oracle_caption;
		xray::sprintf				( oracle_caption, "oracle#%d", i );
		m_planner.oracles().add		( i, NEW( pddl_predicate_redirector )( oracle_caption, actual_problem, i ) );
	}
}

static bool increment_offsets		(
		vector< u32 >& offsets,
		generalized_action const& prototype_action,
		specified_problem const& problem
	)
{
	u32 const offsets_count			= offsets.size();
	R_ASSERT						( offsets_count == prototype_action.get_parameters_count() );
	bool result						= false;
	u32 index						= u32(-1);
	for ( u32 i = 0; i < offsets_count; ++i )
	{
		index						= offsets_count - 1 - i;
		if ( !problem.parameter_iterate_first_only( prototype_action.get_type(), index ) &&
			 ( offsets[index] < problem.get_count_of_objects_by_type( prototype_action.get_parameter_type( index ) ) - 1 ) )
		{
			++offsets[index];
			result					= true;
			break;
		}
		else
			continue;
	}
	if ( !result || index == u32(-1) )
		return						false;
	
	for ( u32 i = index + 1; i < offsets_count; ++i )
		offsets[i]					= 0;
	
	return							true;
}

void pddl_planner::specify_actions	(
		generalized_action const& prototype,
		specified_actions_type& destination,
		specified_problem const& actual_problem
	)
{
	if ( !specified_action::can_be_specified( prototype, actual_problem ) || !actual_problem.get_problem().has_action_instance( prototype.get_type() ) )
		return;

	u32 const parameters_count		= prototype.get_parameters_count();
	vector< u32 > offsets			( parameters_count, 0 );
	bool offsets_incremented		= true;
	
	while ( offsets_incremented )
	{
		specified_action::instances_type parameter_ids;
		for ( u32 j = 0; j < parameters_count; ++j )
		{
			parameter_ids.push_back	( offsets[j] );

			if ( j == parameters_count - 1 )
				offsets_incremented	= increment_offsets( offsets, prototype, actual_problem );
		}
		if ( parameters_count == 0 )
			offsets_incremented		= false;

		if ( parameters_count && !actual_problem.are_parameters_suitable( prototype.get_type(), parameter_ids ) )
			continue;

		specified_action			new_specified_action;
		if ( new_specified_action.specify( prototype, parameter_ids ) )
			destination.push_back	( new_specified_action );
	}
}

void pddl_planner::specify_actions	( specified_actions_type& destination, specified_problem const& actual_problem )
{
	for ( generalized_action const* it = actual_problem.get_domain().get_actions(); it; it = it->next )
	{
		generalized_action const* const prototype = it;
		specify_actions				( *prototype, destination, actual_problem );
		for ( u32 j = 0; j < prototype->get_clones_count(); ++j )
			specify_actions			( *prototype->get_clone( j ), destination, actual_problem );	
	}
}

void pddl_planner::add_world_state_property	(
		pddl_world_state_property_impl const& property,
		world_state& state,
		specified_problem const& actual_problem
	)
{
	u32 const property_index	= calculate_property_index( property, actual_problem );
	state.add					( world_state_property( property_index, property.get_result() ) );
}

void pddl_planner::add_actions	( specified_actions_type const& specified_actions, specified_problem const& actual_problem )
{
	for ( u32 i = 0; i < specified_actions.size(); ++i )
	{
		string256				operator_name;
		xray::sprintf				( operator_name, "operator#%d", i );
		specified_action const& current_action = specified_actions[i];
		operator_type* action	= NEW( operator_type )( operator_name, current_action.get_cost() );

		for ( u32 j = 0; j < specified_actions[i].get_preconditions_count(); ++j )
		{
			pddl_world_state_property_impl const& property	= specified_actions[i].get_precondition( j );
			add_world_state_property( property, action->preconditions(), actual_problem );
		}
		
		for ( u32 j = 0; j < specified_actions[i].get_effects_count(); ++j )
		{
			pddl_world_state_property_impl const& property	= specified_actions[i].get_effect( j );
			add_world_state_property( property, action->effects(), actual_problem );
		}
		
		m_planner.operators().add( i, action );
	}
}

void pddl_planner::set_target_world_state	( specified_problem const& actual_problem )
{
	typedef dnf_world_state::properties_type properties_type;
	typedef pddl_world_state_property_impl	 property_type;
	
	dnf_world_state							target;
	u32 const offsets_count					= actual_problem.get_targets_offsets_count();
	u32 const world_state_size				= actual_problem.get_target_world_state_size();
	u32 current_index						= 0;

	if ( offsets_count == 0 )
	{
		for ( u32 i = 0; i < actual_problem.get_target_world_state_size(); ++i )
		{
			property_type const& property	= actual_problem.get_world_state_property( i );
			u32 const property_index		= calculate_property_index( property, actual_problem );
			target.add						( world_state_property( property_index, property.get_result() ), target.properties().begin() );
		}
	}
	else
	{
		for ( u32 j = 0; j <= offsets_count; ++j )
		{
			u32 const current_offset			= j < offsets_count ? actual_problem.get_target_offset( j ) : world_state_size;
			for ( ; current_index < current_offset; ++current_index )
			{
				properties_type::iterator it_where = j == 0 ? target.properties().begin() : target.properties().begin() + actual_problem.get_target_offset( j - 1 );
				property_type const& property	= actual_problem.get_world_state_property( current_index );
				u32 const property_index		= calculate_property_index( property, actual_problem );
				target.add						( world_state_property( property_index, property.get_result() ), it_where );
			}
		}
	}

	for ( u32 i = 0; i < actual_problem.get_targets_offsets_count(); ++i )
		m_planner.add_target_offset			( actual_problem.get_target_offset( i ) );
	
	m_planner.target						( target );
}

void pddl_planner::reset			( )
{
	m_planner.clear					( );
	m_current_plan.clear			( );
	m_failed						= false;
}

} // namespace planning
} // namespace ai
} // namespace xray