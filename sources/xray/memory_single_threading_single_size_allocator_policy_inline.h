////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_SINGLE_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED
#define XRAY_MEMORY_SINGLE_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED

namespace xray {
namespace memory {

template < typename T >
void single_threading_single_size_allocator_policy< T >::initialize	( free_list_type& free_list_head )
{
	free_list_head.set_pointer	( 0 );
}
	
template < typename T >
T* single_threading_single_size_allocator_policy< T >::allocate		( free_list_type& free_list_head )
{
	T* allocated_node			= free_list_head.get_pointer();
	CURE_ASSERT					( allocated_node, return 0, "single_size_buffer_allocator - out of memory!");
	free_list_head.set_pointer	( allocated_node->next );
	return						allocated_node;
}

template < typename T >
void single_threading_single_size_allocator_policy< T >::deallocate	(
		free_list_type& free_list_head,
		free_list_type& freeing_node
	)
{
	freeing_node.get_pointer()->next	= free_list_head.get_pointer();
	free_list_head						= freeing_node; 	
}

template < typename T >
void single_threading_single_size_allocator_policy< T >::increment	( counter_type& operand )
{
	++operand;
}

template < typename T >
void single_threading_single_size_allocator_policy< T >::decrement	( counter_type& operand )
{
	--operand;
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_SINGLE_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED