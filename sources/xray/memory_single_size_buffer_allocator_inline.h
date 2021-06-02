////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_INLINE_H_INCLUDED
#define XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_INLINE_H_INCLUDED

namespace xray {
namespace memory {

template < int DataSize, class ThreadingPolicy >
inline single_size_buffer_allocator< DataSize, ThreadingPolicy >::single_size_buffer_allocator(
		pvoid arena,
		size_t arena_size
	) :
	m_allocated_count			( 0 ),
	m_max_count					( 0 )
{
	m_max_count					= arena_size / element_size;
	CURE_ASSERT					( arena_size % element_size == 0, return, "address of arena isn't aligned or data size is improper" );

	PolicyType::initialize		( m_free_list_head );

	node* const begin			= static_cast< node* >( arena );
	node* const end				= begin + m_max_count;
	for ( node* i = begin; i != end; ++i )
		i->next					= ( i + 1 != end ) ? ( i + 1 ) : 0;

	m_free_list_head.set_pointer( begin );
}

template < int DataSize, class ThreadingPolicy >
inline pvoid single_size_buffer_allocator< DataSize, ThreadingPolicy >::allocate	( )
{
	node* allocated_node			= PolicyType::allocate( m_free_list_head );
	R_ASSERT						( allocated_node );
	PolicyType::increment			( m_allocated_count );

	return							&allocated_node->data;
}

template < int DataSize, class ThreadingPolicy >
inline void single_size_buffer_allocator< DataSize, ThreadingPolicy >::deallocate	( pvoid& pointer )
{
	free_list_type 						freeing_node;
	freeing_node.set_pointer			( static_cast< node* >( pointer ) );

	PolicyType::deallocate				( m_free_list_head, freeing_node );
	PolicyType::decrement				( m_allocated_count );

#ifndef MASTER_GOLD
#if 0
	typedef unsigned char byte;
	u32 tail							= DataSize % 4;
	int count							= ( DataSize - tail ) / 4;
	memory::fill32						( pointer, DataSize - tail, debug::freed_memory, count );
	
	for ( byte* i = (byte*)( ( (int*) pointer ) + count ), *e = i + tail; i != e; ++i )
		*i									= 0xDD;
#endif
#endif
	pointer								= 0;
}

template < int DataSize, class ThreadingPolicy >
inline pvoid single_size_buffer_allocator< DataSize, ThreadingPolicy >::malloc_impl( size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER	( XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
	R_ASSERT_U					( size == element_size );
	return						allocate();
}

template < int DataSize, class ThreadingPolicy >
inline void single_size_buffer_allocator< DataSize, ThreadingPolicy >::free_impl( pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	deallocate					( pointer );
}

template < int DataSize, class ThreadingPolicy >
inline void single_size_buffer_allocator< DataSize, ThreadingPolicy >::swap			( single_size_buffer_allocator& other )
{
	std::swap					( m_free_list_head,		other.m_free_list_head	);
	std::swap					( m_allocated_count,	other.m_allocated_count	);
	std::swap					( m_max_count,			other.m_max_count		);
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_SINGLE_SIZE_BUFFER_ALLOCATOR_INLINE_H_INCLUDED