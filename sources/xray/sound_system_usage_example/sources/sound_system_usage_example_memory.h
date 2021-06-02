////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SYSTEM_USAGE_EXAMPLE_MEMORY_H_INCLUDED
#define SOUND_SYSTEM_USAGE_EXAMPLE_MEMORY_H_INCLUDED

namespace xray {
namespace sound_system_usage_example {

extern	xray::memory::doug_lea_allocator_type	g_allocator;

#define USER_ALLOCATOR							g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace sound_system_usage_example
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		::xray::sound_system_usage_example::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	::xray::sound_system_usage_example::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	::xray::sound_system_usage_example::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	::xray::sound_system_usage_example::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		::xray::sound_system_usage_example::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	::xray::sound_system_usage_example::g_allocator, type, count )

#endif // #ifndef SOUND_SYSTEM_USAGE_EXAMPLE_MEMORY_H_INCLUDED