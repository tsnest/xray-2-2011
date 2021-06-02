////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "predicate.h"
#include "specified_problem.h"
#include "pddl_domain.h"
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

void predicate::add_to_target_world_state_as_predicate	( specified_problem& problem, u32& offset ) const
{
	pddl_predicate const* const target_predicate = problem.get_domain()[m_predicate_id];
	R_ASSERT							( target_predicate, "predicate is absent in domain" );
	u32 const parameters_count			= m_parameters.size();
	R_ASSERT							( target_predicate->get_parameters_count() == parameters_count, "invalid objects count" );
	
	pddl_world_state_property_impl		property_to_be_added( target_predicate, m_value );
	for ( u32 i = 0; i < parameters_count; ++i )
	{
		u32 const index					= problem.get_object_index( target_predicate->get_parameter( i ), m_parameters[i].instance );
		R_ASSERT						( index != u32(-1), "invalid object or its type, or it's absent in current problem" );
		property_to_be_added.add_index	( index );
	}
	
	for ( u32 i = 0; i < property_to_be_added.get_indices_count(); ++i )
	{
		object_instance const* const object = problem.get_object_by_type_and_index(
												property_to_be_added.get_predicate()->get_parameter( i ),
												property_to_be_added.get_index( i )
											  ); 
		R_ASSERT						( object, "improper type of predicate parameter" );
	}
	
	problem.add_target_property			( property_to_be_added );
	++offset;
}

static pddl_world_state_property_impl create_property	(
		u32 const predicate_id,
		generalized_action& action,
		bool const value,
		expression_parameters_type const& parameters_source
	)
{
	pddl_predicate const* const target_predicate = action.get_domain()[predicate_id];
	R_ASSERT							( target_predicate, "predicate is absent in domain" );
	u32 const parameters_count			= parameters_source.size();
	R_ASSERT							( target_predicate->get_parameters_count() == parameters_count, "invalid objects count" );
	
	pddl_world_state_property_impl		new_property( target_predicate, value );
	for ( u32 i = 0; i < parameters_count; ++i )
		new_property.add_index			( parameters_source[i].index );

	return								new_property;
}

void predicate::add_to_preconditions_as_predicate	( generalized_action& action ) const
{
	action.add_precondition				( create_property( m_predicate_id, action, m_value, m_parameters ) );
}

void predicate::add_to_effects_as_predicate			( generalized_action& action ) const
{
	action.add_effect					( create_property( m_predicate_id, action, m_value, m_parameters ) );
}

} // namespace planning
} // namespace ai
} // namespace xray