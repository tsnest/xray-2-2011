////////////////////////////////////////////////////////////////////////////
//	Module 		: library_linkage.h
//	Created 	: 14.04.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : library linkage file
////////////////////////////////////////////////////////////////////////////

#ifndef CS_LUA_LAUXLIB_H_INCLUDED
#define CS_LUA_LAUXLIB_H_INCLUDED

#include <cs/config.h>

#if CS_USE_LUAJIT
#	include <luajit/lauxlib.h>
#else // #if CS_USE_LUAJIT
#	include <lua/lauxlib.h>
#endif // #if CS_USE_LUAJIT

#endif // #ifndef CS_LUA_LAUXLIB_H_INCLUDED