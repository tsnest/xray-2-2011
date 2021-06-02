////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_PLATFORM_H_INCLUDED
#define XRAY_MACRO_PLATFORM_H_INCLUDED

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_WINDOWS_32
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_WINDOWS_32
#	error do not define XRAY_PLATFORM_WINDOWS_32 macro
#endif // #ifdef XRAY_PLATFORM_WINDOWS_32

#ifdef WIN32
#	define XRAY_PLATFORM_WINDOWS_32	1
#else // #ifdef WIN32
#	define XRAY_PLATFORM_WINDOWS_32	0
#endif // #ifdef WIN32

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_WINDOWS_64
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_WINDOWS_64
#	error do not define XRAY_PLATFORM_WINDOWS_64 macro
#endif // #ifdef XRAY_PLATFORM_WINDOWS_64

#ifdef WIN64
#	define XRAY_PLATFORM_WINDOWS_64	1
#else // #ifdef WIN32
#	define XRAY_PLATFORM_WINDOWS_64	0
#endif // #ifdef WIN32

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_WINDOWS
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_WINDOWS
#	error do not define XRAY_PLATFORM_WINDOWS macro
#endif // #ifdef XRAY_PLATFORM_WINDOWS

#if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_WINDOWS_64
#	define XRAY_PLATFORM_WINDOWS	1
#else // #if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_WINDOWS_64
#	define XRAY_PLATFORM_WINDOWS	0
#endif // #if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_WINDOWS_64

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_XBOX_360
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_XBOX_360
#	error do not define XRAY_PLATFORM_XBOX_360 macro
#endif // #ifdef XRAY_PLATFORM_XBOX_360

#ifdef _XBOX
#	if _XBOX_VER < 200 
#		error engine doesn't support XBox versions earlier to XBox 360
#	endif // #if _XBOX_VER < 200 
#	define XRAY_PLATFORM_XBOX_360	1
#	define XRAY_STATIC_LIBRARIES	1
#else // #ifdef _XBOX
#	define XRAY_PLATFORM_XBOX_360	0
#endif // #ifdef _XBOX

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_PS3
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_PS3
#	error do not define XRAY_PLATFORM_PS3 macro
#endif // #ifdef XRAY_PLATFORM_PS3

#ifdef SN_TARGET_PS3
#	define XRAY_PLATFORM_PS3		1
#else // #ifdef SN_TARGET_PS3
#	define XRAY_PLATFORM_PS3		0
#endif // #ifdef SN_TARGET_PS3

////////////////////////////////////////////////////////////////////////////
// XRAY_LITTLE_ENDIAN / XRAY_BIG_ENDIAN
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_LITTLE_ENDIAN
#	error do not define XRAY_LITTLE_ENDIAN macro
#endif // #ifdef XRAY_LITTLE_ENDIAN

#ifdef XRAY_BIG_ENDIAN
#	error do not define XRAY_BIG_ENDIAN macro
#endif // #ifdef XRAY_BIG_ENDIAN

#if XRAY_PLATFORM_WINDOWS 
#	define	XRAY_LITTLE_ENDIAN		1
#	define	XRAY_BIG_ENDIAN			0
#elif XRAY_PLATFORM_XBOX_360		// #if XRAY_PLATFORM_WINDOWS 
#	define	XRAY_LITTLE_ENDIAN		0
#	define	XRAY_BIG_ENDIAN			1
#elif XRAY_PLATFORM_PS3				// #elif XRAY_PLATFORM_XBOX_360
#	define	XRAY_LITTLE_ENDIAN		0
#	define	XRAY_BIG_ENDIAN			1
#endif								// #elif XRAY_PLATFORM_PS3

#if XRAY_LITTLE_ENDIAN & XRAY_BIG_ENDIAN
#	error invalid platform capabilities specified
#endif // #if XRAY_LITTLE_ENDIAN & XRAY_BIG_ENDIAN

////////////////////////////////////////////////////////////////////////////
// XRAY_PLATFORM_32_BIT / XRAY_PLATFORM_64_BIT
////////////////////////////////////////////////////////////////////////////
#ifdef XRAY_PLATFORM_32_BIT
#	error do not define XRAY_PLATFORM_32_BIT macro
#endif // #ifdef XRAY_LITTLE_ENDIAN

#ifdef XRAY_PLATFORM_64_BIT
#	error do not define XRAY_PLATFORM_64_BIT macro
#endif // #ifdef XRAY_BIG_ENDIAN

#if XRAY_PLATFORM_WINDOWS_64
#	define	XRAY_PLATFORM_32_BIT	0
#	define	XRAY_PLATFORM_64_BIT	1
#elif XRAY_PLATFORM_WINDOWS_32		// #if XRAY_PLATFORM_WINDOWS_64
#	define	XRAY_PLATFORM_32_BIT	1
#	define	XRAY_PLATFORM_64_BIT	0
#elif XRAY_PLATFORM_XBOX_360		// #if XRAY_PLATFORM_WINDOWS 
#	define	XRAY_PLATFORM_32_BIT	1
#	define	XRAY_PLATFORM_64_BIT	0
#elif XRAY_PLATFORM_PS3				// #elif XRAY_PLATFORM_XBOX_360
#	define	XRAY_PLATFORM_32_BIT	1
#	define	XRAY_PLATFORM_64_BIT	0
#endif								// #elif XRAY_PLATFORM_PS3

#if XRAY_PLATFORM_32_BIT & XRAY_PLATFORM_64_BIT
#	error invalid platform capabilities specified
#endif // #if XRAY_LITTLE_ENDIAN & XRAY_BIG_ENDIAN

////////////////////////////////////////////////////////////////////////////
// check macros
////////////////////////////////////////////////////////////////////////////
#if XRAY_PLATFORM_WINDOWS_32
#	if XRAY_PLATFORM_WINDOWS_64 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if XRAY_PLATFORM_WINDOWS_64 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_PS3
#elif XRAY_PLATFORM_WINDOWS_64		// #if XRAY_PLATFORM_WINDOWS_32
#	if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_PS3
#elif XRAY_PLATFORM_XBOX_360			// #elif XRAY_PLATFORM_WINDOWS_64
#	if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_WINDOWS_64 | XRAY_PLATFORM_PS3
#		error you cannot specify several target platforms
#	endif // #if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_WINDOWS_64 | XRAY_PLATFORM_PS3
#elif XRAY_PLATFORM_PS3				// #elif XRAY_PLATFORM_XBOX_360
#	if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_XBOX_360
#		error you cannot specify several target platforms
#	endif // #if XRAY_PLATFORM_WINDOWS_32 | XRAY_PLATFORM_XBOX_360 | XRAY_PLATFORM_XBOX_360
#else								// #elif XRAY_PLATFORM_PS3
#		error unknown target platform!
#endif // #if XRAY_PLATFORM_WINDOWS_32

#endif // #ifndef XRAY_MACRO_PLATFORM_H_INCLUDED