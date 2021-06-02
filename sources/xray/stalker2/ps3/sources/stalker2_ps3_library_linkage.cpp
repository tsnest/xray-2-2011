////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/engine/library_dependencies.h>
#include <xray/engine/library_linkage.h>
#include <xray/render/engine/library_linkage.h>

#include <ogg/library_linkage.h>
#include <vorbis/library_linkage.h>

#include <ode/library_linkage.h>

#include <xray/game/library_linkage.h>

#ifndef MASTER_GOLD
#	define CS_STATIC_LIBRARIES

#	ifdef DEBUG
#		define CS_LUA_USE_DEBUG_LIBRARY
#		define CS_LUABIND_USE_DEBUG_LIBRARY
#		define CS_CORE_USE_DEBUG_LIBRARY
#		define CS_SCRIPT_USE_DEBUG_LIBRARY
#	endif // #ifdef DEBUG

#	include <cs/lua/library_linkage.h>
#	include <luabind/library_linkage.h>
#	include <cs/core/library_linkage.h>
#	include <cs/script/library_linkage.h>
#endif // #ifndef MASTER_GOLD