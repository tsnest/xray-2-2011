////////////////////////////////////////////////////////////////////////////
//	Module 		: libraries_linkage.h
//	Created 	: 14.04.2007
//  Modified 	: 14.04.2007
//	Author		: Dmitriy Iassenev
//	Description : libraries linkage file
////////////////////////////////////////////////////////////////////////////

#ifndef CS_LIBRARIES_LINKAGE_H_INCLUDED
#define CS_LIBRARIES_LINKAGE_H_INCLUDED

#include <cs/config.h>

#ifndef CS_SINGLE_DLL
#	include <cs/lua/library_linkage.h>
#	include <luabind/library_linkage.h>
#	include <cs/core/library_linkage.h>
#	include <cs/script/library_linkage.h>
#endif // #ifndef CS_SINGLE_DLL

#ifdef CS_STATIC_LIBRARIES
#	include <cs/decision/library_linkage.h>
#	include <cs/render_debug/library_linkage.h>
#endif // #ifdef CS_STATIC_LIBRARIES

#include <cs/ai/library_linkage.h>

#endif // #ifndef CS_LIBRARIES_LINKAGE_H_INCLUDED