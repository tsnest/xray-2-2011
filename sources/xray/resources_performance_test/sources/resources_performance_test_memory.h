////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_PERFORMANCE_TEST_MEMORY_H_INCLUDED
#define RESOURCES_PERFORMANCE_TEST_MEMORY_H_INCLUDED

namespace xray {
namespace resources_performance_test {

extern	xray::memory::doug_lea_allocator_type	g_allocator;

#define USER_ALLOCATOR							g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace resources_performance_test
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		::xray::resources_performance_test::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	::xray::resources_performance_test::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	::xray::resources_performance_test::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	::xray::resources_performance_test::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		::xray::resources_performance_test::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	::xray::resources_performance_test::g_allocator, type, count )

#endif // #ifndef RESOURCES_PERFORMANCE_TEST_MEMORY_H_INCLUDED