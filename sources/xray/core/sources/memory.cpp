////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory.h"
#include "memory_platform.h"
#include "memory_monitor.h"
#include "memory_leak_detector.h"
#include "memory_guard.h"
#include "fs_macros.h"
#include "resources_manager.h"
#include <xray/memory_crt_allocator.h>
#include "strings_shared_manager.h"
#include "memory_process_allocator.h"
#include <xray/console_command.h>
#include <xray/testing.h>
#include "memory_platform.h"
#include "resources_allocators.h"
#include <xray/construction.h>
#include <xray/memory_doug_lea_mt_allocator.h>

#if XRAY_DEBUG_ALLOCATOR
xray::memory::doug_lea_mt_allocator_type		xray::debug::g_mt_allocator(
													true,
													false
#if XRAY_USE_MEMORY_LEAK_DETECTOR
													, false
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
												);
#endif // #if XRAY_DEBUG_ALLOCATOR

xray::memory::pthreads_allocator_type			xray::memory::g_mt_allocator;
xray::memory::fs_allocator						xray::memory::g_fs_allocator;
xray::memory::doug_lea_allocator_type			xray::memory::g_cook_allocator;
xray::memory::doug_lea_allocator_type			xray::strings::shared::g_allocator( xray::memory::thread_id_const_false );

xray::memory::doug_lea_allocator_type			xray::memory::g_resources_helper_allocator( xray::memory::thread_id_const_false );
xray::memory::managed_allocator					xray::memory::g_resources_managed_allocator( 0 * Mb, 0 );

xray::memory::doug_lea_allocator_type			xray::memory::g_resources_unmanaged_allocator( xray::memory::thread_id_const_false, false, true );

xray::memory::fixed_size_allocator< xray::resources::resource_link, xray::threading::mutex >
												xray::memory::g_resources_links_allocator;

static xray::command_line::key					s_enable_crt_memory_allocator( "enable_crt_memory_allocator", "", "memory", "enables crt memory allocator usage");

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
xray::memory::crt_allocator_type*				xray::memory::g_crt_allocator = 0;
#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

#if XRAY_DEBUG_ALLOCATOR
static xray::command_line::key					s_debug_allocator_memory("debug_allocator", "", "memory", "set maximum memory for debug allocator, Mb");
#endif // #if XRAY_DEBUG_ALLOCATOR

namespace xray {

namespace command_line {
	bool key_is_set_impl	( pcstr, pcstr );
} // namespace command_line

} // namespace xray

static void dump_memory_statistics( pcstr dummy = 0 )
{
	XRAY_UNREFERENCED_PARAMETER	(dummy);
	xray::memory::dump_statistics();
}

static xray::console_commands::cc_delegate	s_dump_statistics( "dump_memory_statistics", &dump_memory_statistics, false );

static xray::uninitialized_reference<xray::threading::mutex>	s_process_heap_walk;

#ifndef XRAY_STATIC_LIBRARIES
static xray::memory::crt_allocator_type	s_crt_allocator;
#endif // #ifndef XRAY_STATIC_LIBRARIES

static xray::memory::process_allocator	s_process_allocator;

struct allocator_data {
	xray::memory::base_allocator*		allocator;
	u64									arena_size;
	pvoid								arena_address;
	pcstr								arena_id;
}; // struct allocator_data

typedef xray::fixed_vector< allocator_data, 32 >	allocators_type;

static xray::uninitialized_reference<allocators_type>	s_allocators;

static u64			s_arena_size	= 0;

void xray::memory::lock_process_heap		( )
{
	s_process_heap_walk->lock	( );
}

bool xray::memory::try_lock_process_heap	( )
{
	return						s_process_heap_walk->try_lock( );
}

void xray::memory::unlock_process_heap		( )
{
	s_process_heap_walk->unlock	( );
}

