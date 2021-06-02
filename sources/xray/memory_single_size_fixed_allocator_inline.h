////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_INLINE_H_INCLUDED
#define XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_INLINE_H_INCLUDED

namespace xray {
namespace memory {

template < int DataSize, int Count, typename ThreadingPolicy >
inline single_size_fixed_allocator< DataSize, Count, ThreadingPolicy >::single_size_fixed_allocator() :
	m_single_size_allocator		( m_buffer, DataSize * Count )
{
}

template < int DataSize, int Count, typename ThreadingPolicy >
inline pvoid single_size_fixed_allocator< DataSize, Count, ThreadingPolicy >::malloc_impl( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return m_single_size_allocator.malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION XRAY_CORE_DEBUG_PARAMETERS );
}

template < int DataSize, int Count, typename ThreadingPolicy >
inline void single_size_fixed_allocator< DataSize, Count, ThreadingPolicy >::free_impl( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	m_single_size_allocator.free_impl( pointer XRAY_CORE_DEBUG_PARAMETERS );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_SINGLE_SIZE_FIXED_ALLOCATOR_INLINE_H_INCLUDED