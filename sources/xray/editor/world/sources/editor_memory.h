////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#ifdef DEBUG
#	include <typeinfo.h>
#endif // #ifdef DEBUG

#include <xray/memory_macros.h>
#include <xray/editor/world/api.h>

namespace xray {
namespace editor {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::editor::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace input
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::editor::g_allocator, type )
#define DELETE( pointer, ... )					XRAY_DELETE_IMPL(	*::xray::editor::g_allocator, pointer, __VA_ARGS__ )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::editor::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::editor::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::editor::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::editor::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED