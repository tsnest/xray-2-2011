////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_INLINE_H_INCLUDED
#define BASE_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline base_lexeme::base_lexeme				( mutable_buffer& buffer, bool const cloned ) :
	m_buffer		( &buffer ),
	m_cloned		( cloned )
{
}

inline base_lexeme::base_lexeme				( base_lexeme const& other, bool )
{
	*this			= other;
	m_cloned		= true;
}

inline mutable_buffer& base_lexeme::buffer	( ) const
{
	ASSERT			( m_buffer );
	return			*m_buffer;
}

template < typename T >
inline T* base_lexeme::cloned_in_buffer		( )
{
	if ( m_cloned )
		return		static_cast<T*>( this );

	mutable_buffer& buffer = *m_buffer;
	T* const result	= static_cast<T*>( buffer.c_ptr( ) );
	buffer			+= sizeof(T);
	return			new(result) T( *static_cast<T*>(this), true );
}

inline bool	base_lexeme::is_cloned			( ) const
{
	return			m_cloned;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BASE_LEXEME_INLINE_H_INCLUDED