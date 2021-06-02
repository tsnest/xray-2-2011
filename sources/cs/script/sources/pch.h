////////////////////////////////////////////////////////////////////////////
//	Module 		: pch.h
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : precompiled header
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4995)
#	include <malloc.h>
#	pragma warning(pop)
#endif // #ifdef _MSC_VER

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef CS_SINGLE_DLL
#	define CS_API
#endif // #ifdef CS_SINGLE_DLL

#include <cs/core/extensions.h>

#ifndef CS_STATIC_LIBRARIES
#	define CS_SCRIPT_API	__declspec(dllexport)
#endif // #ifdef CS_STATIC_LIBRARIES

#include <cs/script/interfaces.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <luabind/luabind.hpp>
#pragma warning(pop)

inline bool lua_typestring(lua_State* L, int idx)	{ return lua_type(L, idx) == LUA_TSTRING; }
inline bool lua_typenumber(lua_State* L, int idx)	{ return lua_type(L, idx) == LUA_TNUMBER; }

#endif // #ifndef PCH_H_INCLUDED