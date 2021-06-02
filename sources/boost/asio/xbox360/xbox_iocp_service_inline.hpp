////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IOCP_SERVICE_INLINE_HPP_INCLUDED
#define XBOX_IOCP_SERVICE_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace detail {

inline operation::operation(xbox_iocp_service & iocp_service) :
	m_outstanding_operations_ptr(&iocp_service.outstanding_operations())
{
	m_overlapped.Internal		= 0;
	m_overlapped.InternalHigh	= 0;
	m_overlapped.Offset			= 0;
	m_overlapped.OffsetHigh		= 0;
	m_overlapped.hEvent			= 0;
	::InterlockedIncrement		(m_outstanding_operations_ptr);
}

inline operation::~operation()
{
	::InterlockedDecrement(m_outstanding_operations_ptr);
}

template<typename Handler>
xbox_iocp_service::handler_operation<Handler>::handler_operation(xbox_iocp_service & iocp_service,
																 Handler const & h) :
	operation(iocp_service),
	m_handler(h)
{
}

template<typename Handler>
void xbox_iocp_service::handler_operation<Handler>::do_completion()
{
	typedef handler_alloc_traits<Handler, handler_operation>	alloc_traits;
	
	handler_ptr<alloc_traits>	ptr(m_handler, this);
	Handler						tmp_handler(m_handler);
	
	ptr.reset();

	boost_asio_handler_invoke_helpers::invoke(tmp_handler, &tmp_handler);
}

template<typename Handler>
void xbox_iocp_service::handler_operation<Handler>::destroy()
{
	typedef handler_alloc_traits<Handler, handler_operation>	alloc_traits;
	
	handler_ptr<alloc_traits>	ptr(m_handler, this);
	Handler						tmp_handler(m_handler);
	tmp_handler;
	
	ptr.reset();
}

template <typename Handler>
void xbox_iocp_service::handler_operation<Handler>::fetch_result(
	HANDLE file_handle, boost::system::error_code & ec)
{
	ec.clear();
}



inline xbox_iocp_service::xbox_iocp_service(boost::asio::io_service & ioservice) :
	service_base<xbox_iocp_service>(ioservice),
	m_outstanding_operations(0),
	m_stop_signal(0),
	m_shutdown_signal(0),
	m_timer_thread_id(0),
	m_timer_is_interrupting(false)
{
}

inline void xbox_iocp_service::init(size_t /*concurrency_hint*/)
{
}

inline xbox_iocp_service::overlapped_event_type	& xbox_iocp_service::start_operation(
	HANDLE const socket_handle, operation * const oper)
{
	overlapped_event_type & result = m_completion_queue.register_operation(
		socket_handle, oper, eck_overlapped);
	oper->overlapped_ptr()->hEvent = result.get_handle();
	BOOST_ASSERT(oper->overlapped_ptr()->hEvent);
	return result;
}

inline void xbox_iocp_service::shutdown_service()
{
	::InterlockedExchange(&m_shutdown_signal, 1);

	while (::InterlockedExchangeAdd(&m_outstanding_operations, 0))
	{
		xbox_io_completion_queue::async_operation tmp_operation;
		tmp_operation.m_operation_ptr = NULL;
		if (m_completion_queue.get_queued_result(tmp_operation, INFINITE))
		{
			if (tmp_operation.m_operation_ptr)
			{
				tmp_operation.m_operation_ptr->destroy();
			}
		}
	}
	xray::threading::mutex_raii		lock(m_timer_queues_mutex);
	for (timer_queues_t::iterator i = m_timer_queues.begin(),
		ie = m_timer_queues.end(); i != ie; ++i)
	{
		(*i)->destroy_timers();
	}
	m_timer_queues.clear();
}

inline void xbox_iocp_service::init_task()
{
}

inline size_t xbox_iocp_service::run(boost::system::error_code& ec)
{
	size_t n = 0;
	while (do_one(true, ec))
	{
		if (n != (std::numeric_limits<size_t>::max)())
		{
			++n;
		}
	}
	return n;
}

inline size_t xbox_iocp_service::run_one(boost::system::error_code& ec)
{
	return do_one(true, ec);
}

inline size_t xbox_iocp_service::poll(boost::system::error_code& ec)
{
	size_t n = 0;
	while (do_one(false, ec))
	  if (n != (std::numeric_limits<size_t>::max)())
		++n;
	return n;
}

