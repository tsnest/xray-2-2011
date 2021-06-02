////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_EXTENSIONS_H_INCLUDED
#define XRAY_MEMORY_EXTENSIONS_H_INCLUDED

#include <xray/macro_extensions.h>
#include <xray/type_extensions.h>
#include <xray/debug/extensions.h>
#include <xray/memory_pthreads3_allocator.h>

#if XRAY_PLATFORM_WINDOWS
#	define XRAY_MAX_CACHE_LINE_SIZE				64
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
#	define XRAY_MAX_CACHE_LINE_SIZE				128
#elif XRAY_PLATFORM_PS3 // #elif XRAY_PLATFORM_XBOX_360
#	define XRAY_MAX_CACHE_LINE_SIZE				128
#else // #elif XRAY_PLATFORM_PS3
#	error please define your platform!
#endif // #if XRAY_PLATFORM_WINDOWS

#define XRAY_MAX_CACHE_LINE_PAD					char XRAY_STRING_CONCAT(m_cache_line_pad_$, __LINE__) [XRAY_MAX_CACHE_LINE_SIZE]

#if !XRAY_DISABLE_CRT_ALLOCATOR
#	include <xray/memory_crt_allocator.h>
#endif // #if !XRAY_DISABLE_CRT_ALLOCATOR

namespace xray {
namespace memory {

#ifdef XRAY_STATIC_LIBRARIES
	class doug_lea_mt_allocator;
	typedef memory::doug_lea_mt_allocator		crt_allocator_type;
#else // #ifdef XRAY_STATIC_LIBRARIES
	typedef memory::crt_allocator				crt_allocator_type;
#endif // #ifdef XRAY_STATIC_LIBRARIES

	extern XRAY_CORE_API crt_allocator_type*	g_crt_allocator;
} // namespace memory
} // namespace xray

#define XRAY_DEFAULT_ALIGN_SIZE					8
#define XRAY_DEFAULT_ALIGN						XRAY_ALIGN(XRAY_DEFAULT_ALIGN_SIZE)

namespace xray {
namespace memory {

typedef pthreads3_allocator						pthreads_allocator_type;
extern XRAY_CORE_API pthreads_allocator_type	g_mt_allocator;

} // namespace memory
} // namespace xray

#include <xray/memory_doug_lea_allocator.h>
#include <xray/memory_allocator_helper.h>
#include <xray/memory_macros.h>
#include <xray/memory_buffer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {

u64 const Kb					= u64(1024);
u64 const Mb					= u64(1024)*Kb;
u64 const Gb					= u64(1024)*Mb;
u64 const Tb					= u64(1024)*Gb;

namespace memory {

namespace debug {
	enum {
		underrun_guard			= 0xffbdFDFD,
		overrun_guard			= 0xffbdFDFD,
		uninitialized_memory	= 0xffbdCDCD,
		freed_memory			= 0xffbdDDDD,
	};
} // namespace debug

XRAY_CORE_API	void allocate_region				( u64 const additional_memory_size = 0 );
XRAY_CORE_API	void allocate_region				( u64 const additional_memory_size, u64 const additional_address_space );

typedef doug_lea_allocator							doug_lea_allocator_type;

typedef fastdelegate::FastDelegate<void ( base_allocator const&, u32, pvoid, u64, pcstr )>	allocator_predicate_type;
XRAY_CORE_API	void iterate_allocators				( allocator_predicate_type const& predicate );
XRAY_CORE_API	void register_allocator				( base_allocator& allocator, u64 arena_size, pcstr description );

XRAY_CORE_API	void dump_statistics				( bool const dump_stats_for_empty_arenas_as_well = false );
XRAY_CORE_API	void copy							( mutable_buffer const & destination, const_buffer const & source );
XRAY_CORE_API	void copy							( pvoid destination, size_t destination_size, pcvoid source, size_t source_size );

XRAY_CORE_API	void zero							( mutable_buffer const & destination );
XRAY_CORE_API	void zero							( pvoid destination, size_t destination_size );

				template <class T, size_t count>
				void zero							( T (& destination)[count] ) { zero(destination, count * sizeof(T)); }

XRAY_CORE_API	void fill32							( mutable_buffer const & destination, u32 value, u32 count );
XRAY_CORE_API	void fill32							( pvoid destination, size_t destination_size_in_bytes, u32 value, size_t count );
XRAY_CORE_API	void fill8							( mutable_buffer const & destination, u8  value, u32 count );
XRAY_CORE_API	void fill8							( pvoid destination, size_t destination_size_in_bytes, u8  value, size_t count );

XRAY_CORE_API	int  compare						( const_buffer const & buffer1, const_buffer const & buffer2 );
XRAY_CORE_API	bool equal							( const_buffer const & buffer1, const_buffer const & buffer2 );

XRAY_CORE_API	void lock_process_heap				( );
XRAY_CORE_API	bool try_lock_process_heap			( );
XRAY_CORE_API	void unlock_process_heap			( );

inline pvoid fill_uninitialized( pvoid buffer, u32 const buffer_size )
{
#ifndef MASTER_GOLD
	u32 const u32_count	= buffer_size/4;
	if ( u32_count )
		memory::fill32	( buffer, buffer_size, (u32)debug::uninitialized_memory, u32_count );

	u32 const u8_count	= buffer_size - u32_count*sizeof(u32);
	if ( u8_count )
		memory::fill8	( static_cast<u32*>(buffer) + u32_count, u8_count, 0xcd, u8_count );
#endif // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETER( buffer_size );
	return				buffer;
}

} // namespace memory
} // namespace xray

#define ALLOCA( size )								::xray::memory::fill_uninitialized( XRAY_ALLOCA_IMPL(size), (size) )

// multithreaded general purpose memory allocation macros
// use ONLY in case when you need multi threaded functionality
// otherwise use specific single threaded memory allocator
#define MT_NEW( type )								XRAY_NEW_IMPL(		::xray::memory::g_mt_allocator, type )
#define MT_DELETE( pointer )						XRAY_DELETE_IMPL(	::xray::memory::g_mt_allocator, pointer )
#define MT_MALLOC( size, description )				XRAY_MALLOC_IMPL(	::xray::memory::g_mt_allocator, size, description )
#define MT_REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	::xray::memory::g_mt_allocator, pointer, size, description )
#define MT_FREE( pointer )							XRAY_FREE_IMPL(		::xray::memory::g_mt_allocator, pointer )
#define MT_ALLOC( type, count )						XRAY_ALLOC_IMPL(	::xray::memory::g_mt_allocator, type, count )

#endif // #ifndef XRAY_MEMORY_EXTENSIONS_H_INCLUDED
