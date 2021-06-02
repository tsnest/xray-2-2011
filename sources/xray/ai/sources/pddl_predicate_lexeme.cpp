////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pddl_predicate_lexeme.h"
#include <xray/memory_stack_allocator.h>

namespace xray {
namespace ai {
namespace planning {

void pddl_predicate_lexeme::invert_value	( ) const
{
	m_value									= !m_value;
}

// base_lexeme const& pddl_predicate_lexeme::expand_brackets	( memory::stack_allocator& allocator ) const
// {
// 	pddl_predicate_lexeme* const result		= XRAY_NEW_IMPL( allocator, pddl_predicate_lexeme )( m_predicate );
// 	result->m_value							= m_value;
// 	result->m_destroy_manually				= true;
// 	return									*result;
// }

base_lexeme const& pddl_predicate_lexeme::expand_brackets	( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return right.generate_permutations		( allocator, *this );
}

void pddl_predicate_lexeme::destroy			( ) const
{
	if ( m_destroy_manually )
		this->~pddl_predicate_lexeme		( );
}

u32 pddl_predicate_lexeme::memory_size_for_brackets_expansion	( ) const
{
	return									sizeof( *this );
}

u32 pddl_predicate_lexeme::memory_size_for_brackets_expansion	( base_lexeme const& right ) const
{
	return									right.memory_size_for_permutations_generation( *this );
}

base_lexeme const& pddl_predicate_lexeme::generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return									*XRAY_NEW_IMPL( allocator, and_lexeme )( left, *this, true );
}

u32	pddl_predicate_lexeme::memory_size_for_permutations_generation	( base_lexeme const& left ) const
{
	XRAY_UNREFERENCED_PARAMETER				( left );
	return									sizeof( and_lexeme );
}

} // namespace planning
} // namespace ai
} // namespace xray