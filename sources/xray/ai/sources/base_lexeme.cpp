////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "base_lexeme.h"
#include "generalized_action.h"
#include <xray/memory_stack_allocator.h>
#include "specified_problem.h"

namespace xray {
namespace ai {
namespace planning {

base_lexeme::function_pointers const base_lexeme::s_or_function_pointers = base_lexeme::function_pointers( base_lexeme::operation_type_or );
base_lexeme::function_pointers const base_lexeme::s_and_function_pointers = base_lexeme::function_pointers( base_lexeme::operation_type_and );
base_lexeme::function_pointers const base_lexeme::s_predicate_function_pointers = base_lexeme::function_pointers( base_lexeme::operation_type_predicate );

base_lexeme::function_pointers::function_pointers	( operation_type_enum const operation_type )
{
	switch ( operation_type )
	{
		case operation_type_and:
		{
			m_value_invertor		= &base_lexeme::invert_value_as_and;
			m_operands_counter		= &base_lexeme::count_operands_as_and;
			m_brackets_opener1		= &base_lexeme::expand_brackets_as_and;
			m_brackets_opener2		= &base_lexeme::expand_brackets_as_and;
			m_generator				= &base_lexeme::generate_permutations_as_and;
			m_world_state_filler	= &base_lexeme::add_to_target_world_state_as_and;
			m_preconditions_filler	= &base_lexeme::add_to_preconditions_as_and;
			m_effects_filler		= &base_lexeme::add_to_effects_as_and;
		}
		break;

		case operation_type_or:
		{
			m_value_invertor		= &base_lexeme::invert_value_as_or;
			m_operands_counter		= &base_lexeme::count_operands_as_or;
			m_brackets_opener1		= &base_lexeme::expand_brackets_as_or;
			m_brackets_opener2		= &base_lexeme::expand_brackets_as_or;
			m_generator				= &base_lexeme::generate_permutations_as_or;
			m_world_state_filler	= &base_lexeme::add_to_target_world_state_as_or;
			m_preconditions_filler	= &base_lexeme::add_to_preconditions_as_or;
			m_effects_filler		= &base_lexeme::add_to_effects_as_or;
		}
		break;

		case operation_type_predicate:
		{
			m_value_invertor		= &base_lexeme::invert_value_as_predicate;
			m_operands_counter		= &base_lexeme::count_operands_as_predicate;
			m_brackets_opener1		= &base_lexeme::expand_brackets_as_predicate;
			m_brackets_opener2		= &base_lexeme::expand_brackets_as_predicate;
			m_generator				= &base_lexeme::generate_permutations_as_predicate;
			m_world_state_filler	= &base_lexeme::add_to_target_world_state_as_predicate;
			m_preconditions_filler	= &base_lexeme::add_to_preconditions_as_predicate;
			m_effects_filler		= &base_lexeme::add_to_effects_as_predicate;
		}
		break;

		default:
			NODEFAULT				( );
	}
}

void base_lexeme::reset_pointers	( ) const
{
	m_function_pointers				= m_operation_type == operation_type_or ? &s_or_function_pointers :
									( m_operation_type == operation_type_and ? &s_and_function_pointers :
									&s_predicate_function_pointers);
}

void base_lexeme::invert_value						( ) const
{
	( this->*m_function_pointers->m_value_invertor )( );
}

base_lexeme_ptr base_lexeme::expand_brackets		( memory::stack_allocator& allocator ) const
{
	return ( this->*m_function_pointers->m_brackets_opener1 )( allocator );
}

base_lexeme_ptr base_lexeme::expand_brackets		( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return ( this->*m_function_pointers->m_brackets_opener2 )( allocator, right );
}

base_lexeme_ptr base_lexeme::generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return ( this->*m_function_pointers->m_generator )( allocator, left );
}

operands_calculator base_lexeme::count_operands_for_brackets_expansion	( ) const
{
	return ( this->*m_function_pointers->m_operands_counter )( );
}

void base_lexeme::increment_counter	( ) const
{
	if ( !m_destroy_manually )
		return;

	++m_counter;
}

void base_lexeme::decrement_counter	( ) const
{
	if ( !m_destroy_manually )
		return;

	R_ASSERT						( m_counter );
	
	if ( !--m_counter )
		this->~base_lexeme			( );
}

void base_lexeme::add_to_target_world_state	( specified_problem& problem, u32& offset ) const
{
	( this->*m_function_pointers->m_world_state_filler )( problem, offset );
}

void base_lexeme::add_to_preconditions		( generalized_action& action ) const
{
	( this->*m_function_pointers->m_preconditions_filler )( action );
}

void base_lexeme::add_to_effects			( generalized_action& action ) const
{
	( this->*m_function_pointers->m_effects_filler )( action );
}

base_lexeme_ptr base_lexeme::expand_brackets_as_and	( memory::stack_allocator& allocator ) const
{
	base_lexeme_ptr left		= m_left->expand_brackets( allocator );
	base_lexeme_ptr right		= m_right->expand_brackets( allocator );
	return						left->expand_brackets( allocator, *right );
}

base_lexeme_ptr base_lexeme::expand_brackets_as_or	( memory::stack_allocator& allocator ) const
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
	return						this;
}

base_lexeme_ptr base_lexeme::expand_brackets_as_predicate( memory::stack_allocator& allocator ) const
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
	return						this;
}

