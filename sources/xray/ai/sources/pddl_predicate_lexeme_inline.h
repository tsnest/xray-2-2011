////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PREDICATE_LEXEME_INLINE_H_INCLUDED
#define PDDL_PREDICATE_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline pddl_predicate_lexeme::pddl_predicate_lexeme	( pddl_predicate const* required_predicate ) :
	m_predicate										( required_predicate ),
	m_value											( true ),
	m_destroy_manually								( false )
{
}

inline and_lexeme operator &&	( base_lexeme const& left, base_lexeme const& right )
{
	return and_lexeme			( left, right, false );
}

inline or_lexeme operator ||	( base_lexeme const& left, base_lexeme const& right )
{
	return or_lexeme			( left, right, false );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_PREDICATE_LEXEME_INLINE_H_INCLUDED