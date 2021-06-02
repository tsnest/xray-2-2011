////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR
#include "memory_leak_detector.h"
#include <xray/memory_doug_lea_allocator.h>

#define USE_DL_PREFIX
#include "ptmalloc3/malloc-2.8.3.h"

using xray::memory::doug_lea_allocator;
using xray::memory::thread_id_const_bool;

doug_lea_allocator::doug_lea_allocator	(
		thread_id_const_bool const thread_id_const,
		bool const crash_after_out_of_memory,
		bool const return_null_after_out_of_memory,
		bool const use_guards		
#ifndef MASTER_GOLD
		, bool use_leak_detector
#endif // #ifndef MASTER_GOLD
	) :
#ifndef MASTER_GOLD
	super						( use_leak_detector ),
#endif // #ifndef MASTER_GOLD
	m_arena						( 0 ),
	m_user_thread_id			( threading::current_thread_id( ) ),
	m_crash_after_out_of_memory	( crash_after_out_of_memory ),
	m_return_null_after_out_of_memory( return_null_after_out_of_memory ),
	m_out_of_memory				( false ),
	m_use_guards				( use_guards ),
	m_user_thread_id_called		( false ),
	m_thread_id_const			( thread_id_const ),
	m_user_thread_logging_name	( "invalid thread id" )
{
}

static char __stdcall out_of_memory_with_crash	( mspace const space, void const* const parameter, int const first_time )
{
	XRAY_UNREFERENCED_PARAMETERS( parameter, space, first_time );

	LOG_ERROR					( "out of memory in arena \"%s\"", (( doug_lea_allocator* )parameter)->arena_id( ) );
	xray::memory::dump_statistics( );
	FATAL						( "not enough memory for arena [%s]", (( doug_lea_allocator* )parameter)->arena_id( ) );
	return						0;
}

static char __stdcall out_of_memory_silent		( mspace const space, void const* const parameter, int const first_time )
{
	XRAY_UNREFERENCED_PARAMETER	( parameter );
	XRAY_UNREFERENCED_PARAMETER	( space );
	if ( first_time )
		return					0;

	xray::memory::dump_statistics	( );
	FATAL						( "not enough memory for arena [%s]", (( doug_lea_allocator* )parameter)->arena_id( ) );
	return						1;
}

void doug_lea_allocator::user_thread_id	( u32 user_thread_id ) const
{
	if ( m_user_thread_id == user_thread_id )
		return;

	if ( m_thread_id_const )
		R_ASSERT							( !m_user_thread_id_called );

	threading::interlocked_exchange	( (threading::atomic32_value_type&)m_user_thread_id, user_thread_id );

	if ( m_thread_id_const )
		m_user_thread_id_called			=	true;
}

void doug_lea_allocator::user_current_thread_id	( ) const
{
	user_thread_id				( threading::current_thread_id( ) );
	m_user_thread_logging_name	= threading::current_thread_logging_name( );
}

void doug_lea_allocator::initialize_impl( pvoid arena, u64 arena_size, pcstr arena_id )
{
	XRAY_UNREFERENCED_PARAMETER	( arena_id );

	ASSERT						( !m_arena, "arena is not initialized or is initialized more than once" );
	m_arena						= create_xray_mspace_with_base( arena, arena_size, 0, m_crash_after_out_of_memory ? &out_of_memory_with_crash : &out_of_memory_silent, (pcvoid)this );
}

void doug_lea_allocator::finalize_impl	( )
{
	ASSERT						( m_arena, "arena is not initialized or is initialized more than once" );
	destroy_mspace				( m_arena );
}

inline bool doug_lea_allocator::initialized	( ) const
{
	if ( !super::initialized( ) )
		return					( false );

	u32 const current_thread_id	= xray::threading::current_thread_id ( );
	XRAY_UNREFERENCED_PARAMETER	( current_thread_id );
	R_ASSERT_CMP				(
		current_thread_id, ==, m_user_thread_id,
		"thread[%s] calls allocator for thread[%s]",
		xray::threading::current_thread_logging_name( ),
		m_user_thread_logging_name
	);
	return						( true );
}

pvoid doug_lea_allocator::malloc_impl	( size_t const size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	if ( m_out_of_memory )
		return					0;

	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT						( initialized ( ) );
	size_t const real_size		= needed_size( size );
	pvoid const result			= xray_mspace_malloc( m_arena, real_size );
	if ( !result )
	{
		if ( !m_return_null_after_out_of_memory )
			LOG_ERROR			( "out of memory!!!!" );
	}
	m_out_of_memory				= m_return_null_after_out_of_memory & !result;
	return						( on_malloc	( result, size, 0, XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER ) );
}

pvoid doug_lea_allocator::realloc_impl	( pvoid pointer, size_t const new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	m_out_of_memory				= m_out_of_memory && (new_size != 0);

	ASSERT						( initialized ( ) );
	if ( !new_size ) {
		free_impl				( pointer XRAY_CORE_DEBUG_PARAMETERS );
		return					( 0 );
	}

	size_t const previous_size	= pointer ? usable_size( pointer ) : 0;
	if ( pointer )
		on_free					( pointer, false );

	size_t const real_size		= needed_size( new_size );
	pvoid const result			= xray_mspace_realloc( m_arena, pointer, real_size );
	return
		on_malloc(
			result,
			new_size,
			previous_size,
			XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		);
}

void doug_lea_allocator::free_impl		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT						( initialized ( ) );

	if ( !pointer )
		return;

	m_out_of_memory				= false;

	on_free						( pointer );

	xray_mspace_free			( m_arena, pointer );
}

pvoid doug_lea_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return						( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid doug_lea_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return						( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void doug_lea_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl					( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

size_t doug_lea_allocator::total_size	( ) const
{
	thread_id_const_bool saved_thread_id_const	=	m_thread_id_const;
	m_thread_id_const			= thread_id_const_false;

	u32 const owner_thread_id	= m_user_thread_id;
	user_current_thread_id		( );

	ASSERT						( initialized ( ) );
	size_t const result			= xray_mspace_mallinfo( m_arena ).usmblks;

	user_thread_id				( owner_thread_id );
	m_thread_id_const			= saved_thread_id_const;

	return						( result );
}

size_t doug_lea_allocator::usable_size_impl	( pvoid pointer ) const
{
	return						xray_mspace_usable_size( pointer );
}

#include "ptmalloc3/malloc-private.h"

size_t doug_lea_allocator::allocated_size( ) const
{
	thread_id_const_bool saved_thread_id_const	=	m_thread_id_const;
	m_thread_id_const			= thread_id_const_false;
	u32 const owner_thread_id	= m_user_thread_id;
	user_current_thread_id		( );

	ASSERT						( initialized ( ) );
	u32 result					= (u32)xray_mspace_mallinfo( m_arena ).uordblks;

	m_user_thread_id			= owner_thread_id;

	u32 const min_size			= pad_request( sizeof( malloc_state ) );
	ASSERT						( result >= min_size );
	result						-= min_size;
	m_thread_id_const			= saved_thread_id_const;
	return						( result );
}

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR