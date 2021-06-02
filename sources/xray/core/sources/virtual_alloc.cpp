////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "virtual_alloc.h"

#if 1
void* virtual_alloc	(virtual_alloc_arena* arena, size_t size)
{
	size	= xray::math::align_up( size, (size_t)65536 );
	virtual_alloc_region*					i;
	for ( i = arena->first_free_region; i && (i->size < size); i = i->next_free_region ) ;

	if ( i ) {

		if ( i->size == size) {
			ASSERT							( arena->free_size >= size );
			arena->free_size				-= size;

			ASSERT							( arena->region_count );
			--arena->region_count;

			if ( i->previous_free_region )
				i->previous_free_region->next_free_region	= i->next_free_region;
			else
				arena->first_free_region	= i->next_free_region;

			if ( i->next_free_region )
				i->next_free_region->previous_free_region	= i->previous_free_region;


			return							(i);
		}

		ASSERT								( i->size > size );

		u32 const page_size					= 4096;
		u32 const request_size				= ( ( int( size ) - 1)/page_size + 1 )*page_size;

		ASSERT								( arena->free_size > request_size );
		arena->free_size					-= request_size;

		virtual_alloc_region* const region	= ( virtual_alloc_region* )( pstr( i ) + request_size );
		region->next_free_region			= i->next_free_region;
		region->previous_free_region		= i->previous_free_region;
		region->size						= i->size - request_size;

		if ( i->previous_free_region )
			i->previous_free_region->next_free_region	= region;
		else
			arena->first_free_region		= region;

		if ( i->next_free_region )
			i->next_free_region->previous_free_region	= region;

		return								( i );
	}

	if ( arena->out_of_memory_handler )
		arena->out_of_memory_handler		( arena, arena->out_of_memory_handler_parameter, 0);

	return									( pvoid(-1) );
}

int virtual_free	(virtual_alloc_arena* const arena, void* const pointer_raw, size_t size)
{
	ASSERT									( ( size % 65536 ) == 0 );
	ASSERT									( ( ( pbyte( pointer_raw ) - pbyte( arena->start_pointer ) ) % 65536 ) == 0 );
	arena->free_size						+= size;

	char* const pointer						= ( char* const )pointer_raw;
	virtual_alloc_region* i, *j;
	for ( i = arena->first_free_region, j = 0; i && ( pstr( i ) < pointer ); j = i, i = i->next_free_region ) {
		ASSERT								( !i->previous_free_region || ( i->previous_free_region->next_free_region == i) );
		ASSERT								( !i->next_free_region || ( i->next_free_region->previous_free_region == i) );
	}

	ASSERT									( pstr(i) != pointer );

	virtual_alloc_region* const	previous	= ( virtual_alloc_region* const )j;
	bool join_with_previous					= previous && ( pstr(previous) + previous->size == pointer );

	virtual_alloc_region* next				= ( virtual_alloc_region* const )i;
	bool join_with_next						= next && ( pointer + size == pstr(next) );
	
	if (join_with_previous && join_with_next) {
		--arena->region_count;

		previous->next_free_region			= next->next_free_region;
		previous->size						+= size + next->size;

		if (next->next_free_region)
			next->next_free_region->previous_free_region	= previous;

		return								( 0 );
	}

	if (join_with_previous) {
		previous->size						+= size;
		return								( 0 );
	}

	virtual_alloc_region* const new_region	= ( virtual_alloc_region* const )pointer_raw;
	new_region->previous_free_region		= previous;

	if (join_with_next) {
		new_region->next_free_region		= next->next_free_region;
		new_region->size					= size + next->size;
		next								= next->next_free_region;
	}
	else {
		new_region->next_free_region		= next;
		new_region->size					= size;
		++arena->region_count;
	}

	if (!previous)
		arena->first_free_region			= new_region;

	if (previous)
		previous->next_free_region			= new_region;

	if (next)
		next->previous_free_region			= new_region;

	return									( 0 );
}
#else // #if 0
void* virtual_alloc	(virtual_alloc_arena* arena, size_t size)
{
	void* const result	= VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if (!result)
		return			((void*)(-1));

	return				(result);
}

int virtual_free	(virtual_alloc_arena* arena, void* pointer, size_t size)
{
  MEMORY_BASIC_INFORMATION minfo;
  char* cptr = (char*)pointer;
  while (size) {
    if (VirtualQuery(cptr, &minfo, sizeof(minfo)) == 0)
      return -1;
    if (minfo.BaseAddress != cptr || minfo.AllocationBase != cptr ||
        minfo.State != MEM_COMMIT || minfo.RegionSize > size)
      return -1;
    if (VirtualFree(cptr, 0, MEM_RELEASE) == 0)
      return -1;
    cptr += minfo.RegionSize;
    size -= minfo.RegionSize;
  }
  return 0;
}
#endif // #if 0