inline size_t xbox_iocp_service::poll_one(boost::system::error_code& ec)
{
	return do_one(false, ec);
}

inline void xbox_iocp_service::stop()
{
	::InterlockedExchange(&m_stop_signal, 1);
	m_completion_queue.register_operation(
		NULL, NULL, eck_stop).set(true);
}

inline void xbox_iocp_service::reset()
{
	::InterlockedExchange(&m_stop_signal, 0);
}

inline void xbox_iocp_service::work_started()
{
	::InterlockedIncrement(&m_outstanding_operations);
}

inline void xbox_iocp_service::work_finished()
{
	if (::InterlockedDecrement(&m_outstanding_operations) == 0)
	{
		stop();
	}
}

template <typename Handler>
void xbox_iocp_service::dispatch(Handler handler)
{
	if (call_stack<xbox_iocp_service>::contains(this))
		boost_asio_handler_invoke_helpers::invoke(handler, &handler);
	else
		post(handler);
}

template <typename Handler>
void xbox_iocp_service::post(Handler handler)
{
	if (::InterlockedExchangeAdd(&m_shutdown_signal, 0))
	{
		return;
	}

	typedef handler_operation<Handler>					value_type;
    typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
    
	//allocating memory
	raw_handler_ptr<alloc_traits>	raw_ptr(handler); 
	//contructing handler_operation
	handler_ptr<alloc_traits>		ptr(raw_ptr, *this, handler);

    // Enqueue the operation on the I/O completion port.
	m_completion_queue.register_operation(
		NULL, ptr.release(), eck_overlapped).set(true);
}

inline void xbox_iocp_service::post_timer_rearrange()
{
	m_completion_queue.register_operation(
		NULL, NULL, eck_timers_rearrange).set(true);
}


	
template <typename Time_Traits>
void xbox_iocp_service::add_timer_queue(timer_queue<Time_Traits>& timer_queue)
{
	xray::threading::mutex_raii	lock(m_timer_queues_mutex);
	BOOST_ASSERT(
		std::find(
			m_timer_queues.begin(),
			m_timer_queues.end(), 
			static_cast<timer_queue_base*>(&timer_queue)) == m_timer_queues.end());
	m_timer_queues.push_back(&timer_queue);
}

template <typename Time_Traits>
void xbox_iocp_service::remove_timer_queue(timer_queue<Time_Traits>& timer_queue)
{
	xray::threading::mutex_raii	lock(m_timer_queues_mutex);
	timer_queues_t::iterator tmp_iter = std::find(
		m_timer_queues.begin(),
		m_timer_queues.end(),
		static_cast<timer_queue_base*>(&timer_queue));
	BOOST_ASSERT(tmp_iter != m_timer_queues.end());
	m_timer_queues.erase(tmp_iter);
}

template <typename Time_Traits, typename Handler>
void xbox_iocp_service::schedule_timer(timer_queue<Time_Traits>& timer_queue,
									   const typename Time_Traits::time_type& time,
									   Handler handler,
									   void* token)
{
	if (::InterlockedExchangeAdd(&m_shutdown_signal, 0))
	{
		return;
	}
	m_timer_queues_mutex.lock();
	if (timer_queue.enqueue_timer(time, handler, token))
    {
      if (!m_timer_is_interrupting)
      {
        m_timer_is_interrupting = true;
		m_timer_queues_mutex.unlock();
		post_timer_rearrange();
		return;
      }
    }
	m_timer_queues_mutex.unlock();
}
	
template <typename Time_Traits>
std::size_t	xbox_iocp_service::cancel_timer(timer_queue<Time_Traits>& timer_queue,
											void* token)
{
	if (::InterlockedExchangeAdd(&m_shutdown_signal, 0))
	{
		return 0;
	}

	m_timer_queues_mutex.lock();
	size_t cancelled = timer_queue.cancel_timer(token);
	if (!cancelled || m_timer_is_interrupting)
	{
		m_timer_queues_mutex.unlock();
		return 0;
	}
	m_timer_is_interrupting = true;
	m_timer_queues_mutex.unlock();
	post_timer_rearrange();
	return cancelled;
}