base_lexeme_ptr base_lexeme::expand_brackets_as_and	( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return						right.generate_permutations( allocator, *this );
}

base_lexeme_ptr base_lexeme::expand_brackets_as_or	( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return						XRAY_NEW_IMPL( allocator, base_lexeme )(
									operation_type_or,
									*m_left->expand_brackets( allocator, right ),
									*m_right->expand_brackets( allocator, right ),
									true
								);
}

base_lexeme_ptr base_lexeme::expand_brackets_as_predicate( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return						right.generate_permutations( allocator, *this );
}

base_lexeme_ptr base_lexeme::generate_permutations_as_and	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return						XRAY_NEW_IMPL( allocator, base_lexeme )( operation_type_and, left, *this, true );
}

base_lexeme_ptr base_lexeme::generate_permutations_as_or	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return						XRAY_NEW_IMPL( allocator, base_lexeme )(
									operation_type_or,
									*m_left->generate_permutations( allocator, left ),
									*m_right->generate_permutations( allocator, left ),
									true
								);
}

base_lexeme_ptr base_lexeme::generate_permutations_as_predicate( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return						XRAY_NEW_IMPL( allocator, base_lexeme )( operation_type_and, left, *this, true );
}

void base_lexeme::invert_value_as_and	( ) const
{
	m_left->invert_value				( );
	m_right->invert_value				( );

	m_operation_type					= operation_type_or;
	reset_pointers						( );
}

void base_lexeme::invert_value_as_or	( ) const
{
	m_left->invert_value				( );
	m_right->invert_value				( );

	m_operation_type					= operation_type_and;
	reset_pointers						( );
}

void base_lexeme::invert_value_as_predicate	( ) const
{
	m_value								= !m_value;
}

operands_calculator base_lexeme::count_operands_as_and	( ) const
{
	operands_calculator const& left		= m_left->count_operands_for_brackets_expansion();
	operands_calculator const& right	= m_right->count_operands_for_brackets_expansion();
	return operands_calculator			(
			left.operands_count * right.operands_count,
			left.operands_count * right.operands_count + left.and_count + right.and_count,
			left.or_count + right.or_count + ( left.operands_count * right.operands_count - 1 )
		);
}

operands_calculator base_lexeme::count_operands_as_or	( ) const
{
	return								m_left->count_operands_for_brackets_expansion() + 
										m_right->count_operands_for_brackets_expansion();
}

operands_calculator base_lexeme::count_operands_as_predicate	( ) const
{
	return								operands_calculator( 1, 0, 0 );
}

void base_lexeme::add_to_target_world_state_as_and	( specified_problem& problem, u32& offset ) const
{
	m_left->add_to_target_world_state	( problem, offset );
	m_right->add_to_target_world_state	( problem, offset );
}

void base_lexeme::add_to_target_world_state_as_or	( specified_problem& problem, u32& offset ) const
{
	m_left->add_to_target_world_state	( problem, offset );
	problem.add_target_state_offset		( offset );
	m_right->add_to_target_world_state	( problem, offset );
}

void base_lexeme::add_to_target_world_state_as_predicate( specified_problem& problem, u32& offset ) const
{
	XRAY_UNREFERENCED_PARAMETERS		( &problem, &offset );
	NOT_IMPLEMENTED						( );
}

void base_lexeme::add_to_preconditions_as_and	( generalized_action& action ) const
{
	m_left->add_to_preconditions		( action );
	m_right->add_to_preconditions		( action );
}

void base_lexeme::add_to_preconditions_as_or	( generalized_action& action ) const
{
	m_left->add_to_preconditions		( action );
	
	generalized_action* clone			= action.clone();
	R_ASSERT							( clone );
	m_right->add_to_preconditions		( *clone );
}

void base_lexeme::add_to_preconditions_as_predicate	( generalized_action& action ) const
{
	XRAY_UNREFERENCED_PARAMETER			( action );
	NOT_IMPLEMENTED						( );
}

void base_lexeme::add_to_effects_as_and	( generalized_action& action ) const
{
	m_left->add_to_effects				( action );
	m_right->add_to_effects				( action );
}

void base_lexeme::add_to_effects_as_or	( generalized_action& action ) const
{
	XRAY_UNREFERENCED_PARAMETER			( action );
	UNREACHABLE_CODE					( );
}

void base_lexeme::add_to_effects_as_predicate	( generalized_action& action ) const
{
	XRAY_UNREFERENCED_PARAMETER			( action );
	NOT_IMPLEMENTED						( );
}

} // namespace planning
} // namespace ai
} // namespace xray