////////////////////////////////////////////////////////////////////////////
//	Unit		: macro_platform.h
//	Created		: 14.09.2009
//	Author		: Dmitriy Iassenev
//	Copyright(C) Challenge Solutions(tm) - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MACRO_PLATFORM_H_INCLUDED
#define CS_CORE_MACRO_PLATFORM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////
// CS_PLATFORM_WINDOWS_32
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PLATFORM_WINDOWS_32
#	error do not define CS_PLATFORM_WINDOWS_32 macro
#endif // #ifdef CS_PLATFORM_WINDOWS_32

#ifdef WIN32
#	define CS_PLATFORM_WINDOWS_32	1
#else // #ifdef WIN32
#	define CS_PLATFORM_WINDOWS_32	0
#endif // #ifdef WIN32

////////////////////////////////////////////////////////////////////////////
// CS_PLATFORM_WINDOWS_64
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PLATFORM_WINDOWS_64
#	error do not define CS_PLATFORM_WINDOWS_64 macro
#endif // #ifdef CS_PLATFORM_WINDOWS_64

#ifdef WIN64
#	define CS_PLATFORM_WINDOWS_64	1
#else // #ifdef WIN32
#	define CS_PLATFORM_WINDOWS_64	0
#endif // #ifdef WIN32

////////////////////////////////////////////////////////////////////////////
// CS_PLATFORM_WINDOWS
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PLATFORM_WINDOWS
#	error do not define CS_PLATFORM_WINDOWS macro
#endif // #ifdef CS_PLATFORM_WINDOWS

#if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_WINDOWS_64
#	define CS_PLATFORM_WINDOWS		1
#else // #if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_WINDOWS_64
#	define CS_PLATFORM_WINDOWS		0
#endif // #if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_WINDOWS_64

////////////////////////////////////////////////////////////////////////////
// CS_PLATFORM_XBOX_360
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PLATFORM_XBOX_360
#	error do not define CS_PLATFORM_XBOX_360 macro
#endif // #ifdef XRAY_CORE_BUILDING

#ifdef _XBOX
#	define CS_PLATFORM_XBOX_360		1
#else // #ifdef _XBOX
#	define CS_PLATFORM_XBOX_360		0
#endif // #ifdef _XBOX

////////////////////////////////////////////////////////////////////////////
// CS_PLATFORM_PS3
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PLATFORM_PS3
#	error do not define CS_PLATFORM_PS3 macro
#endif // #ifdef XRAY_CORE_BUILDING

#ifdef SN_TARGET_PS3
#	define CS_PLATFORM_PS3			1
#else // #ifdef SN_TARGET_PS3
#	define CS_PLATFORM_PS3			0
#endif // #ifdef SN_TARGET_PS3

////////////////////////////////////////////////////////////////////////////
// CS_LITTLE_ENDIAN / CS_BIG_ENDIAN
////////////////////////////////////////////////////////////////////////////
#ifdef CS_LITTLE_ENDIAN
#	error do not define CS_LITTLE_ENDIAN macro
#endif // #ifdef XRAY_CORE_BUILDING

#ifdef CS_BIG_ENDIAN
#	error do not define CS_BIG_ENDIAN macro
#endif // #ifdef XRAY_CORE_BUILDING

#if CS_PLATFORM_WINDOWS 
#	define	CS_LITTLE_ENDIAN	1
#	define	CS_BIG_ENDIAN		0
#elif CS_PLATFORM_XBOX_360		// #if CS_PLATFORM_WINDOWS 
#	define	CS_LITTLE_ENDIAN	0
#	define	CS_BIG_ENDIAN		1
#elif CS_PLATFORM_PS3			// #elif CS_PLATFORM_XBOX_360
#	define	CS_LITTLE_ENDIAN	0
#	define	CS_BIG_ENDIAN		1
#endif							// #elif CS_PLATFORM_PS3

#if CS_LITTLE_ENDIAN & CS_BIG_ENDIAN
#	error invalid platform capabilities specified
#endif // #if CS_LITTLE_ENDIAN & CS_BIG_ENDIAN

#if defined(_MSC_VER)
#	define CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( type )	virtual ~type	( ) = 0 { }
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#	define CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( type )	inline virtual ~type( ) { }
#else // #elif defined(__GNUC__)
#	error define macro CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR for your compiler
#endif // #if defined(_MSC_VER)

////////////////////////////////////////////////////////////////////////////
// check macros
////////////////////////////////////////////////////////////////////////////
#if CS_PLATFORM_WINDOWS_32
#	if CS_PLATFORM_WINDOWS_64 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if CS_PLATFORM_WINDOWS_64 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_PS3
#elif CS_PLATFORM_WINDOWS_64		// #if CS_PLATFORM_WINDOWS_32
#	if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_PS3
#elif CS_PLATFORM_XBOX_360			// #elif CS_PLATFORM_WINDOWS_64
#	if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_WINDOWS_64 | CS_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_WINDOWS_64 | CS_PLATFORM_PS3
#elif CS_PLATFORM_PS3				// #elif CS_PLATFORM_XBOX_360
#	if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_XBOX_360
#		error you cannot specify several target platforms
#	endif // #if CS_PLATFORM_WINDOWS_32 | CS_PLATFORM_XBOX_360 | CS_PLATFORM_XBOX_360
#else								// #elif CS_PLATFORM_PS3
#		error unknown target platform!
#endif // #if CS_PLATFORM_WINDOWS_32

#endif // #ifndef CS_CORE_MACRO_PLATFORM_H_INCLUDED