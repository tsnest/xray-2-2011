////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_H_INCLUDED
#define XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_H_INCLUDED

#include <xray/intrusive_spsc_queue.h>
#include <xray/intrusive_mpsc_queue.h>

namespace xray {

template <class T>
class XRAY_CORE_API threads_channel_query_base_helper 
{
public:
	threads_channel_query_base_helper	() 
		: m_next_forward(NULL), m_next_backward(NULL) {}

public:
	T *							m_next_forward;
	T *							m_next_backward;
};

template <class T>
class XRAY_CORE_API threads_channel_query_base : public threads_channel_query_base_helper<T> {
public:
	typedef threads_channel_query_base_helper<T>	super;
	typedef	intrusive_spsc_queue<T, super, & super::m_next_backward>	backward_queue_type;

public:
	threads_channel_query_base	(memory::base_allocator * allocator)
		: m_backward_queue(NULL), m_allocator(allocator), m_in_backward_queue(false) {;}
	
	backward_queue_type *		backward_queue	() const { return m_backward_queue; }
	memory::base_allocator *	allocator		() const { return m_allocator; }
	bool						in_backward_queue	() const { return m_in_backward_queue; }
	void						set_in_backward_queue	() { m_in_backward_queue = true; }

	void						set_backward_queue	(backward_queue_type * queue) { m_backward_queue = queue; }

private:
	backward_queue_type *		m_backward_queue;
	memory::base_allocator *	m_allocator;
	bool						m_in_backward_queue;
};

template <class T, class ForwardQueueType, typename NullNodeType>
class one_way_threads_channel_with_response : public core::noncopyable 
{
public:
	typedef threads_channel_query_base_helper<T>		query_base_with_next;
	typedef	ForwardQueueType							forward_queue_type;
	typedef	typename T::backward_queue_type				backward_queue_type;
	typedef typename forward_queue_type::pointer_type	pointer_type;

public:
	inline	one_way_threads_channel_with_response	();
	inline	~one_way_threads_channel_with_response	();
	
	//--------------------------------------------------
	// consumer
	//--------------------------------------------------

	inline	void			consumer_finalize			();
	inline	pointer_type	consumer_pop_to_process		();
	inline	void			consumer_push_processed		(pointer_type const value);

	inline	bool			consumer_empty				() const { return !m_forward_queue.initialized() || m_forward_queue->empty(); }

	//--------------------------------------------------
	// producer
	//--------------------------------------------------
	inline	void			producer_finalize			();
	inline	pointer_type	producer_pop_processed		();	
	inline	void			producer_push_to_process	(pointer_type const value);

	//--------------------------------------------------
	// implementation details
	//--------------------------------------------------
protected:
	inline	backward_queue_type *		backward_queue_for_current_thread	(memory::base_allocator * allocator);

	struct forward_queue_construct_predicate
	{
		forward_queue_construct_predicate(memory::base_allocator * allocator, backward_queue_type * backward_queue) 
			: m_allocator(allocator), m_backward_queue(backward_queue) {}

		forward_queue_type *   operator () (pvoid buffer);
	private:
		memory::base_allocator *		m_allocator;
		backward_queue_type *			m_backward_queue;
	};

	inline	forward_queue_type *		forward_queue_initialize_if_needed	(memory::base_allocator * allocator, backward_queue_type * backward_queue);

private:
	uninitialized_reference< forward_queue_type >	m_forward_queue;

	u32									m_backward_queue_tls_key;
	u32									m_backward_queue_allocator_tls_key;

}; // class mpsc_threads_channel

} // namespace xray

#include <xray/one_way_threads_channel_with_response_inline.h>

#endif // #ifndef XRAY_ONE_WAY_THREADS_CHANNEL_WITH_RESPONSE_H_INCLUDED