inline void xbox_iocp_service::dispatch_timers()
{
	xray::threading::mutex_raii		lock(m_timer_queues_mutex);
	for (timer_queues_t::iterator i = m_timer_queues.begin(),
		ie = m_timer_queues.end(); i != ie; ++i)
	{
		(*i)->dispatch_timers();
		(*i)->dispatch_cancellations();
		(*i)->complete_timers();
	}
}

inline size_t xbox_iocp_service::do_one(bool blocking_mode,
										boost::system::error_code& ec)
{
	call_stack<xbox_iocp_service>::context	ctx(this);
	if (::InterlockedExchangeAdd(&m_outstanding_operations, 0) == 0)
	{
		ec.clear();
		return 0;
	}
	
	size_t			result				= 0;
	long			this_thread_id		= static_cast<long>(xray::threading::current_thread_id());

	::InterlockedCompareExchange		(&m_timer_thread_id, this_thread_id, 0);
	bool			is_responsible_for_timer = 
		(::InterlockedExchangeAdd(&m_timer_thread_id, 0) == this_thread_id);
	
	DWORD			wait_time			= blocking_mode ? max_timeout : 0;
	if (is_responsible_for_timer)
	{
		wait_time = blocking_mode ? get_timeout() : 0;
	}
	
	while (1)
	{
		bool										skip_queue = false;
		if (m_completion_queue.empty())
		{
			skip_queue = true;
			Sleep(wait_time);
		}
		xbox_io_completion_queue::async_operation	io_result;
		if (!skip_queue && m_completion_queue.get_queued_result(io_result, wait_time))
		{
			ec.clear();
			switch (io_result.m_key)
			{
			case eck_overlapped:
				{
					io_result.m_operation_ptr->fetch_result	(io_result.m_handle, ec);
					io_result.m_operation_ptr->do_completion();
					result									= 1;
				}break;
			case eck_timers_rearrange:
				{
					// this is a case when several working threads holds the 
					// responsibility for timers
					m_timer_queues_mutex.lock	();
					m_timer_is_interrupting		= false;
					m_timer_queues_mutex.unlock	();
					::InterlockedExchange		(&m_timer_thread_id, this_thread_id);
					wait_time					= blocking_mode ? get_timeout() : 0;
					continue;
				}break;
			case eck_stop:
				{
					// pass stop signal to next waiting thread 
					if (m_stop_signal)
						stop();
				}break;
			}; // switch (completion_key)
			// do_one is complete for now...
			if (::InterlockedExchangeAdd(&m_timer_thread_id, 0) == this_thread_id)
			{
				dispatch_timers();
			}
			break;
		} else
		{
			if (::InterlockedExchangeAdd(&m_timer_thread_id, 0) == this_thread_id)
			{
				dispatch_timers();
				if (blocking_mode)
				{
					wait_time = get_timeout();
					continue;
				}
			}
			if (blocking_mode)
			{
				wait_time = max_timeout;
				continue;
			}
			break;
		}
	} 
	return result;
};

inline bool xbox_iocp_service::all_timers_empty()
{
	xray::threading::mutex_raii	lock(m_timer_queues_mutex);
	for (timer_queues_t::iterator i = m_timer_queues.begin(),
		ie = m_timer_queues.end(); i != ie; ++i)
	{
		if ((*i)->empty() == false)
		{
			return false;
		}
	}
	return true;
}


inline bool xbox_iocp_service::timer_euque_predicate::operator()(
	timer_queues_t::value_type const & left,
	timer_queues_t::value_type const & right) const
{
	return left->wait_duration() < right->wait_duration();
}

inline DWORD xbox_iocp_service::get_timeout()
{
	using namespace boost::posix_time;
	if (all_timers_empty())
		return max_timeout;

	timer_euque_predicate	min_predicate;
	
	timer_queues_t::iterator minwait_iter = std::min_element(
		m_timer_queues.begin(),
		m_timer_queues.end(),
		min_predicate);
	
	BOOST_ASSERT(minwait_iter != m_timer_queues.end());
	//BOOST_ASSERT((*minwait_iter)->wait_duration() >= time_duration());

	time_duration			tmp_td = (*minwait_iter)->wait_duration();
	DWORD					result = 1;
	if (tmp_td > time_duration())
	{
		result = tmp_td.total_milliseconds();
	} 
	return std::min(result, max_timeout);
}


} // namespace detail
} // namespace asio
} // namespace boost

#endif // #ifndef XBOX_IOCP_SERVICE_INLINE_HPP_INCLUDED