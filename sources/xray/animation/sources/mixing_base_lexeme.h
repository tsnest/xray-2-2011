////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_H_INCLUDED
#define BASE_LEXEME_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

class expression;

class base_lexeme {
public:
	explicit inline	base_lexeme		( mutable_buffer& buffer, bool const cloned = false );
	inline			base_lexeme		( base_lexeme const& other, bool );
	inline mutable_buffer& buffer	( ) const;

protected:
	template < typename T >
	inline	T*		cloned_in_buffer( );
	inline	bool	is_cloned		( ) const;

private:
	mutable_buffer*	m_buffer;
	bool			m_cloned;
}; // class base_lexeme

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_base_lexeme_inline.h"

#endif // #ifndef BASE_LEXEME_H_INCLUDED