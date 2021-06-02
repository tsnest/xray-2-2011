////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/animation/api.h>
#include <xray/animation/animation_containers.h>

namespace xray {
namespace animation {

extern allocator_type*							g_allocator;

} // namespace animation
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::animation::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::animation::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::animation::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::animation::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::animation::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::animation::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED
