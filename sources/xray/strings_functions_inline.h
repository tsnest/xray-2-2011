////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STRINGS_FUNCTIONS_INLINE_H_INCLUDED
#define XRAY_STRINGS_FUNCTIONS_INLINE_H_INCLUDED

#if defined(__SNC__) || defined(__GCC__)
	#include <ctype.h>
#endif // #if defined(__SNC__) || defined(__GCC__)

inline u32 xray::strings::length		( pcstr string )
{
	return				( ( u32 )strlen( string ) );
}

inline int xray::strings::compare		( pcstr left, pcstr right )
{
	return				( strcmp( left, right ) );
}

inline int xray::strings::compare_insensitive	( pcstr left, pcstr right )
{
#if defined(_MSC_VER)
	return				( _stricmp( left, right ) );
#elif defined(__SNC__) || defined(__GCC__) // #if defined(_MSC_VER)
	pcstr i = left, j = right;
	for ( ; *i && *j; ++i, ++j ) {
		int const lower_i	= tolower(*i);
		int const lower_j	= tolower(*j);
		if ( lower_i < lower_j )
			return		-1;

		if ( lower_i > lower_j )
			return		1;
	}

	return				(*i < *j) ? -1 : ((*i > *j) ? 1 : 0);
#else // #elif defined(__SNC__) || defined(__GCC__)
#	error define here implementation of stricmp for your platform
#endif // #ifdef _MSC_VER
}

inline pstr xray::strings::copy			( pstr destination, u32 destination_size, pcstr source )
{
#ifdef _MSC_VER
	errno_t const error	= strcpy_s( destination, destination_size, source );
	XRAY_UNREFERENCED_PARAMETER	( error );
#elif defined(__GCC__) // #ifdef _MSC_VER
	strlcpy			( destination, source, destination_size );
#elif defined(__SNC__) // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER	( destination_size );
	strcpy			( destination, source );
#else // #elif defined(__SNC__)
#	error define strings::copy for your platform here
#endif // #ifdef _MSC_VER
	return				( destination );
}

template < int count >
inline pstr xray::strings::copy			( char (&destination)[count], pcstr source )
{
	return				( copy(&destination[0], count, source) );
}

inline pstr xray::strings::copy_n		( pstr const destination, size_t const destination_size, pcstr const source, size_t const count )
{
#ifdef _MSC_VER
	errno_t const error	= strncpy_s( destination, destination_size, source, count );
	XRAY_UNREFERENCED_PARAMETER	( error );
#else // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER	( destination_size );
	strncpy				( destination, source, count );
#endif // #ifdef _MSC_VER
	return				( destination );
}

template < typename allocator_type >
inline pstr xray::strings::duplicate	( allocator_type& allocator, pcstr const string )
{
	u32 const size		= (length( string ) + 1)*sizeof(char);
	pstr const result	= ( char* )XRAY_MALLOC_IMPL(allocator, size, "strings::duplicate" );
	memory::copy		( result, size, string, size );
	return				( result );
}

template <typename predicate_type>
inline bool	xray::strings::iterate_items( pcstr string, u32 length, predicate_type const& predicate, char const separator )
{
	pcstr I				= string;

	pstr const			temp_string	= ( pstr )ALLOCA( ( length + 1 )*sizeof( char ) );
	pstr i				= temp_string;
	pstr j				= i;

	int					index = 0;

	for ( ; *I; ++I, ++i) {
		if (*I != separator) {
			*i			= *I;
			continue;
		}

		*i				= 0;

		if ( !predicate( index++, j, int( i - j ), false ) )
			return		( false );

		j				= i + 1;
	}

	if ( !index )
		return			( predicate( index, string, length, true ) );

	*i					= 0;
	return				( predicate( index, j, int( i - j ), true ) );
}

template <typename predicate_type>
inline bool	xray::strings::iterate_items( pcstr string, predicate_type const& predicate, char const separator )
{
	return				( iterate_items ( string, length( string ), predicate, separator ) );
}

#endif // #ifndef XRAY_STRINGS_FUNCTIONS_INLINE_H_INCLUDED