////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/sound/api.h>

namespace xray {
namespace sound {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::sound::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace sound
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::sound::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::sound::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::sound::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::sound::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::sound::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::sound::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED