////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "binary_operation_lexeme.h"
#include <xray/memory_stack_allocator.h>
#include "and_lexeme.h"
#include "or_lexeme.h"

namespace xray {
namespace ai {
namespace planning {

binary_operation_lexeme::binary_operation_lexeme	( 
		operation_type_enum const operation_type,
		base_lexeme const& left,
		base_lexeme const& right,
		bool const destroy_manually
	) :
	m_operation_type		( operation_type ),
	m_left					( &left ),
	m_right					( &right ),
	m_destroy_manually		( destroy_manually )
{
}

void binary_operation_lexeme::invert_value	( ) const
{
	R_ASSERT								( sizeof( and_lexeme ) == sizeof( or_lexeme ) );
	R_ASSERT								( pvoid( ( and_lexeme* ) 0 ) == pvoid( ( binary_operation_lexeme* )( ( and_lexeme* ) 0 ) ) );
	
	switch ( m_operation_type )
	{
		case operation_type_and:
			m_operation_type				= operation_type_or;
		break;

		case operation_type_or:
			m_operation_type				= operation_type_and;
		break;

		default:
			NODEFAULT						( );
	}
	m_left->invert_value					( );
	m_right->invert_value					( );
}

void binary_operation_lexeme::destroy		( ) const
{
	m_left->destroy							( );
	m_right->destroy						( );
}

void binary_operation_lexeme::add_to_target_world_state	( pddl_problem& problem ) const
{
	R_ASSERT								( m_operation_type != operation_type_or, "|| can't be used for target world state" );
	if ( m_operation_type == operation_type_or )
		return;

	m_left->add_to_target_world_state		( problem );
	m_right->add_to_target_world_state		( problem );
}

} // namespace planning
} // namespace ai
} // namespace xray