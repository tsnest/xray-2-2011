////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/maya_animation/api.h>

namespace xray {
namespace maya_animation {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::maya_animation::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace maya_animation
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::maya_animation::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::maya_animation::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::maya_animation::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::maya_animation::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::maya_animation::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::maya_animation::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED