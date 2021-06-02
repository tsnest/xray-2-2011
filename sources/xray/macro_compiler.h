////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_COMPILER_H_INCLUDED
#define XRAY_MACRO_COMPILER_H_INCLUDED

#include <xray/macro_platform.h>

#if defined(_MSC_VER)
#	if _MSC_VER >= 1100
#		define XRAY_NOVTABLE									__declspec( novtable )
#	else // #if _MSC_VER >= 1100
#		define XRAY_NOVTABLE
#	endif // #if _MSC_VER >= 1100

#	define XRAY_ALIGN( alignment )								__declspec( align(alignment) )
#	define XRAY_DLL_IMPORT										__declspec( dllimport )
#	define XRAY_DLL_EXPORT										__declspec( dllexport )
//#	define XRAY_ASSUME( expression )							__assume( expression )
#	define XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( type )			virtual	~type	( ) = 0 { }
#	define XRAY_ALLOCA_IMPL										_alloca
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#	define XRAY_NOVTABLE
#	define XRAY_ALIGN( alignment )								__attribute__( (aligned(alignment)) )
#	define XRAY_DLL_IMPORT										__attribute__( (dllimport) )
#	define XRAY_DLL_EXPORT										__attribute__( (dllexport) )
#	define XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( type )			virtual	~type	( ) {}

#	include <alloca.h>
#	define XRAY_ALLOCA_IMPL										alloca

#	define __FUNCSIG__											"<unknown function signature>"
#else // #elif defined(__GNUC__)
#	error please define macros for your compiler here
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

#endif // #ifndef XRAY_MACRO_COMPILER_H_INCLUDED