////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_EDITOR_MEMORY_H_INCLUDED
#define PARTICLE_EDITOR_MEMORY_H_INCLUDED

#include <xray/editor/particle/api.h>

namespace xray {
namespace particle_editor {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::particle_editor::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace particle_editor
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::particle_editor::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::particle_editor::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::particle_editor::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::particle_editor::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::particle_editor::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::particle_editor::g_allocator, type, count )

#endif // #ifndef PARTICLE_EDITOR_MEMORY_H_INCLUDED