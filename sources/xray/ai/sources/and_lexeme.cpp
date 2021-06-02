////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "and_lexeme.h"
#include <xray/memory_stack_allocator.h>

namespace xray {
namespace ai {
namespace planning {

and_lexeme::and_lexeme			( base_lexeme const& left, base_lexeme const& right, bool const destroy_manually ) :
	binary_operation_lexeme		( operation_type_and, left, right, destroy_manually )
{
}

base_lexeme const& and_lexeme::expand_brackets	( memory::stack_allocator& allocator) const
{
	return m_left->expand_brackets( allocator ).expand_brackets( allocator, m_right->expand_brackets( allocator ) );
}

base_lexeme const& and_lexeme::expand_brackets	( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return right.generate_permutations			( allocator, *this );
}

base_lexeme const& and_lexeme::generate_permutations( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return *XRAY_NEW_IMPL						( allocator, and_lexeme )( left, *this, true );
}

void and_lexeme::destroy						( ) const
{
	binary_operation_lexeme::destroy			( );

	if ( m_destroy_manually )
		this->~and_lexeme						( );
}

u32 and_lexeme::memory_size_for_brackets_expansion	( ) const
{
	return										m_left->memory_size_for_brackets_expansion() +
												m_right->memory_size_for_brackets_expansion() +
												m_left->memory_size_for_brackets_expansion( *m_right );
}

u32 and_lexeme::memory_size_for_brackets_expansion	( base_lexeme const& right ) const
{
	return										m_left->memory_size_for_permutations_generation( right ) +
												m_right->memory_size_for_permutations_generation( right );
}

u32 and_lexeme::memory_size_for_permutations_generation	( base_lexeme const& left ) const
{
	XRAY_UNREFERENCED_PARAMETER					( left );
	return										sizeof( and_lexeme );
}

void and_lexeme::add_to_preconditions			( generalized_action& action ) const
{
	m_left->add_to_preconditions				( action );
	m_right->add_to_preconditions				( action );
}

void and_lexeme::add_to_effects					( generalized_action& action ) const
{
	m_left->add_to_effects						( action );
	m_right->add_to_effects						( action );
}

} // namespace planning
} // namespace ai
} // namespace xray