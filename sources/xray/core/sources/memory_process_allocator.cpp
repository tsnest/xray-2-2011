////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_monitor.h"
#include <new.h>
#include "memory_process_allocator.h"

using xray::memory::process_allocator;

void process_allocator::initialize_impl	( pvoid buffer, u64 buffer_size, pcstr arena_id )
{
	XRAY_UNREFERENCED_PARAMETERS( buffer, buffer_size, arena_id );
}

void process_allocator::finalize_impl	( )
{
}

pvoid process_allocator::malloc_impl	( size_t const size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_UNREFERENCED_PARAMETER			( size );
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT							( initialized ( ) );

	pvoid const result				= 0;

	return							
		on_malloc(
			result,
			size,
			0,
			XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		);
}

pvoid process_allocator::realloc_impl	( pvoid pointer, size_t const new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	ASSERT							( initialized ( ) );
	if ( !new_size ) {
		free_impl					( pointer XRAY_CORE_DEBUG_PARAMETERS );
		return						( 0 );
	}

	size_t const previous_size		= pointer ? usable_size( pointer ) : 0;
	if ( pointer )
		on_free						( pointer, false );

	pvoid const result				= 0;

	return							
		on_malloc					(
			result, 
			new_size, 
			previous_size, 
			XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER
		);
}

void process_allocator::free_impl		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;

	ASSERT							( initialized ( ) );

	if ( !pointer )
		return;

	on_free							( pointer );
}

pvoid process_allocator::call_malloc	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

pvoid process_allocator::call_realloc	( pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return							( realloc_impl( pointer, new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS ) );
}

void process_allocator::call_free		( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	free_impl						( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

size_t process_allocator::usable_size_impl	( pvoid ) const
{
	NOT_IMPLEMENTED					( return 0 );
}