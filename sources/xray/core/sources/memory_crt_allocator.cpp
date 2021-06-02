////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/memory_crt_allocator.h>
#include "memory_monitor.h"
#include <new.h>

using xray::memory::crt_allocator;

#ifdef _MSC_VER
static int out_of_memory				( size_t size )
{
	XRAY_UNREFERENCED_PARAMETER		( size );
	FATAL							( "not enough memory for arena [c runtime allocator][request=%d]", size );
	return							( 1 );
}
#endif // #ifdef _MSC_VER

void crt_allocator::initialize_impl	( pvoid buffer, u64 buffer_size, pcstr arena_id )
{
	XRAY_UNREFERENCED_PARAMETERS	(arena_id, buffer, buffer_size);

#ifdef _MSC_VER
	_set_new_mode					(1);
	_set_new_handler				(&out_of_memory);
#endif // #ifdef _MSC_VER
}

void crt_allocator::finalize_impl	( )
{
}

pvoid crt_allocator::malloc_impl	( size_t const size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

#ifdef DEBUG
	XRAY_UNREFERENCED_PARAMETER		( description );
#endif // #ifdef DEBUG

	ASSERT							( m_malloc_ptr );

	size_t const real_size			= size;//needed_size( size );
	pvoid const result				= (*m_malloc_ptr)( real_size );
	return							result;//( on_malloc	( result, size, 0, XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER ) );
}

pvoid crt_allocator::realloc_impl	( pvoid pointer, size_t const new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
#ifdef DEBUG
	XRAY_UNREFERENCED_PARAMETER		( description );
#endif // #ifdef DEBUG

	if ( !new_size ) {
		free_impl					( pointer XRAY_CORE_DEBUG_PARAMETERS );
		return						( 0 );
	}

//	u32 const previous_size			= pointer ? buffer_size( pointer ) : 0;
//	if ( pointer )
//		on_free						( pointer, false );

	ASSERT							( m_realloc_ptr );
	size_t const real_size			= new_size;//needed_size( new_size );
	pvoid const result				= (*m_realloc_ptr)( pointer, real_size );

	return							result;
		//on_malloc(
		//	result, 
		//	new_size, 
		//	previous_size, 
		//	XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		//);
}

void crt_allocator::free_impl		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	if ( !pointer )
		return;

//	on_free							( pointer );

	ASSERT							( m_free_ptr );
	(*m_free_ptr)					( pointer );
}

pvoid crt_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid crt_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void crt_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl						( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

size_t crt_allocator::usable_size_impl	( pvoid ) const
{
	NOT_IMPLEMENTED					( return 0 );
}