void xray::memory::preinitialize			( )
{
#if !XRAY_DISABLE_CRT_ALLOCATOR && !defined( XRAY_STATIC_LIBRARIES )
	memory::g_crt_allocator						= &s_crt_allocator;	
#else // #if !XRAY_DISABLE_CRT_ALLOCATOR && !defined( XRAY_STATIC_LIBRARIES )
//	XRAY_UNREFERENCED_PARAMETER					( command_line );
#endif // #if !XRAY_DISABLE_CRT_ALLOCATOR && !defined( XRAY_STATIC_LIBRARIES )

	XRAY_CONSTRUCT_REFERENCE					( s_process_heap_walk, xray::threading::mutex );

	ASSERT										( !s_arena_size, "memory has been preinitialized already" );

	XRAY_CONSTRUCT_REFERENCE					( s_allocators, allocators_type );

#ifndef XRAY_STATIC_LIBRARIES
	s_crt_allocator.do_register					(             0,	"C runtime library"		);
#else // #ifndef XRAY_STATIC_LIBRARIES
	g_crt_allocator->do_register				(			  0,	"C runtime library"		);
#endif // #ifndef XRAY_STATIC_LIBRARIES
	s_process_allocator.do_register				(             0,	"process heap"			);
	strings::shared::g_allocator.do_register	(      256*Kb  ,	"shared strings"		);
	g_fs_allocator.do_register					(		16*Mb  ,	"filesystem"			);

	//xray::particle::g_particles_allocator.do_register			(		 10*Mb  ,	"particle system"		);
	
	u32 mt_memory_amount						= 8*Mb;
	if ( testing::run_tests_command_line () )
		mt_memory_amount						+= 16*Mb;

#if XRAY_DEBUG_ALLOCATOR
	if ( !s_debug_allocator_memory.is_set() )
		register_debug_allocator				( 0 );
	else {
		int result;
		if ( !s_debug_allocator_memory.is_set_as_number(&result) )
			register_debug_allocator			( 0 );
		else
			register_debug_allocator			( result*Mb );
	}
#endif // #if XRAY_DEBUG_ALLOCATOR
	g_mt_allocator.do_register					( mt_memory_amount,	"global multithreaded"	);
	g_cook_allocator.do_register				( 1*Mb + 128*Kb,	"cook allocator"		);

	g_resources_helper_allocator.do_register	(
#if XRAY_PLATFORM_64_BIT
		2*
#endif // #if XRAY_PLATFORM_64_BIT
		4*Mb,
		"resources helper allocator"
	);

	g_resources_links_allocator.do_register		(
		math::align_down<u64>( 256*Kb, sizeof(xray::resources::resource_link) ),
		"resources links allocator"
	);
}

void xray::memory::allocate_region		( u64 const additional_memory_size )
{
	allocate_region				( additional_memory_size, additional_memory_size );
}

void xray::memory::allocate_region		( u64 const additional_memory_size, u64 const additional_address_space )
{
	s_arena_size				= 0;

	u32 const count				= s_allocators->size() + 2;
	platform::regions_type		regions( ALLOCA(count*sizeof(platform::region)), count );

	{
		allocators_type::iterator i			= s_allocators->begin( );
		allocators_type::iterator const e	= s_allocators->end( );
		for ( ; i != e; ++i ) {
			if ( !(*i).arena_size )
				continue;

			s_arena_size					+= (*i).arena_size;
			platform::region const value	= { (*i).arena_size, 0, &*i };
			regions.push_back				( value );
		}
	}

	platform::region managed_arena		= { 0, 0, &managed_arena };
	platform::region unmanaged_arena	= { 0, 0, &unmanaged_arena };

	platform::allocate_arenas		( additional_memory_size, additional_address_space, regions, managed_arena, unmanaged_arena );

	g_resources_managed_allocator.do_register	( managed_arena.size,	"resources (managed) allocator" );
	managed_arena.data				= &s_allocators->back();
	regions.push_back				( managed_arena );

	g_resources_unmanaged_allocator.do_register	( unmanaged_arena.size,	"resources (unmanaged) allocator" );
	unmanaged_arena.data			= &s_allocators->back();
	regions.push_back				( unmanaged_arena );

	{
		platform::regions_type::const_iterator i		= regions.begin( );
		platform::regions_type::const_iterator const e	= regions.end( );
		for ( ; i != e; ++i )
			(static_cast<allocator_data*>((*i).data))->arena_address	= (*i).address;
	}

	memory::initialize				( );
}

