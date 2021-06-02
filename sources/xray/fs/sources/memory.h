////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_MEMORY_H_INCLUDED
#define FS_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

#include <xray/fs/api.h>

namespace xray {
namespace fs_new {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::fs_new::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace fs_new
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::fs::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::fs::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::fs::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::fs::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::fs::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::fs::g_allocator, type, count )

#endif // #ifndef FS_MEMORY_H_INCLUDED