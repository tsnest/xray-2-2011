////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "propositional_symbol.h"
#include <xray/memory_stack_allocator.h>
#include "pddl_world_state_property_impl.h"
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

base_lexeme const& propositional_symbol::expand_brackets	( memory::stack_allocator& allocator ) const
{
	propositional_symbol* const result	= XRAY_NEW_IMPL( allocator, propositional_symbol )( m_predicate );
	result->m_value						= m_value;
	result->m_destroy_manually			= true;
	result->m_parameters_placeholders	= m_parameters_placeholders;
	return								*result;
}

u32 propositional_symbol::memory_size_for_brackets_expansion( ) const
{
	return								sizeof( *this );
}

void propositional_symbol::destroy		( ) const
{
	if ( m_destroy_manually )
		this->~propositional_symbol		( );
}

void propositional_symbol::add_to_target_world_state		( pddl_problem& problem ) const
{
	XRAY_UNREFERENCED_PARAMETER			( problem );
	UNREACHABLE_CODE					( "propositional_symbol can't be used for target world state setting" );
}

void propositional_symbol::add_to_preconditions				( generalized_action& action ) const
{
	pddl_world_state_property_impl		new_property( m_predicate, m_value );
	for ( u32 i = 0; i < m_parameters_placeholders.size(); ++i )
		new_property.add_index			( m_parameters_placeholders[i] );

	action.add_precondition				( new_property );
}

void propositional_symbol::add_to_effects					( generalized_action& action ) const
{
	pddl_world_state_property_impl		new_property( m_predicate, m_value );
	for ( u32 i = 0; i < m_parameters_placeholders.size(); ++i )
		new_property.add_index			( m_parameters_placeholders[i] );

	action.add_effect					( new_property );
}

} // namespace planning
} // namespace ai
} // namespace xray