void xray::memory::register_allocator		( base_allocator& allocator, u64 const arena_size, pcstr const description )
{
	::allocator_data const temp		= { &allocator, arena_size, 0, description };
	s_allocators->push_back		( temp );
}

void xray::memory::initialize				( )
{
	ASSERT						( s_arena_size, "memory hasn't been preinitialized yet" );

	allocators_type::iterator i			= s_allocators->begin( );
	allocators_type::iterator const e	= s_allocators->end( );
	for ( ; i != e; ++i )
		(*i).allocator->initialize	( (*i).arena_address, (*i).arena_size, (*i).arena_id);

#if XRAY_USE_MEMORY_MONITOR
	monitor::initialize			( );
#endif // #if XRAY_USE_MEMORY_MONITOR

#if XRAY_USE_MEMORY_LEAK_DETECTOR
	leak_detector::initialize	( );
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR

#if XRAY_USE_MEMORY_GUARD
	guard::initialize			( );
#endif // #if XRAY_USE_MEMORY_GUARD

	on_after_memory_initialized	( );

	memory::dump_statistics		( true );
}

void xray::memory::finalize				( )
{
	ASSERT						( s_arena_size, "memory hasn't been preinitialized yet" );

#if XRAY_USE_MEMORY_GUARD
	guard::finalize				( );
#endif // #if XRAY_USE_MEMORY_GUARD

#if XRAY_USE_MEMORY_LEAK_DETECTOR
	leak_detector::finalize		( );
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR

#if XRAY_USE_MEMORY_MONITOR
	monitor::finalize			( );
#endif // #if XRAY_USE_MEMORY_MONITOR

//.	memory::dump_statistics		( );

	allocators_type::reverse_iterator i			= s_allocators->rbegin( );
	allocators_type::reverse_iterator const e	= s_allocators->rend( );
	for ( ; i != e; ++i ) {
		if ( !(*i).arena_size )
			continue;

		(*i).allocator->finalize( );
		platform::free_region	( (*i).arena_address, (*i).arena_size );
	}

	XRAY_DESTROY_REFERENCE		( s_allocators );
	XRAY_DESTROY_REFERENCE		( s_process_heap_walk );
}

void xray::memory::dump_statistics		( bool const dump_stats_for_empty_arenas_as_well )
{
#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
	u64 crt_allocated_size		= 0;
	u64 process_allocated_size	= 0;
	u64 total_size				= 0;
	u64 allocated_size			= 0;
	allocators_type::const_iterator	i = s_allocators->begin( );
	allocators_type::const_iterator	e = s_allocators->end( );
	for ( ; i != e; ++i ) {
		total_size					+= (*i).allocator->total_size( );
		u64 const size				= (*i).allocator->allocated_size( );
		allocated_size				+= size;

#ifndef XRAY_STATIC_LIBRARIES
		if ( &s_crt_allocator == (*i).allocator )
			crt_allocated_size		= size;
		else
#endif // #ifndef XRAY_STATIC_LIBRARIES
		{
			if ( &s_process_allocator == (*i).allocator )
				process_allocated_size	= size;
		}

		if ( size || dump_stats_for_empty_arenas_as_well )
			(*i).allocator->dump_statistics	( );
	}
#else // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR
	u64 crt_allocated_size		= (static_cast<base_allocator const&>(s_crt_allocator)).allocated_size( )
	u64 process_allocated_size	= (static_cast<base_allocator const&>(s_process_allocator)).allocated_size( )
	u64 const total_size		= (static_cast<base_allocator const&>(s_crt_allocator)).total_size( );
	u64 const allocated_size	= (static_cast<base_allocator const&>(s_crt_allocator)).allocated_size( );
#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR

	R_ASSERT_CMP				( allocated_size, >=, crt_allocated_size + process_allocated_size );
	u64 const xray_used			= allocated_size - (crt_allocated_size + process_allocated_size);
	LOG_INFO					( logging::format_message, "---------------overall memory stats---------------" );
	LOG_INFO					( logging::format_message, "xray: " XRAY_PRINTF_SPEC_LONG_LONG(10) " (%6.2f%%)", xray_used, total_size == 0.f ? 0.f : float(xray_used)/float(total_size)*100.f );
	LOG_INFO					( logging::format_message, "used: " XRAY_PRINTF_SPEC_LONG_LONG(10) " (%6.2f%%)", allocated_size, total_size == 0.f ? 0.f : float(allocated_size)/float(total_size)*100.f );
	LOG_INFO					( logging::format_message, "free: " XRAY_PRINTF_SPEC_LONG_LONG(10) " (%6.2f%%)", total_size - allocated_size, total_size == 0.f ? 0.f : float(total_size - allocated_size)/float(total_size)*100.f );
	LOG_INFO					( logging::format_message, "size: " XRAY_PRINTF_SPEC_LONG_LONG(10), total_size );
}

