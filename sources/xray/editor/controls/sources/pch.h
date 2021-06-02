////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#ifdef DEBUG
#	define ASSERT(expression)	if ( true ) { if (!(expression)) throw; } else (void)0
#	define NODEFAULT( ... )		if ( true ) { __debugbreak(); __VA_ARGS__; } else (void)0
#else // #ifdef DEBUG
#	define ASSERT(expression)	if ( false ) { expression; } else (void)0
#	define NODEFAULT( ... )		__assume(0)
#endif // #ifdef DEBUG

#pragma warning(disable:4127)
#pragma warning(disable:4100)

#endif // #ifndef PCH_H_INCLUDED