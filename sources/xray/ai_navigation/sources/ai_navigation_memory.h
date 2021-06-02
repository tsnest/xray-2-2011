////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef AI_NAVIGATION_MEMORY_H_INCLUDED
#define AI_NAVIGATION_MEMORY_H_INCLUDED

#include <xray/ai_navigation/api.h>

namespace xray {
namespace ai {
namespace navigation {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::ai::navigation::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace navigation
} // namespace ai
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::ai::navigation::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::ai::navigation::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::ai::navigation::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::ai::navigation::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::ai::navigation::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::ai::navigation::g_allocator, type, count )

#endif // #ifndef AI_NAVIGATION_MEMORY_H_INCLUDED