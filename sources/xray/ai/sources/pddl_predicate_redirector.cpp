////////////////////////////////////////////////////////////////////////////
//	Created		: 01.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pddl_predicate_redirector.h"
#include "pddl_predicate.h"
#include "specified_problem.h"

namespace xray {
namespace ai {
namespace planning {

pddl_predicate_redirector::pddl_predicate_redirector(
		pcstr caption,
		specified_problem const& problem,
		property_id_type const& id
	) :
	super											( caption ),
	m_problem										( problem ),
	m_id											( id )
{
}

property_value_type pddl_predicate_redirector::value( )
{
	pddl_predicate const* predicate					= m_problem.get_lower_bound_predicate( m_id );
	R_ASSERT										( predicate, "invalid offset for predicate" );
	u32 const predicate_offset						= m_problem.get_predicate_offset( predicate->get_type() );
	R_ASSERT										( predicate_offset != u32(-1), "invalid calculated world state size" );
	u32 const difference							= m_id - predicate_offset;
	
	u32 const parameters_count						= predicate->get_parameters_count();
	property_values_type							values( parameters_count, 0 );
	u32 combinations_count							= 1;

	for ( u32 i = 0; i < parameters_count; ++i )
	{
		u32 const other_side_i						= parameters_count - i - 1;
		u32 const count_of_type						= m_problem.get_count_of_objects_by_type( predicate->get_parameter( other_side_i ) );
		R_ASSERT									( count_of_type > 0, "parametrized predicate with no corresponding objects in problem, error in pddl_domain::get_world_state_size()!" );
		u32 const object_index						= ( difference / combinations_count ) % count_of_type;
		object_instance const* object				= m_problem.get_object_by_type_and_index( predicate->get_parameter( other_side_i ), object_index );	
		values[other_side_i]						= object->get_instance();
		combinations_count							*= count_of_type;
	}

	return											predicate->evaluate_value( values );
}

} // namespace planning
} // namespace ai
} // namespace xray