////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED
#define XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED

#define XRAY_USE_CRT_MEMORY_ALLOCATOR	0

extern "C" {
	pvoid malloc					( size_t size );
	pvoid calloc					( size_t count, size_t element_size );
	pvoid realloc					( pvoid pointer, size_t size );
	void free						( pvoid pointer );
	pvoid _malloc_crt				( size_t size );
	pvoid _calloc_crt				( size_t count, size_t size );
	pvoid _realloc_crt				( pvoid ptr, size_t size );
	pvoid _recalloc_crt				( pvoid ptr, size_t count, size_t size );
	pvoid _aligned_malloc			( size_t size, size_t alignment );
	pvoid _aligned_realloc			( pvoid ptr, size_t size, size_t alignment );
	void _aligned_free				( pvoid ptr );
	pvoid _aligned_offset_malloc	( size_t size, size_t alignment, size_t offset );
	pvoid _aligned_offset_realloc	( pvoid ptr, size_t size, size_t alignment, size_t offset );
} // extern "C"

// to prevent memory functions redefinition
#define _CRT_ALLOCATION_DEFINED

#if defined(_MSC_VER)
#	include <malloc.h>
#endif // #if defined(_MSC_VER)

#include <new.h>

#ifdef _MSC_VER
#	include <memory.h>
#endif // #ifdef _MSC_VER

#include <string.h>

#include <algorithm>
#include <map>

#define malloc(...)						XRAY_UNREACHABLE_CODE( "CRT malloc detected!" );
#define calloc(...)						XRAY_UNREACHABLE_CODE( "CRT calloc detected!" );
#define realloc(...)					XRAY_UNREACHABLE_CODE( "CRT realloc detected!" );
#define free(...)						XRAY_UNREACHABLE_CODE( "CRT free detected!" );

#define _aligned_malloc(...)			XRAY_UNREACHABLE_CODE( "CRT _aligned_malloc detected!" );
#define _aligned_realloc(...)			XRAY_UNREACHABLE_CODE( "CRT _aligned_realloc detected!" );
#define _aligned_free(...)				XRAY_UNREACHABLE_CODE( "CRT _aligned_free detected!" );
#define _aligned_offset_malloc(...)		XRAY_UNREACHABLE_CODE( "CRT _aligned_offset_malloc detected!" );
#define _aligned_offset_realloc(...)	XRAY_UNREACHABLE_CODE( "CRT _aligned_offset_realloc detected!" );

#define _malloc_crt(...)				XRAY_UNREACHABLE_CODE( "CRT _malloc_crt detected!" );
#define _calloc_crt(...)				XRAY_UNREACHABLE_CODE( "CRT _calloc_crt detected!" );
#define _realloc_crt(...)				XRAY_UNREACHABLE_CODE( "CRT _realloc_crt detected!" );
#define _recalloc_crt(...)				XRAY_UNREACHABLE_CODE( "CRT _recalloc_crt detected!" );

#define strdup(...)						XRAY_UNREACHABLE_CODE( "CRT strdup detected!" );
#define _strdup(...)					XRAY_UNREACHABLE_CODE( "CRT _strdup detected!" );

#if defined(_MSC_VER)
#	pragma deprecated(malloc,calloc,realloc,free,_malloc_crt,_calloc_crt,_realloc_crt,_recalloc_crt,strdup,_strdup)
#endif // #if defined(_MSC_VER)

pvoid	__cdecl	operator new			( size_t buffer_size );
pvoid	__cdecl operator new [ ]		( size_t buffer_size );
void	__cdecl operator delete			( pvoid pointer );
void	__cdecl operator delete [ ]		( pvoid pointer ) throw ( );

#endif // #ifndef XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED