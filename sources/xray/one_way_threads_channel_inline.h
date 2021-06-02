////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ONE_WAY_THREADS_CHANNEL_INLINE_H_INCLUDED
#define XRAY_ONE_WAY_THREADS_CHANNEL_INLINE_H_INCLUDED

namespace xray {

template < typename ForwardQueueType, typename BackwardQueueType >
inline one_way_threads_channel< ForwardQueueType, BackwardQueueType >::one_way_threads_channel			(
		memory::base_allocator& owner_allocator
	) :
	m_owner_allocator					( owner_allocator )
{
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_initialize			(
		pointer_type const forward_queue_initial_value,
		pointer_type const backward_queue_initial_value
	)
{
	m_forward_queue.set_push_thread_id	( );
	m_forward_queue.push_null_node		( backward_queue_initial_value );

	m_backward_queue.set_pop_thread_id	( );
	m_backward_queue.push_null_node		( forward_queue_initial_value );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_finalize				( )
{
	m_forward_queue.set_pop_thread_id	( );
	while ( !m_forward_queue.empty() ) {
		pointer_type					value_to_delete;
		m_forward_queue.pop_front		( value_to_delete );
		delete_value					( value_to_delete );
	}

	owner_delete_processed_items		( );

	delete_value						( m_backward_queue.pop_null_node() );
	delete_value						( m_forward_queue.pop_null_node() );
}

template < typename ForwardQueueType, typename BackwardQueueType >
template < typename DeferredValuesContainerType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_finalize				(
		DeferredValuesContainerType& container
	)
{
	while ( !container.empty() ) {
		pointer_type const value_to_delete	= container.pop_front( );
		R_ASSERT						( value_to_delete );
		if ( value_to_delete != m_forward_queue.null_node() )
			delete_value				( value_to_delete );
	}

	owner_finalize						( );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_push_back				(
		pointer_type const value
	)
{
	m_forward_queue.push_back			( value );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_delete_processed_items( )
{
	while ( pointer_type item_to_delete = owner_get_next_value_to_delete() )
		delete_value					( item_to_delete );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline memory::base_allocator&
	one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_allocator						( ) const
{
	R_ASSERT							( &m_owner_allocator );
	return								m_owner_allocator;
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::user_initialize				( )
{
	m_forward_queue.set_pop_thread_id	( );
	m_backward_queue.set_push_thread_id	( );
}

template < typename ForwardQueueType, typename BackwardQueueType >
template < typename PredicateType >
inline typename one_way_threads_channel< ForwardQueueType, BackwardQueueType >::pointer_type
	one_way_threads_channel< ForwardQueueType, BackwardQueueType >::user_pop_front						( PredicateType const& predicate )
{
	pointer_type						value_to_delete;
	pointer_type const result			= m_forward_queue.pop_front( value_to_delete );
	if ( result && predicate(value_to_delete) )
		m_backward_queue.push_back		( value_to_delete );
	return								result;
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline typename one_way_threads_channel< ForwardQueueType, BackwardQueueType >::pointer_type
	one_way_threads_channel< ForwardQueueType, BackwardQueueType >::user_pop_front						( )
{
	return								user_pop_front (
		one_way_threads_channel_default_pop_front_predicate( )
	);
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline bool one_way_threads_channel< ForwardQueueType, BackwardQueueType >::user_is_queue_empty			( ) const
{
	return								m_forward_queue.empty( );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::delete_value				( pointer_type const value )
{
	pointer_type temp					= value;
	XRAY_DELETE_IMPL					( m_owner_allocator, temp );
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline typename one_way_threads_channel< ForwardQueueType, BackwardQueueType >::pointer_type
	one_way_threads_channel< ForwardQueueType, BackwardQueueType >::owner_get_next_value_to_delete		( )
{
	pointer_type						value_to_delete;
	if ( !m_backward_queue.pop_front( value_to_delete ) )
		return							0;

	return								value_to_delete;
}

template < typename ForwardQueueType, typename BackwardQueueType >
inline void one_way_threads_channel< ForwardQueueType, BackwardQueueType >::user_delete_deffered_value	( pointer_type value_to_delete )
{
	if ( value_to_delete != m_forward_queue.null_node() )
		m_backward_queue.push_back		( value_to_delete );
}

} // namespace xray

#endif // #ifndef XRAY_ONE_WAY_THREADS_CHANNEL_INLINE_H_INCLUDED