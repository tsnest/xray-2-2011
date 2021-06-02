////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SHARED_STRING_INLINE_H_INCLUDED
#define XRAY_SHARED_STRING_INLINE_H_INCLUDED

inline xray::shared_string::shared_string					( )
{
}

inline xray::shared_string::shared_string					( shared_string const& other ) :
	m_pointer	( other.m_pointer )
{
}

inline pcstr xray::shared_string::c_str						( ) const
{
	if ( m_pointer )
		return	( m_pointer->value( ) );

	return		( 0 );
}

inline bool xray::shared_string::operator!					( ) const
{
	return		( !m_pointer );
}

inline bool xray::shared_string::operator<					( shared_string const& other ) const
{
	return		( m_pointer < other.m_pointer );
}

inline bool xray::shared_string::operator==					( shared_string const& other ) const
{
	return		( m_pointer == other.m_pointer );
}

inline bool xray::shared_string::operator!=					( shared_string const& other ) const
{
	return		( m_pointer != other.m_pointer );
}

inline u32 xray::shared_string::length						( ) const
{
	R_ASSERT	( m_pointer );
	return		( m_pointer->length() );
}

inline u32 xray::shared_string::size						( ) const
{
	if ( !m_pointer )
		return	( 0 );

	return		( length( ) );
}

inline xray::shared_string::operator unspecified_bool_type	( ) const
{
	if ( !m_pointer )
		return	( 0 );

	return		( &shared_string::c_str );
}

#endif // #ifndef XRAY_SHARED_STRING_INLINE_H_INCLUDED