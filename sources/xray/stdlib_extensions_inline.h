////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STDLIB_EXTENSIONS_INLINE_H_INCLUDED
#define XRAY_STDLIB_EXTENSIONS_INLINE_H_INCLUDED

namespace xray {

inline int	sprintf		( pstr buffer, u32 const buffer_size, pcstr const format, ... )
{
	va_list				mark;
	va_start			( mark, format );
	return				vsprintf( buffer, buffer_size, format, mark );
}

template <int Size>
inline int	sprintf		( char (&buffer)[Size], pcstr const format, ... )
{
	va_list				mark;
	va_start			( mark, format );
	return				vsprintf( &buffer[0], Size, format, mark );
}

inline pstr	strlwr		( pstr source, u32 const count )
{
#ifdef _MSC_VER
	errno_t const error	= _strlwr_s( source, count );
	R_ASSERT_U			( !error, "cannot lowercase string \"%s\" (%d)", source, count );
#else // #ifdef _MSC_VER
	for ( pstr i=source; *i; ++i )
		*i				= tolower( *i );
#endif // #ifdef _MSC_VER
	return				source;
}

template < int Count >
inline pstr strlwr ( char (&string)[Count] )
{
	return				strlwr( string, Count*sizeof(char) );
}

inline pstr	strupr		( pstr source, u32 const count )
{
#ifdef _MSC_VER
	errno_t const error	= _strupr_s( source, count );
	R_ASSERT_U			( !error, "cannot uppercase string \"%s\" (%d)", source, count );
#else // #ifdef _MSC_VER
	for ( pstr i=source; *i; ++i )
		*i				= toupper( *i );
#endif // #ifdef _MSC_VER
	return				source;
}

template < int Count >
inline pstr strupr		( char (&string)[Count] )
{
	return				strupr( string, Count*sizeof(char) );
}

inline s64 fseek64		( FILE* const file, u64 const offset, u32 origin )
{
#ifdef _MSC_VER
	return				_lseeki64( _fileno(file), offset, origin );
#else // #ifdef _MSC_VER
	return				lseek64( fileno(file), offset, origin );
#endif // #ifdef _MSC_VER
}

inline int unlink		( pcstr const file_name )
{
#ifdef _MSC_VER
	return				::_unlink( file_name );
#else // #ifdef _MSC_VER
	return				::unlink( file_name );
#endif // #ifdef _MSC_VER
}

} // namespace xray

#endif // #ifndef XRAY_STDLIB_EXTENSIONS_INLINE_H_INCLUDED