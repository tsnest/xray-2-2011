////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if 1//XRAY_DEBUG_ALLOCATOR
#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
#include "memory_leak_detector.h"
#include <xray/memory_doug_lea_mt_allocator.h>

using xray::memory::doug_lea_mt_allocator;

class mutex_mt_raii : private boost::noncopyable {
public:
	inline	mutex_mt_raii	( doug_lea_mt_allocator const& instance ) :
		m_instance( instance )
	{
		m_instance.mutex().lock				( );
		m_instance.user_current_thread_id	( );
	}

	inline	~mutex_mt_raii	( )
	{
		m_instance.mutex().unlock			( );
	}

private:
	doug_lea_mt_allocator const& m_instance;
}; // class mutex_mt_raii

doug_lea_mt_allocator::doug_lea_mt_allocator(
		bool const crash_after_out_of_memory,
		bool const return_null_after_out_of_memory,
		bool const use_guards
#ifndef MASTER_GOLD
		, bool const use_leak_detector
#endif // #ifndef MASTER_GOLD
	) :
	super	(
		xray::memory::thread_id_const_false,
		crash_after_out_of_memory,
		return_null_after_out_of_memory,
		use_guards		
#ifndef MASTER_GOLD
		, use_leak_detector
#endif // #ifndef MASTER_GOLD
	)
{
}

pvoid doug_lea_mt_allocator::malloc_impl	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	mutex_mt_raii	guard( *this );
	return		super::malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
}

pvoid doug_lea_mt_allocator::realloc_impl	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	mutex_mt_raii	guard( *this );
	return		super::realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
}

void doug_lea_mt_allocator::free_impl		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	mutex_mt_raii	guard( *this );
	super::free_impl( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

pvoid doug_lea_mt_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return		( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid doug_lea_mt_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return		( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void doug_lea_mt_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl	( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

void doug_lea_mt_allocator::initialize		( pvoid arena, u64 arena_size, pcstr arena_id )
{
	if ( !arena )
		return;

	super::initialize	( arena, arena_size, arena_id );
}

void doug_lea_mt_allocator::initialize_impl	( pvoid arena, u64 arena_size, pcstr arena_id )
{
	if ( !arena )
		return;

	mutex_mt_raii	guard( *this );
	super::initialize_impl	( arena, arena_size, arena_id );
}

void doug_lea_mt_allocator::finalize_impl	( )
{
	mutex_mt_raii	guard( *this );
	super::finalize_impl	( );
}

size_t doug_lea_mt_allocator::total_size	( ) const
{
	mutex_mt_raii	guard( *this );
	return	super::total_size( );
}

size_t doug_lea_mt_allocator::allocated_size( ) const
{
	mutex_mt_raii	guard( *this );
	return	super::allocated_size( );
}

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR
#endif // #if XRAY_DEBUG_ALLOCATOR