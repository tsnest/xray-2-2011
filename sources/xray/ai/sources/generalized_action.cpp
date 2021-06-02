////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "generalized_action.h"
#include "base_lexeme.h"
#include <xray/memory_stack_allocator.h>

namespace xray {
namespace ai {
namespace planning {

generalized_action::~generalized_action		( )
{
	for ( u32 i = 0; i < m_clones.size(); ++i )
		DELETE								( m_clones[i] );
}
	
void generalized_action::set_preconditions	( base_lexeme const& target_expression )
{
	memory::stack_allocator					allocator;
	u32 const memory_size					= target_expression.memory_size_for_brackets_expansion();
	allocator.initialize					( ALLOCA( memory_size ), memory_size, "world state expression" );
	base_lexeme_ptr const new_expression	= target_expression.expand_brackets( allocator );
	(*new_expression).add_to_preconditions	( *this );
}

void generalized_action::set_effects		( base_lexeme const& target_expression )
{
	memory::stack_allocator					allocator;
	u32 const memory_size					= target_expression.memory_size_for_brackets_expansion();
	allocator.initialize					( ALLOCA( memory_size ), memory_size, "world state expression" );
	
	base_lexeme_ptr const new_expression	= target_expression.expand_brackets( allocator );
	(*new_expression).add_to_effects		( *this );
	for ( u32 j = 0; j < m_clones.size(); ++j )
		(*new_expression).add_to_effects	( *m_clones[j] );
}

generalized_action* generalized_action::clone	( )
{
	if ( m_parent )
		return								m_parent->clone();
	else
	{
		generalized_action* copy			= NEW( generalized_action )( m_domain, m_type, m_caption.c_str(), m_cost );
		copy->m_effects						= m_effects;
		copy->m_parent						= this;
		m_clones.push_back					( copy );
		return								copy;
	}
}

} // namespace planning
} // namespace ai
} // namespace xray