////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED
#define XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED

namespace xray {
namespace memory {

template < typename T >
void multi_threading_single_size_allocator_policy< T >::initialize	( free_list_type& free_list_head )
{
	free_list_head.set_pointer	( 0 );
	free_list_head.whole		= 0;
	free_list_head.counter		= 0;
}
	
template < typename T >
T* multi_threading_single_size_allocator_policy< T >::allocate		( free_list_type& free_list_head )
{
	free_list_type				allocated_node;
	free_list_type				new_head;
	do 
	{
		allocated_node			= free_list_head;
		if ( !allocated_node.get_pointer() )
			break;

		new_head.set_pointer	( allocated_node.get_pointer()->next );
		new_head.counter		= allocated_node.counter + 1;

	} while ( threading::interlocked_compare_exchange	(
			  free_list_head.whole,
			  new_head.whole,
			  allocated_node.whole
			) != allocated_node.whole );

	CURE_ASSERT	( allocated_node.get_pointer(), return 0, "single_size_buffer_allocator - out of memory!" );
	return		allocated_node.get_pointer();
}

template < typename T >
void multi_threading_single_size_allocator_policy< T >::deallocate	(
		free_list_type& free_list_head,
		free_list_type& freeing_node
	)
{
	free_list_type 							head;
	do
	{	
		head								=	free_list_head;
		freeing_node.get_pointer()->next	=	head.get_pointer();
		freeing_node.counter				=	head.counter;
	} while ( threading::interlocked_compare_exchange	(
			free_list_head.whole,
			freeing_node.whole,
			head.whole
		) != head.whole );
}

template < typename T >
void multi_threading_single_size_allocator_policy< T >::increment	( counter_type& operand )
{
	threading::interlocked_increment( operand );
}

template < typename T >
void multi_threading_single_size_allocator_policy< T >::decrement	( counter_type& operand )
{
	threading::interlocked_decrement( operand );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_MULTI_THREADING_SINGLE_SIZE_ALLOCATOR_POLICY_INLINE_H_INCLUDED