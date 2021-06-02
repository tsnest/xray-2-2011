////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_DEBUG_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_DEBUG_ALLOCATOR_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS
#	ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				1
#	else // #ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				0
#	endif // #ifndef MASTER_GOLD
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
#	ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				1
#	else // #ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				0
#	endif // #ifndef MASTER_GOLD
#elif XRAY_PLATFORM_PS3 // #elif XRAY_PLATFORM_XBOX_360
#	ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				1
#	else // #ifndef MASTER_GOLD
#		define XRAY_DEBUG_ALLOCATOR				0
#	endif // #ifndef MASTER_GOLD
#else // #elif XRAY_PLATFORM_PS3
#	error please define your platform!
#endif // #if XRAY_PLATFORM_WINDOWS

#if XRAY_DEBUG_ALLOCATOR
#	include <xray/memory_doug_lea_mt_allocator.h>

namespace xray {

#if XRAY_DEBUG_ALLOCATOR

namespace memory {
	typedef doug_lea_mt_allocator					doug_lea_mt_allocator_type;
} // namespace memory

namespace debug {
	extern XRAY_CORE_API memory::doug_lea_mt_allocator_type	g_mt_allocator;
} // namespace debug

#endif // #if XRAY_DEBUG_ALLOCATOR

} // namespace xray

#define DEBUG_NEW( type )							XRAY_NEW_IMPL(		::xray::debug::g_mt_allocator, type )
#define DEBUG_DELETE( pointer )						XRAY_DELETE_IMPL(	::xray::debug::g_mt_allocator, pointer )
#define DEBUG_MALLOC( size, description )			XRAY_MALLOC_IMPL(	::xray::debug::g_mt_allocator, size, description )
#define DEBUG_REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	::xray::debug::g_mt_allocator, pointer, size, description )
#define DEBUG_FREE( pointer )						XRAY_FREE_IMPL(		::xray::debug::g_mt_allocator, pointer )
#define DEBUG_ALLOC( type, count )					XRAY_ALLOC_IMPL(	::xray::debug::g_mt_allocator, type, count )

#endif // #if XRAY_DEBUG_ALLOCATOR

#endif // #ifndef XRAY_MEMORY_DEBUG_ALLOCATOR_H_INCLUDED