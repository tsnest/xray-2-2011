////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IO_COMPLETION_QUEUE_INLINE_HPP_INCLUDED
#define XBOX_IO_COMPLETION_QUEUE_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace detail {

template <typename T>
wait_queue<T>::wait_queue() :
	m_single_thread_guard(::GetCurrentThreadId())
{
}

template <typename T>
void wait_queue<T>::verify_single_thread() const
{
	BOOST_ASSERT(m_single_thread_guard == ::GetCurrentThreadId());
}

template <typename T>
void wait_queue<T>::push_back(T const & value, event_type* signal)
{
	verify_single_thread();
	m_operations.push_back(operation_item(value, signal));
}

template <typename T>
bool wait_queue<T>::empty() const
{
	verify_single_thread();
	return m_operations.empty();
}


template <typename T>
event_type* wait_queue<T>::wait_for_pop(T & dest_result,
										event_handles_type & wait_events,
										unsigned int wait_time_ms)
{
	verify_single_thread();
	
	event_handles_type::size_type ecount = wait_events.size();
	if (ecount != m_operations.size())
	{
		update_wait_events	(wait_events);
		ecount				= wait_events.size();
	}
	
	BOOST_ASSERT(
		verify_queues_equation(wait_events) &&
		(wait_events.size() == m_operations.size()));
	
	DWORD wait_result = ::WaitForMultipleObjects(
		static_cast<DWORD>(ecount),
		static_cast<HANDLE*>(wait_events.begin()),
		FALSE,
		wait_time_ms);

	if (wait_result == WAIT_TIMEOUT)
	{
		return NULL;
	}
	
	BOOST_ASSERT(wait_result < (WAIT_OBJECT_0 + ecount));
	if (wait_result >= (WAIT_OBJECT_0 + ecount))
	{
		LOG_ERROR("wait failed");
		return NULL;
	}
	
	int								index		= wait_result - WAIT_OBJECT_0;
	operations_type::iterator		oper_iter	= m_operations.begin();
	event_handles_type::iterator	event_iter	= wait_events.begin();
	
	std::advance				(oper_iter, index);
	std::advance				(event_iter, index);
	
	dest_result					= oper_iter->m_operation;
	event_type* result			= oper_iter->m_signal;
	
	m_operations.erase			(oper_iter);
	wait_events.erase			(event_iter);
	
	return result;
}

template <typename T>
void wait_queue<T>::update_wait_events(event_handles_type & wait_events)
{
	bool the_same		= true;
	wait_events.resize	(m_operations.size(), NULL);

	event_handles_type::iterator dst_iter = wait_events.begin();
	for (operations_type::const_iterator i = m_operations.begin(),
		ie = m_operations.end(); i != ie; ++i)
	{
		if (!the_same || (i->m_signal->get_handle() != *dst_iter))
		{
			the_same	= false;
			*dst_iter	= i->m_signal->get_handle();
		}
		++dst_iter;
	}
}

template <typename T>
bool wait_queue<T>::verify_queues_equation(event_handles_type const & wait_events) const
{
	event_handles_type::const_iterator	we_iter = wait_events.begin();
	for (operations_type::const_iterator i = m_operations.begin(),
		ie = m_operations.end(); i != ie; ++i)
	{
		if (i->m_signal->get_handle() != *we_iter)
		{
			return false;
		}
		++we_iter;
	}
	return true;
}


inline xbox_io_completion_queue::xbox_io_completion_queue() :
	m_single_thread_guard(::GetCurrentThreadId())
{
}

inline xbox_io_completion_queue::~xbox_io_completion_queue()
{
}

inline void	xbox_io_completion_queue::verify_single_thread() const
{
	BOOST_ASSERT(m_single_thread_guard == ::GetCurrentThreadId());
}

inline event_type & xbox_io_completion_queue::register_operation(HANDLE const socket_handle,
																 operation * const operation_ptr,
																 unsigned int operation_type)
{
	verify_single_thread();
	async_operation						src_op;
	src_op.m_handle						= socket_handle;
	src_op.m_operation_ptr				= operation_ptr;
	src_op.m_key						= operation_type;
	event_type*	tmp_event				= XN_NEW(event_type);
	m_overlapped_wait_events.push_back	(src_op, tmp_event);
	return *tmp_event;
}

inline bool xbox_io_completion_queue::get_queued_result(async_operation & dest,
												 unsigned int const wait_time_ms)
{
	verify_single_thread();
	event_type*	tmp_event = m_overlapped_wait_events.wait_for_pop(
		dest, m_last_wait_events, wait_time_ms);
	if (tmp_event)
	{
		XN_DELETE(tmp_event);
		return true;
	}
	return false;
}

inline size_t xbox_io_completion_queue::empty() const
{
	verify_single_thread();
	return m_overlapped_wait_events.empty();
}


} // namespace detail
} // namespace asio
} // namespace boost

#endif // #ifndef XBOX_IO_COMPLETION_QUEUE_INLINE_HPP_INCLUDED