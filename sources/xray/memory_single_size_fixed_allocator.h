////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_H_INCLUDED
#define XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_H_INCLUDED

#include <xray/memory_single_size_buffer_allocator.h>

namespace xray {
namespace memory {

template < int DataSize, int Count, typename ThreadingPolicy >
class single_size_fixed_allocator
{
public:	
	inline				single_size_fixed_allocator	( );

	inline	pvoid		malloc_impl	( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );
	inline	void		free_impl	( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION );

private:
	single_size_buffer_allocator< DataSize, ThreadingPolicy >	m_single_size_allocator;
	char														m_buffer[DataSize * Count];
}; // class single_size_fixed_allocator

} // namespace memory
} // namespace xray

#include <xray/memory_single_size_fixed_allocator_inline.h>

#endif // #ifndef XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_H_INCLUDED