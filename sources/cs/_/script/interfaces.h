////////////////////////////////////////////////////////////////////////////
//	Module 		: interfaces.h
//	Created 	: 15.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : script interfaces
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_INTERFACES_H_INCLUDED
#define CS_SCRIPT_INTERFACES_H_INCLUDED

#include <cs/config.h>

#define CS_SCRIPT_CALL		CS_CALL

#ifndef CS_SCRIPT_API
#	define CS_SCRIPT_API	CS_API
#endif // #ifndef CS_SCRIPT_API

#include <cs/script/world.h>
#include <cs/script/engine.h>
#include <cs/script/thread.h>
#ifdef CS_BUILDING
#	include <cs/script/property_holder.h>
#endif // #ifdef CS_BUILDING

extern "C" {
	CS_SCRIPT_API	void CS_SCRIPT_CALL	cs_lua_studio_backend_init			();						// by default lua_studio_backend is not initialized
	CS_SCRIPT_API	void CS_SCRIPT_CALL	cs_lua_studio_backend_add_log_line	(char const* log_line);
}

#endif // #ifndef CS_SCRIPT_INTERFACES_H_INCLUDED