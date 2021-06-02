////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "specified_action.h"
#include "generalized_action.h"
#include "object_instance.h"
#include "specified_problem.h"
#include "pddl_problem.h"
#include "action_instance.h"

namespace xray {
namespace ai {
namespace planning {

specified_action::specified_action	( ) :
	m_prototype						( 0 )
{
}

bool specified_action::are_instances_indices_equal	( pddl_world_state_property_impl const& first_property, pddl_world_state_property_impl const& second_property )
{
	if ( first_property.get_indices_count() != second_property.get_indices_count() )
		return						false;

	for ( u32 i = 0; i < first_property.get_indices_count(); ++i )
		if ( first_property.get_index( i ) != second_property.get_index( i ) )
			return					false;
	
	return							true;
}

bool specified_action::can_be_specified	( generalized_action const& prototype, specified_problem const& problem )
{
	for ( u32 j = 0; j < prototype.get_parameters_count(); ++j )
		if ( problem.get_count_of_objects_by_type( prototype.get_parameter_type( j ) ) == 0 )
			return false;
	
	return true;
}

bool specified_action::specify_property	( pddl_world_state_property_impl const& property_prototype, world_state_type& state )
{
	pddl_world_state_property_impl		specified_property( property_prototype.get_predicate(), property_prototype.get_result() );
	for ( u32 i = 0; i < property_prototype.get_indices_count(); ++i )
		specified_property.add_index( m_parameters_instances[property_prototype.get_index( i )] );

	for ( u32 i = 0; i < state.size(); ++i )
	{
		if ( specified_property.get_predicate() == state[i].get_predicate() &&
			 are_instances_indices_equal( specified_property, state[i] ) )
		{
// 			if ( specified_property.get_value() != state[i].get_value() )
// 			{
				LOG_WARNING			( "attempt to add already set world state property, specification is skipping");
				return				false;
// 			}
// 			else
// 				continue;
		}
	}
	
	state.push_back					( specified_property );
	return							true;
}

bool specified_action::specify	( generalized_action const& prototype, instances_type const& parameters )
{
	R_ASSERT					( parameters.size() == prototype.get_parameters_count(), "improper parameters count" );
	m_prototype					= &prototype;
	m_parameters_instances		= parameters;
	
	for ( u32 i = 0; i < prototype.get_preconditions_count(); ++i )
		if ( !specify_property( prototype.get_precondition( i ), m_preconditions ) )
			return				false;
	
	for ( u32 i = 0; i < prototype.get_effects_count(); ++i )
		if ( !specify_property( prototype.get_effect( i ), m_effects ) )
			return				false;

	return						true;
}

void specified_action::debug_output	( specified_problem const& problem ) const
{
	if ( !m_prototype )
	{
		LOG_WARNING				( "attempt to manipulate with unspecified action" );
		return;
	}

	fixed_string512				output_text( m_prototype->get_caption() );
	output_text.append			( " (");
	for ( u32 i = 0; i < m_parameters_instances.size(); ++i )
	{
		object_instance const* const object = problem.get_object_by_type_and_index(
														m_prototype->get_parameter_type( i ),					
														m_parameters_instances[i]
													  );
		R_ASSERT				( object, "invalid object type and/or id" );
		output_text.append		( object->get_caption() );
		output_text.append		( i == m_parameters_instances.size() - 1 ? ")" : ", " );
	}
	if ( m_parameters_instances.empty() )
		output_text.append		( ")" );

	LOG_INFO					( output_text.c_str() );
}

pddl_world_state_property_impl const& specified_action::get_precondition	( u32 const index ) const
{
	return						m_preconditions[index];
}

pddl_world_state_property_impl const& specified_action::get_effect		( u32 const index ) const
{
	return						m_effects[index];
}

u32 specified_action::get_cost	( ) const
{
	R_ASSERT						( m_prototype, "attempt to manipulate with unspecified action" );
	return							m_prototype->get_cost( );
}

void specified_action::add_this_to_plan	( plan_type& plan, specified_problem const& specific_problem ) const
{
	if ( !m_prototype )
	{
		LOG_WARNING						( "attempt to manipulate with unspecified action" );
		return;
	}

	action_instance const* const action	= specific_problem.get_problem().get_action_instance( m_prototype->get_type() );
	if ( !action )	
	{
		LOG_WARNING						( "action instance wasn't found in problem" );
		return;
	}
	
	plan_item							new_plan_item;
	new_plan_item.action				= action;
	
	property_values_type				objects;
	for ( u32 i = 0; i < m_parameters_instances.size(); ++i )
	{
		object_instance const* const object = specific_problem.get_object_by_type_and_index(
														m_prototype->get_parameter_type( i ),					
														m_parameters_instances[i]
													  );
		R_ASSERT						( object, "invalid object type and/or id" );
		new_plan_item.parameters.push_back( object->get_instance() );
	}
	plan.push_back						( new_plan_item );
}

} // namespace planning
} // namespace ai
} // namespace xray