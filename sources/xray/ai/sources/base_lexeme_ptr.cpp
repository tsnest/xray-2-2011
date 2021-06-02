////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "base_lexeme_ptr.h"
#include "base_lexeme.h"

namespace xray {
namespace ai {
namespace planning {

base_lexeme_ptr::base_lexeme_ptr	( base_lexeme const* const lexeme ) :
	m_lexeme						( lexeme )
{
	if ( m_lexeme )
		m_lexeme->increment_counter	( );
}

base_lexeme_ptr::base_lexeme_ptr	( base_lexeme_ptr const& other ) :
	m_lexeme						( other.m_lexeme )
{
	if ( m_lexeme )
		m_lexeme->increment_counter	( );
}

base_lexeme_ptr::~base_lexeme_ptr	( )
{
	if ( !m_lexeme )
		return;

	m_lexeme->decrement_counter		( );
}

} // namespace planning
} // namespace ai
} // namespace xray