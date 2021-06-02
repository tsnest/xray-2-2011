////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_BASE_MEMORY_H_INCLUDED
#define EDITOR_BASE_MEMORY_H_INCLUDED

#include <xray/editor/base/api.h>

namespace xray {
namespace editor_base {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::editor_base::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace editor_base
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::editor_base::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::editor_base::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::editor_base::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::editor_base::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::editor_base::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::editor_base::g_allocator, type, count )

#endif // #ifndef EDITOR_BASE_MEMORY_H_INCLUDED