////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "goal_specificator.h"
#include "pddl_planner.h"
#include "goal.h"
#include "pddl_problem.h"
#include "specified_problem.h"
#include "target_selector_base.h"
#include "pddl_domain.h"
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

goal_specificator::goal_specificator( ) :
	m_planner						( NEW( pddl_planner )() )
{
}
goal_specificator::~goal_specificator	( )
{
	DELETE							( m_planner );
}

void goal_specificator::finalize	( ) const
{
	m_planner->finalize				( );
}

void goal_specificator::fill_problem( specified_problem& specific_problem, goal const* current_goal, indices_type const& targets_indices )
{
	using namespace					selectors;

	specific_problem.reset			( );

	pddl_domain const& domain		= specific_problem.get_domain();
	pddl_problem const& problem		= specific_problem.get_problem();

	for ( generalized_action const* it = domain.get_actions(); it; it = it->next )
	{
		generalized_action const* const prototype = it;
		if ( !problem.has_action_instance( prototype->get_type() ) )
			continue;
		
		for ( u32 k = 0; k < prototype->get_parameters_count(); ++k )
		{
			specific_problem.fill_parameter_targets( prototype->get_type() );
			target_selector_base* selector = specific_problem.get_parameter_selector( prototype->get_type(), k );
			if ( !selector )
				continue;

			for ( u32 j = 0; j < selector->get_targets_count(); ++j )
				specific_problem.add_object_instance( selector->get_target( j ), selector->get_target_caption( j ) );
		}
	}

	for ( u32 k = 0; k < current_goal->get_parameters_count(); ++k )
	{
		action_parameter* parameter	= current_goal->get_parameter( k );
		target_selector_base* selector = specific_problem.get_parameter_selector( parameter );
		if ( !selector )
			continue;
		
		for ( u32 j = 0; j < selector->get_targets_count(); ++j )
			specific_problem.add_object_instance( selector->get_target( j ), selector->get_target_caption( j ) );
	}
	
	for ( u32 i = 0; i < current_goal->get_target_world_state_size(); ++i )
	{
		pddl_world_state_property_impl const& property = current_goal->get_target_state_property( i );
		property_values_type		objects;
		if ( current_goal->has_owner_parameter() )
		{
			object_type const& owner = specific_problem.get_owner_as_object();
			specific_problem.add_object_instance( owner, specific_problem.get_owner_caption() );
			objects.push_back		( owner.first );
		}
		for ( u32 j = 0; j < property.get_indices_count(); ++j )
		{
			action_parameter* parameter = current_goal->get_parameter( property.get_index( j ) );
			target_selector_base* selector = specific_problem.get_parameter_selector( parameter );
			if ( !selector )
				continue;

			objects.push_back		( selector->get_target( targets_indices[property.get_index( j )] ).first );
		}

		pddl_world_state_property_impl		new_target( property.get_predicate(), property.get_result() );
		for ( u32 i = 0; i < objects.size(); ++i )
		{
			u32 const index					= specific_problem.get_object_index( property.get_predicate()->get_parameter( i ), objects[i] );
			R_ASSERT						( index != u32(-1), "invalid object or its type, or it's absent in current problem" );
			new_target.add_index			( index );
		}

		specific_problem.add_target_property( new_target );
	}
}

static bool increment_targets		(
		goal_specificator::indices_type& offsets,
		goal const* const goal_for_specification,
		specified_problem& specific_problem
	)
{
	using namespace					selectors;
	
	u32 const offsets_count			= offsets.size();
	R_ASSERT						( offsets_count == goal_for_specification->get_parameters_count() );
	bool result						= false;
	u32 index						= u32(-1);
	for ( u32 i = 0; i < offsets_count; ++i )
	{
		index						= offsets_count - 1 - i;
		action_parameter* parameter	= goal_for_specification->get_parameter( index );
		target_selector_base* selector = specific_problem.get_parameter_selector( parameter );

		if ( selector && !parameter->iterate_only_first() && offsets[index] < selector->get_targets_count() - 1 )
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

bool goal_specificator::are_parameters_suitable	(
		specified_problem& specific_problem,
		goal const* goal_for_specification,
		indices_type const& targets_indices
	) const
{
	using namespace					selectors;
	
	u32 const parameters_count		= goal_for_specification->get_parameters_count();
	R_ASSERT						( parameters_count == targets_indices.size() );
	
	property_values_type			objects;
	for ( u32 i = 0; i < parameters_count; ++i )
	{
		action_parameter* parameter = goal_for_specification->get_parameter( i );
		target_selector_base* selector = specific_problem.get_parameter_selector( parameter );
		if ( !selector )
			continue;

		objects.push_back			( selector->get_target( targets_indices[i] ).first );
	}
	return							goal_for_specification->are_parameters_suitable( objects );
}

bool goal_specificator::can_plan_be_created	(
		specified_problem& specific_problem,
		goal const* goal_for_specification,
		search& search_service
	)
{
	u32 const parameters_count				= goal_for_specification->get_parameters_count();
	for ( u32 i = 0; i < parameters_count; ++i )
	{
		action_parameter const* const parameter = goal_for_specification->get_parameter( i );
		specific_problem.fill_parameter_targets( parameter );
		if ( parameter->get_type() != parameter_type_owner && specific_problem.has_no_targets( parameter ) )
			return							false;
	}
	
	indices_type offsets					( parameters_count, 0 );
	bool offsets_incremented				= true;
	
	while ( offsets_incremented )
	{
		indices_type						parameter_ids;
		for ( u32 j = 0; j < parameters_count; ++j )
		{
			parameter_ids.push_back			( offsets[j] );

			if ( j == parameters_count - 1 )
				offsets_incremented			= increment_targets( offsets, goal_for_specification, specific_problem );
		}

		if ( parameters_count && !are_parameters_suitable( specific_problem, goal_for_specification, parameter_ids ) )
			continue;

		fill_problem						( specific_problem, goal_for_specification, parameter_ids );
		
		if ( m_planner->build_plan( search_service, specific_problem, goals_captions[goal_for_specification->get_type()], true ) )
			return							true;

		if ( !parameters_count )
			break;
	}

	return									false;
}

} // namespace planning
} // namespace ai
} // namespace xray
