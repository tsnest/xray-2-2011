////////////////////////////////////////////////////////////////////////////
//	Unit		: stdlib_extensions.h
//	Created		: 10.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright(C) Challenge Solutions(tm) - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STDLIB_EXTENSIONS_H_INCLUDED
#define CS_CORE_STDLIB_EXTENSIONS_H_INCLUDED

#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#	include <io.h>
#	include <stdarg.h>
#elif defined(__SNC__) // #ifdef _MSC_VER
#	include <ctype.h>
#	include <unistd.h>
#endif // #ifdef _MSC_VER

namespace cs {

inline int	vsnprintf	( pstr const destination, u32 const destination_size, u32 const max_count, pcstr const format, va_list const args )
{
#ifdef _MSC_VER
	return				_vsnprintf_s( destination, destination_size, max_count, format, args );
#else // #ifdef _MSC_VER
	return				::vsnprintf( destination, max_count, format, args );
#endif // #ifdef _MSC_VER
}

template <int Size>
inline int	vsnprintf	( char (&destination)[Size], u32 const max_count, pcstr const format, va_list const args )
{
	return				vsnprintf( &destination[0], Size, max_count, format, args );
}

inline int	vsprintf	( pstr const destination, u32 const destination_size, pcstr const format, va_list const args )
{
#ifdef _MSC_VER
	return				vsprintf_s( destination, destination_size, format, args );
#else // #ifdef _MSC_VER
	return				::vsprintf( destination, format, args );
#endif // #ifdef _MSC_VER
}

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

} // namespace cs

#if defined(CS_SCANF) || defined(CS_SSCANF) || defined(CS_SCANF_STRING)
#	error do not define macros CS_SCANF, CS_SSCANF and CS_SCANF_STRING
#endif // #if defined(CS_SCANF) || defined(CS_SSCANF) || defined(CS_SCANF_STRING)

#if defined(_MSC_VER)
#	define CS_SCANF( format, ... )					scanf_s( format, ##__VA_ARGS__ )
#	define CS_SSCANF( string, format, ... )			sscanf_s( string, format, ##__VA_ARGS__ )
#	define CS_SCANF_STRING( string, buffer_size )	(string), (buffer_size)
#elif defined(__SNC__) // #if defined(_MSC_VER)
#	define CS_SCANF( format, ... )					scanf( format, ##__VA_ARGS__ )
#	define CS_SSCANF( string, format, ... )			sscanf( string, format, ##__VA_ARGS__ )
#	define CS_SCANF_STRING( string, buffer_size )	(string)
#elif defined(__GCC__) // #if defined(_MSC_VER)
#	define CS_SCANF( format, ... )					scanf( format, ##__VA_ARGS__ )
#	define CS_SSCANF( string, format, ... )			sscanf( string, format, ##__VA_ARGS__ )
#	define CS_SCANF_STRING( string, buffer_size )	(string)
#else // #elif defined(SN_TARGET_PS3)
#	error define CS_SCANF, CS_SSCANF and CS_SCANF_STRING macros for your platform here
#endif // #if defined(_MSC_VER)


#endif // #ifndef CS_CORE_STDLIB_EXTENSIONS_H_INCLUDED