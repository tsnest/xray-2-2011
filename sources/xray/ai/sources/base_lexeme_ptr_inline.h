////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_PTR_INLINE_H_INCLUDED
#define BASE_LEXEME_PTR_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline base_lexeme_ptr::base_lexeme_ptr	( ) :
	m_lexeme							( 0 )
{
}

inline base_lexeme_ptr& base_lexeme_ptr::operator =	( base_lexeme_ptr const& other )
{
	base_lexeme_ptr( other ).swap		( *this );
	return								*this;
}

inline void base_lexeme_ptr::swap		( base_lexeme_ptr& other )
{
	std::swap							( m_lexeme, other.m_lexeme );
}

inline base_lexeme const* base_lexeme_ptr::operator ->	( ) const
{
	return								m_lexeme;
}

inline base_lexeme const& base_lexeme_ptr::operator *	( ) const
{
	R_ASSERT							( m_lexeme );
	return								*m_lexeme;
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef BASE_LEXEME_PTR_INLINE_H_INCLUDED