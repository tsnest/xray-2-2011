////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_monitor.h"
#include "memory_leak_detector.h"
#include "memory_guard.h"
#include <xray/memory_debug.h>
#include "memory.h"

namespace xray {
namespace memory {

base_allocator::base_allocator	(
#ifndef MASTER_GOLD
		bool const use_leak_detector
#endif // #ifndef MASTER_GOLD
	) :
	m_arena_start				( 0 ),
	m_arena_end					( 0 ),
	m_arena_id					( 0 )
#ifndef MASTER_GOLD
	, m_use_leak_detector		( use_leak_detector )
#endif // #ifndef MASTER_GOLD
{
}

void base_allocator::initialize	( pvoid arena, u64 size, pcstr arena_id )
{
	ASSERT						( !initialized ( ) );

	m_arena_id					= arena_id;
	m_arena_start				= arena;
	m_arena_end					= (u8*)arena + size;

	initialize_impl				( arena, size, arena_id );
}

void base_allocator::finalize	( )
{
	ASSERT						( initialized ( ) );

	finalize_impl				( );

	m_arena_start				= 0;
	m_arena_end					= 0;
	m_arena_id					= 0;
}

void base_allocator::do_register	( u64 const arena_size, pcstr const description )
{
	register_allocator			( *this, arena_size, description );
}

void base_allocator::dump_statistics( ) const
{
	u64 const total_size		= this->total_size( );
	u64 const allocated_size	= this->allocated_size( );

	LOG_INFO					( logging::format_message, "---------------memory stats for arena [%s]---------------", m_arena_id );
	LOG_INFO					( logging::format_message, "used: " XRAY_PRINTF_SPEC_LONG_LONG(10) " (%6.2f%%)", allocated_size, total_size == 0.f ? 0.f : float(allocated_size)/float(total_size)*100.f );
	LOG_INFO					( logging::format_message, "free: " XRAY_PRINTF_SPEC_LONG_LONG(10) " (%6.2f%%)", total_size - allocated_size, total_size == 0.f ? 0.f : float(total_size - allocated_size)/float(total_size)*100.f );
	LOG_INFO					( logging::format_message, 
		m_arena_start
		?
			"size: " XRAY_PRINTF_SPEC_LONG_LONG(10)
			", start address: 0x" XRAY_PRINTF_SPEC_LONG_LONG_HEX(09)
			", end address: 0x" XRAY_PRINTF_SPEC_LONG_LONG_HEX(09)
		: 
			"size: " XRAY_PRINTF_SPEC_LONG_LONG(10),
		total_size,
		(u64)*(size_t*)&m_arena_start,
		(u64)*(size_t*)&m_arena_end
	);
}

#if XRAY_USE_MEMORY_TOOLS

pvoid base_allocator::on_malloc		( pvoid buffer, size_t buffer_size, size_t previous_size, pcstr const description ) const
{
	if ( !buffer )
		return					buffer;

	XRAY_UNREFERENCED_PARAMETER	( previous_size );
	XRAY_UNREFERENCED_PARAMETER	( description );

	R_ASSERT_CMP				( buffer, >=, m_arena_start );
	R_ASSERT_CMP				( buffer, <, m_arena_end );
	R_ASSERT_CMP				( (ptrdiff_t)buffer_size, <=, ((u8*)m_arena_end - (u8*)m_arena_start), "memory corruption detected: buffer underrun" );

	buffer_size					= needed_size(buffer_size);

#if XRAY_USE_MEMORY_MONITOR
	monitor::on_alloc			( buffer, buffer_size, previous_size, description );
#endif // #if XRAY_USE_MEMORY_MONITOR
#if XRAY_USE_MEMORY_LEAK_DETECTOR
	if ( m_use_leak_detector )
		leak_detector::on_alloc	( buffer, buffer_size, previous_size, description );
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
#if XRAY_USE_MEMORY_GUARD
	guard::on_alloc				( buffer, buffer_size, previous_size, description );
#endif // #if XRAY_USE_MEMORY_GUARD

	return						buffer;
}

void base_allocator::on_free		( pvoid& buffer, bool can_clear ) const
{
	XRAY_UNREFERENCED_PARAMETER	( can_clear );

	R_ASSERT_CMP_U				( buffer, >=, m_arena_start );
	R_ASSERT_CMP_U				( buffer, <, m_arena_end );

#if XRAY_USE_MEMORY_GUARD
	guard::on_free				( buffer, can_clear );
#endif // #if XRAY_USE_MEMORY_GUARD
#if XRAY_USE_MEMORY_LEAK_DETECTOR
	if ( m_use_leak_detector )
		leak_detector::on_free	( buffer, can_clear );
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
#if XRAY_USE_MEMORY_MONITOR
	monitor::on_free			( buffer, can_clear );
#endif // #if XRAY_USE_MEMORY_MONITOR

	R_ASSERT_CMP_U				( buffer, >=, m_arena_start );
	R_ASSERT_CMP_U				( buffer, <, m_arena_end );
}

size_t base_allocator::needed_size ( size_t const size ) const 
{
	return
		size
#if XRAY_USE_MEMORY_MONITOR
		+ monitor::get_house_keeping_size()
#endif // #if XRAY_USE_MEMORY_MONITOR
#if XRAY_USE_MEMORY_LEAK_DETECTOR
		+ leak_detector::get_house_keeping_size()
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
#if XRAY_USE_MEMORY_GUARD
		+ guard::get_house_keeping_size()
#endif // #if XRAY_USE_MEMORY_GUARD
	;
}

#endif // #if XRAY_USE_MEMORY_TOOLS

size_t base_allocator::usable_size ( pvoid pointer ) const 
{
#if XRAY_USE_MEMORY_GUARD
	pointer						= guard::get_real_pointer( pointer );
#endif // #if XRAY_USE_MEMORY_GUARD
#if XRAY_USE_MEMORY_LEAK_DETECTOR
	if ( m_use_leak_detector )
		pointer					= leak_detector::get_real_pointer( pointer );
#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
#if XRAY_USE_MEMORY_MONITOR
	pointer						= monitor::get_real_pointer( pointer );
#endif // #if XRAY_USE_MEMORY_MONITOR
	return						usable_size_impl( pointer );
}

void base_allocator::call_initialize_impl	( base_allocator& allocator, pvoid arena, u64 size, pcstr arena_id )
{
	allocator.initialize_impl	( arena, size, arena_id );
}

void base_allocator::call_finalize_impl		( base_allocator& allocator )
{
	allocator.finalize_impl		( );
}

void base_allocator::copy					( base_allocator const& allocator )
{
	m_arena_start				= allocator.m_arena_start;
	m_arena_end					= allocator.m_arena_end;
	m_arena_id					= allocator.m_arena_id;
}

pvoid base_allocator::malloc_impl		( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return						call_malloc ( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
}

pvoid base_allocator::realloc_impl	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return						call_realloc ( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
}

void base_allocator::free_impl		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	call_free					( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

} // namespace memory
} // namespace xray