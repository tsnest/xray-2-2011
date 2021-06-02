////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_PTR_H_INCLUDED
#define BASE_LEXEME_PTR_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

class base_lexeme;

class base_lexeme_ptr
{
public:
	inline					base_lexeme_ptr	( );
							base_lexeme_ptr	( base_lexeme const* const lexeme );
							base_lexeme_ptr	( base_lexeme_ptr const& other );

							~base_lexeme_ptr( );

	inline	base_lexeme_ptr&	operator =	( base_lexeme_ptr const& other );

	inline	void				swap		( base_lexeme_ptr& other );

	inline base_lexeme const*	operator ->	( ) const;
	inline base_lexeme const&	operator *	( ) const;

private:
	base_lexeme const*			m_lexeme;
}; // class base_lexeme_ptr

} // namespace planning
} // namespace ai
} // namespace xray

#include "base_lexeme_ptr_inline.h"

#endif // #ifndef BASE_LEXEME_PTR_H_INCLUDED