void xray::memory::copy					( mutable_buffer const & destination, const_buffer const & source )
{
	xray::memory::copy			( destination.c_ptr(), destination.size(), source.c_ptr(), source.size() );
}

void xray::memory::copy					( pvoid const destination, size_t const destination_size, pcvoid const source, size_t const size )
{
	XRAY_UNREFERENCED_PARAMETER	( destination_size );
	R_ASSERT					( destination_size >= size );
	memcpy						( destination, source, size );
}

void xray::memory::zero					( mutable_buffer const & destination )
{
	xray::memory::zero			( destination.c_ptr(), destination.size() );
}

void xray::memory::zero					( pvoid destination, size_t size_in_bytes )
{
	memset						( destination, 0, size_in_bytes );
}

void xray::memory::fill32					( mutable_buffer const & destination, u32 const value, u32 const count )
{
	xray::memory::fill32		( destination.c_ptr(), destination.size(), value, count );
}

void xray::memory::fill32				( pvoid const destination, size_t const destination_size_in_bytes, u32 const value, size_t const count )
{
	XRAY_UNREFERENCED_PARAMETER	( destination_size_in_bytes );
	R_ASSERT					( destination_size_in_bytes >= count*sizeof(u32) );

	for ( int* i = (int*)destination, *e = i + count; i != e; ++i )
		*i						= value;
}

void xray::memory::fill8					( mutable_buffer const & destination, u8 const value, u32 const count )
{
	xray::memory::fill8			( destination.c_ptr(), destination.size(), value, count );
}

void xray::memory::fill8				( pvoid destination, size_t const destination_size_in_bytes, u8 value, size_t count )
{
	XRAY_UNREFERENCED_PARAMETER	( destination_size_in_bytes );
	R_ASSERT					( destination_size_in_bytes >= count*sizeof(u8) );
	memset						( destination, value, count );
}

int xray::memory::compare				( const_buffer const & buffer1, const_buffer const & buffer2 )
{
	if ( buffer1.size() < buffer2.size() )
		return					-1;
	else if ( buffer2.size() < buffer1.size() )
		return					+1;

	if ( !buffer1.size() )
		return					0;

	return						memcmp(buffer1.c_ptr(), buffer2.c_ptr(), buffer1.size());
}

bool xray::memory::equal				( const_buffer const & buffer1, const_buffer const & buffer2 )
{
	return						!compare(buffer1, buffer2);
}

void xray::memory::iterate_allocators		( allocator_predicate_type const& predicate )
{
	allocators_type::const_iterator	i = s_allocators->begin( );
	allocators_type::const_iterator	e = s_allocators->end( );
	for ( u32 j = 0; i != e; ++i, ++j )
		predicate				( *(*i).allocator, j, (*i).arena_address, (*i).arena_size, (*i).arena_id );
}
