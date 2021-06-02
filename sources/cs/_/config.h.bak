////////////////////////////////////////////////////////////////////////////
//	Module 		: config.h
//	Created 	: 28.10.2006
//  Modified 	: 14.04.2007
//	Author		: Dmitriy Iassenev
//	Description : configuration file
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CONFIG_H_INCLUDED
#define CS_CONFIG_H_INCLUDED

#include <cs/defines.h>
#include <cs/core/macro_platform.h>

////////////////////////////////////////////////////////////////////////////
// CS_USE_LUAJIT
////////////////////////////////////////////////////////////////////////////
#if CS_USE_LUAJIT
#	error do not define CS_USE_LUAJIT macro
#endif // #if CS_USE_LUAJIT

#if CS_PLATFORM_WINDOWS_32
#	define CS_USE_LUAJIT		1
#else // #if CS_PLATFORM_WINDOWS_32
#	define CS_USE_LUAJIT		0
#endif // #if CS_PLATFORM_WINDOWS_32

////////////////////////////////////////////////////////////////////////////
// CS_DEBUG_LIBRARIES
////////////////////////////////////////////////////////////////////////////
#ifdef CS_DEBUG_LIBRARIES
#	error do not define CS_DEBUG_LIBRARIES macro
#endif // #ifdef CS_DEBUG_LIBRARIES

#ifdef NDEBUG
#	define CS_DEBUG_LIBRARIES	0
#else // #ifdef NDEBUG
#	define CS_DEBUG_LIBRARIES	1
#endif // #ifdef NDEBUG

////////////////////////////////////////////////////////////////////////////
// CS_CALL
////////////////////////////////////////////////////////////////////////////
#ifdef CS_CALL
#	error do not define CS_CALL macro
#endif // #ifdef CS_CALL

#define CS_CALL					__stdcall

////////////////////////////////////////////////////////////////////////////
// CS_PACK_SIZE
////////////////////////////////////////////////////////////////////////////
#ifdef CS_PACK_SIZE
#	error do not define CS_PACK_SIZE macro
#endif // #ifdef CS_PACK_SIZE

#define CS_PACK_SIZE			4

#ifndef CS_API
#	ifdef CS_STATIC_LIBRARIES
#		define CS_API
#	else // #ifdef CS_STATIC_LIBRARIES
#		define CS_API			__declspec(dllimport)
#	endif // #ifdef CS_STATIC_LIBRARIES
#endif // #ifndef CS_API

#endif // #ifndef CS_CONFIG_H_INCLUDED