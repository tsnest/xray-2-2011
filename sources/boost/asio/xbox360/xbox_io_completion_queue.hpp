////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IO_COMPLETION_QUEUE_HPP_INCLUDED
#define XBOX_IO_COMPLETION_QUEUE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace detail {

//forward declaration
class operation;

struct event_type
{
	void	set				(bool state)	{ m_event_impl.set(state); }
	HANDLE	get_handle		() 				{ return m_event_impl.get_handle(); }
	xray::threading::event	m_event_impl;
	//event_type*				m_prev;
	//event_type*				m_next;
}; //struct event_type

template <typename T>
class wait_queue
{
public:
	typedef xray::network::vector<HANDLE> event_handles_type;

			wait_queue	();
			~wait_queue	()	{}

	void		push_back		(T const & value, event_type* signal);
	event_type*	wait_for_pop	(T & dest_result,
								 event_handles_type & wait_events,
								 unsigned int wait_time_ms = INFINITE);
	bool		empty			() const;
private:
	void	update_wait_events(event_handles_type & wait_events);
	struct operation_item
	{
		operation_item		(T const & operation, event_type* signal) :
			m_signal	(signal),
			m_operation	(operation)
		{
		}
		event_type*			m_signal;
		T					m_operation;
	}; //struct operation_item

	typedef xray::network::vector<operation_item>	operations_type;
	
	/* if the queue will use several threads, then interrupter event
	is required, because if the one thread pushes a wait event to queue
	and second(worker) thread is sleeping in WaitForMultipleObjects then
	there will be a lag.*/
	//event_type										m_interrupter;

	operations_type		m_operations;
	
	void				verify_single_thread	() const;
	bool				verify_queues_equation	(event_handles_type const & wait_events) const;
	DWORD	const		m_single_thread_guard;
}; //class wait_queue
	
class xbox_io_completion_queue
{
public:
	struct async_operation
	{
		HANDLE			m_handle;
		operation*		m_operation_ptr;
		unsigned int	m_key;
	}; //struct async_operation

	inline xbox_io_completion_queue				();
	inline ~xbox_io_completion_queue			();

	inline event_type &	register_operation		(HANDLE const socket_handle,
												 operation * const operation_ptr,
												 unsigned int operation_type);
	inline bool			get_queued_result		(async_operation & dest,
												 unsigned int const wait_time_ms = INFINITE);
	inline size_t		empty					() const;
private:
	
	typedef wait_queue<async_operation>				wait_queue_type;
	typedef wait_queue_type::event_handles_type		wait_events_type;
		
	wait_queue_type		m_overlapped_wait_events;
	wait_events_type	m_last_wait_events;
	
	void				verify_single_thread	() const;
	DWORD	const		m_single_thread_guard;
}; // class xbox_io_completion_queue

} // namespace detail
} // namespace asio
} // namespace boost

#include "xbox_io_completion_queue_inline.hpp"

#endif // #ifndef XBOX_IO_COMPLETION_QUEUE_HPP_INCLUDED