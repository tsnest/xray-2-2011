////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_INLINE_H_INCLUDED
#define XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE				template <class T, class ForwardQueueType, typename NullNodeType> inline
#define	TEMPLATE_SPECIALIZATION			one_way_threads_channel_with_response<T, ForwardQueueType, NullNodeType>

namespace xray {

TEMPLATE_SIGNATURE
TEMPLATE_SPECIALIZATION::one_way_threads_channel_with_response	() 
	:	m_backward_queue_tls_key			(threading::tls_create_key()), 
		m_backward_queue_allocator_tls_key	(threading::tls_create_key())	
{
}

TEMPLATE_SIGNATURE
TEMPLATE_SPECIALIZATION::~one_way_threads_channel_with_response	() 
{ 
	threading::tls_delete_key				(m_backward_queue_tls_key); 
	threading::tls_delete_key				(m_backward_queue_allocator_tls_key);
}
	
TEMPLATE_SIGNATURE
void   TEMPLATE_SPECIALIZATION::consumer_finalize ()
{
	if ( !m_forward_queue.initialized() )
		return;
	pointer_type null_node				=	m_forward_queue->pop_null_node();
	backward_queue_type * backward_queue =	null_node->backward_queue();
	if ( !null_node->in_backward_queue() )
		backward_queue->push_back			(null_node);
}

TEMPLATE_SIGNATURE
typename TEMPLATE_SPECIALIZATION::pointer_type   TEMPLATE_SPECIALIZATION::consumer_pop_to_process ()
{
	if ( !m_forward_queue.initialized() )
		return								NULL;
	
	if ( m_forward_queue->pop_thread_id() == u32(-1) )
		m_forward_queue->set_pop_thread_id	();

	pointer_type	to_delete			=	NULL;
	pointer_type	to_return			=	m_forward_queue->pop_front(to_delete);
	if ( to_delete && !to_delete->in_backward_queue() )
		consumer_push_processed				(to_delete);

	return									to_return;
}

TEMPLATE_SIGNATURE
void   TEMPLATE_SPECIALIZATION::consumer_push_processed	(pointer_type const value)
{
	backward_queue_type * const backward_queue	=	value->backward_queue();
	if ( backward_queue->push_thread_id() == u32(-1) )
		backward_queue->set_push_thread_id	();

	value->set_in_backward_queue			();
	backward_queue->push_back				(value);
}

TEMPLATE_SIGNATURE
void   TEMPLATE_SPECIALIZATION::producer_finalize ()
{
	if ( !m_forward_queue.initialized() )
		return;
	R_ASSERT								(!m_forward_queue->initialized(), "first you have to finalize consumer");
	backward_queue_type * backward_queue =	backward_queue_for_current_thread(NULL);
	if ( !backward_queue )
		return;

	pointer_type null_node				=	backward_queue->pop_null_node();
	memory::base_allocator * const null_node_allocator	=	null_node->allocator();
	memory::base_allocator * const queue_allocator	=	(memory::base_allocator *)threading::tls_get_value(m_backward_queue_allocator_tls_key);
	XRAY_DELETE_IMPL						(null_node_allocator, null_node);
	XRAY_DELETE_IMPL						(queue_allocator, backward_queue);
}

TEMPLATE_SIGNATURE
typename TEMPLATE_SPECIALIZATION::pointer_type   TEMPLATE_SPECIALIZATION::producer_pop_processed ()
{
	backward_queue_type * const	backward_queue	=	backward_queue_for_current_thread(NULL);
	if ( !backward_queue )
		return								NULL;

	pointer_type	to_delete			=	NULL;
	pointer_type	to_return			=	backward_queue->pop_front(to_delete);

	if ( to_delete )
	{
		memory::base_allocator * const allocator	=	to_delete->allocator();
		XRAY_DELETE_IMPL					(allocator, to_delete);
	}

	return									to_return;
}
	
TEMPLATE_SIGNATURE
void   TEMPLATE_SPECIALIZATION::producer_push_to_process (pointer_type const value)
{
	memory::base_allocator * value_allocator	=	value->allocator();
	backward_queue_type * const backward_queue	=	backward_queue_for_current_thread(value_allocator);
	value->set_backward_queue						(backward_queue);
	
	ForwardQueueType * forward_queue	=	m_forward_queue.initialized() ? 
												m_forward_queue.c_ptr() : forward_queue_initialize_if_needed(value_allocator, backward_queue);
	forward_queue->push_back				(value);
}

TEMPLATE_SIGNATURE
typename TEMPLATE_SPECIALIZATION::backward_queue_type *   TEMPLATE_SPECIALIZATION::backward_queue_for_current_thread (memory::base_allocator * allocator)
{
	backward_queue_type * backward_queue	=	(backward_queue_type *)threading::tls_get_value(m_backward_queue_tls_key);
	if ( backward_queue )
		return								backward_queue;

	if ( !allocator )
		return								NULL;

	backward_queue						=	XRAY_NEW_IMPL(allocator, backward_queue_type);
	backward_queue->set_pop_thread_id		();
	NullNodeType * const null_node		=	XRAY_NEW_IMPL(allocator, NullNodeType)(allocator);
	null_node->set_backward_queue			(backward_queue);

	backward_queue->set_push_thread_id		();
	backward_queue->push_null_node			(null_node);
	backward_queue->clear_push_thread_id	();

	threading::tls_set_value				(m_backward_queue_tls_key, backward_queue);
	threading::tls_set_value				(m_backward_queue_allocator_tls_key, allocator);

	return									backward_queue;
}

TEMPLATE_SIGNATURE
ForwardQueueType *   TEMPLATE_SPECIALIZATION::forward_queue_construct_predicate::operator () (pvoid buffer)
{
	forward_queue_type * const forward_queue	=	new (buffer) forward_queue_type();
	
	NullNodeType * const null_node		=	XRAY_NEW_IMPL(m_allocator, NullNodeType)(m_allocator);
	null_node->set_backward_queue			(m_backward_queue);
	forward_queue->push_null_node			(null_node);
	return									forward_queue;
}

TEMPLATE_SIGNATURE
ForwardQueueType *   TEMPLATE_SPECIALIZATION::forward_queue_initialize_if_needed (memory::base_allocator * allocator, backward_queue_type * backward_queue)
{
	if ( m_forward_queue.initialized() )
		return								m_forward_queue.c_ptr();
	
	XRAY_CONSTRUCT_REFERENCE_BY_PREDICATE_MT_SAFE	(m_forward_queue, forward_queue_construct_predicate(allocator, backward_queue));

	return									m_forward_queue.c_ptr();
}

} // namespace xray

#undef TEMPLATE_SIGNATURE
#undef TEMPLATE_SPECIALIZATION

#endif // #ifndef XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_INLINE_H_INCLUDED
