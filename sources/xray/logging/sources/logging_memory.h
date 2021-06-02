////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_MEMORY_H_INCLUDED
#define LOGGING_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

#include <xray/logging/api.h>

namespace xray {
namespace logging {

extern memory::doug_lea_allocator *				g_allocator;

#define USER_ALLOCATOR							*::xray::logging::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace logging
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::logging::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::logging::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::logging::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::logging::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::logging::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::logging::g_allocator, type, count )

#endif // #ifndef LOGGING_MEMORY_H_INCLUDED