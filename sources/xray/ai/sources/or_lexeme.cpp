////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "or_lexeme.h"
#include <xray/memory_stack_allocator.h>
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

or_lexeme::or_lexeme		( base_lexeme const& left, base_lexeme const& right, bool const destroy_manually ) :
	binary_operation_lexeme	( operation_type_or, left, right, destroy_manually )
{
}

base_lexeme const& or_lexeme::expand_brackets	( memory::stack_allocator& allocator ) const
{
	return *XRAY_NEW_IMPL( allocator, or_lexeme )(
				m_left->expand_brackets( allocator ),
				m_right->expand_brackets( allocator ),
				true
			);
}

base_lexeme const& or_lexeme::expand_brackets	( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	m_left					= &m_left->expand_brackets( allocator, right );
	m_right					= &m_right->expand_brackets( allocator, right );
	return					*this;
}

base_lexeme const& or_lexeme::generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return *XRAY_NEW_IMPL( allocator, or_lexeme )	(
				m_left->generate_permutations( allocator, left ),
				m_right->generate_permutations( allocator, left ),
				true
			);
}

void or_lexeme::destroy					( ) const
{
	binary_operation_lexeme::destroy	( );

	if ( m_destroy_manually )
		this->~or_lexeme				( );
}

u32 or_lexeme::memory_size_for_brackets_expansion	( ) const
{
	return								sizeof( *this ) +
										m_left->memory_size_for_brackets_expansion() +
										m_right->memory_size_for_brackets_expansion();
}

u32 or_lexeme::memory_size_for_brackets_expansion	( base_lexeme const& right ) const
{
	return								m_left->memory_size_for_brackets_expansion( right ) +
										m_right->memory_size_for_brackets_expansion( right );
}

u32 or_lexeme::memory_size_for_permutations_generation	( base_lexeme const& left ) const
{
	return								sizeof( *this ) +
										m_left->memory_size_for_permutations_generation( left ) +
										m_right->memory_size_for_permutations_generation( left );
}

void or_lexeme::add_to_preconditions	( generalized_action& action ) const
{
	m_left->add_to_preconditions		( action );
	
	generalized_action* clone			= action.clone();
	R_ASSERT							( clone );
	m_right->add_to_preconditions		( *clone );
}

void or_lexeme::add_to_effects			( generalized_action& action ) const
{
	XRAY_UNREFERENCED_PARAMETER			( action );
	UNREACHABLE_CODE					( "|| can't be used for action effects setting" );
}

} // namespace planning
} // namespace ai
} // namespace xray