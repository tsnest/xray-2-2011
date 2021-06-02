////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !XRAY_USE_CRT_MEMORY_ALLOCATOR

#include <xray/memory_pthreads3_allocator.h>

#define USE_PT3_PREFIX
#include "ptmalloc3/malloc-2.8.3.h"
#include "virtual_alloc.h"

using xray::memory::pthreads3_allocator;

static char __stdcall out_of_memory			( mspace const space, void const* const parameter, int const first_time )
{
	XRAY_UNREFERENCED_PARAMETER		(space);
	XRAY_UNREFERENCED_PARAMETER		( parameter );
	if ( first_time )
		return						1;

	FATAL							( "not enough memory for arena [global mt allocator]" );
	return							0;
}

void pthreads3_allocator::initialize_impl	( pvoid buffer, u64 buffer_size, pcstr arena_id )
{
	initialize_virtual_alloc_arena	( &g_ptmalloc3_arena, buffer, buffer_size, 64*1024, arena_id, &out_of_memory, 0 );
}

void pthreads3_allocator::finalize_impl		( )
{
}

pvoid pthreads3_allocator::malloc_impl		( size_t const size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT							( initialized ( ) );
	size_t const real_size			= needed_size( size );
	pvoid const result				= pt3malloc( real_size );
	return							
		on_malloc(
			result,
			size,
			0,
			XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		);
}

pvoid pthreads3_allocator::realloc_impl		( pvoid pointer, size_t const new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	ASSERT							( initialized ( ) );
	if ( !new_size ) {
		free_impl					( pointer XRAY_CORE_DEBUG_PARAMETERS );
		return						( 0 );
	}
	
	size_t const previous_size		= pointer ? usable_size( pointer ) : 0;
	if ( pointer )
		on_free						( pointer, false );

	size_t const real_size			= needed_size( new_size );
	pvoid const result				= pt3realloc( pointer, real_size );

	return
		on_malloc(
			result, 
			new_size, 
			previous_size, 
			XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		);
}

void pthreads3_allocator::free_impl			( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT							( initialized ( ) );

	if ( !pointer )
		return;

	on_free							( pointer );

	pt3free							( pointer );
}

pvoid pthreads3_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid pthreads3_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void pthreads3_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl						( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

size_t pthreads3_allocator::total_size		( ) const
{
	return							( (u32)g_ptmalloc3_arena.total_size );
}

size_t pthreads3_allocator::usable_size_impl( pvoid pointer ) const
{
	return							xray_mspace_usable_size( pointer );
}

#include "ptmalloc3/malloc-private.h"

size_t pthreads3_allocator::allocated_size	( ) const
{
	u32 const pure_size			= ( (u32)(g_ptmalloc3_arena.total_size - g_ptmalloc3_arena.free_size) );
	if ( !pure_size )
		return					( 0 );

	u32 result					= (u32)pt3mallinfo( ).uordblks;
	u32 const min_size			= pad_request(sizeof(struct malloc_state)) + TOP_FOOT_SIZE + CHUNK_ALIGN_MASK + 1;
	ASSERT						( result >= min_size );
	result						-= min_size;
	return						( result );
}

#endif // #if !XRAY_USE_CRT_MEMORY_ALLOCATOR