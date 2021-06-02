////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

namespace stalker2 {

extern xray::memory::doug_lea_allocator_type*	g_allocator;

#define USER_ALLOCATOR							*::stalker2::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace stalker2

#define NEW( type )								XRAY_NEW_IMPL(		*::stalker2::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::stalker2::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::stalker2::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::stalker2::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::stalker2::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::stalker2::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED