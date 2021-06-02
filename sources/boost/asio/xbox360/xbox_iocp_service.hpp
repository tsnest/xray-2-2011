////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IOCP_SERVICE_HPP_INCLUDED
#define XBOX_IOCP_SERVICE_HPP_INCLUDED


#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/detail/socket_types.hpp> // Must come before posix_time.

#include <boost/asio/detail/push_options.hpp>
#include <cstddef>
#include <boost/config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/asio/detail/pop_options.hpp>
#include <boost/asio/detail/push_options.hpp>
#include <limits>
#include <boost/throw_exception.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/detail/pop_options.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include <boost/asio/detail/fd_set_adapter.hpp>
#include <boost/asio/detail/mutex.hpp>
#include <boost/asio/detail/noncopyable.hpp>
#include <boost/asio/detail/service_base.hpp>
#include <boost/asio/detail/signal_blocker.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/detail/task_io_service.hpp>
#include <boost/asio/detail/thread.hpp>
#include <boost/asio/detail/timer_queue.hpp>

#include "xbox_io_completion_queue.hpp"

namespace boost {
namespace asio {
namespace detail {

class xbox_iocp_service;
class operation
{
public:

	inline			operation		(xbox_iocp_service & iocp_service);										 
	virtual void	do_completion	() = 0;
	virtual void	destroy			() = 0;
	virtual	void	fetch_result	(HANDLE file_handle, boost::system::error_code & ec) = 0;
	LPOVERLAPPED 	overlapped_ptr	() { return &m_overlapped; };
protected:
	// Prevent deletion through this type.
			~operation		();
private:
	OVERLAPPED						m_overlapped;
	long volatile *					m_outstanding_operations_ptr;
}; // class operation

class xbox_iocp_service :
	public service_base<xbox_iocp_service>
{
public:
	
	template<typename Handler>
	class handler_operation : public operation
	{
	public:
						handler_operation	(xbox_iocp_service & iocp_service,
											 Handler const & h);
	private:
		virtual void	do_completion		();
		virtual void	destroy				();
		virtual	void	fetch_result		(HANDLE file_handle, boost::system::error_code & ec);
		
		Handler			m_handler;
	}; // class handler_operation
	
	typedef event_type		overlapped_event_type;

	// Constructor
	inline 					xbox_iocp_service			(boost::asio::io_service & ioservice);
	// Initializes an io completion port
	inline void				init						(size_t concurrency_hint);
	// Destroy all user-defined handler objects owned by the service.
	inline void				shutdown_service			();
	// Initialise the task, if required.
	inline void				init_task					();
	// Run the event loop until interrupted or no more work.
	inline size_t			run							(boost::system::error_code& ec);
	// Run until interrupted or one operation is performed.
	inline size_t			run_one						(boost::system::error_code& ec);
	// Poll for operations without blocking.
	inline size_t			poll						(boost::system::error_code& ec);
	// Poll for one operation without blocking.
	inline size_t			poll_one					(boost::system::error_code& ec);
	// Interrupt the event processing loop.
	inline void				stop						();
	// Reset in preparation for a subsequent run invocation.
	inline void				reset						();
	// Notify that some work has started.
	inline void				work_started				();
	// Notify that some work has finished.
	inline void				work_finished				();
	// Request invocation of the given handler.
	template <typename Handler>
	void					dispatch					(Handler handler);
	// Request invocation of the given handler and return immediately.
	template <typename Handler>
	void					post						(Handler handler);
	// Add a new timer queue to the service.
	template <typename Time_Traits>
	void					add_timer_queue				(timer_queue<Time_Traits>& timer_queue);
	// Remove a timer queue from the service.
	template <typename Time_Traits>
	void					remove_timer_queue			(timer_queue<Time_Traits>& timer_queue);
	// Schedule a timer in the given timer queue to expire at the specified
	// absolute time. The handler object will be invoked when the timer expires.
	template <typename Time_Traits, typename Handler>
	void					schedule_timer				(timer_queue<Time_Traits>& timer_queue,
														 const typename Time_Traits::time_type& time,
														 Handler handler,
														 void* token);
	// Cancel the timer associated with the given token. Returns the number of
	// handlers that have been posted or dispatched.
	template <typename Time_Traits>
	std::size_t				cancel_timer				(timer_queue<Time_Traits>& timer_queue,
														 void* token);

	inline overlapped_event_type &	start_operation		(HANDLE const socket_handle,
														 operation * const oper);
	
	long volatile &					outstanding_operations() { return m_outstanding_operations; };
private:
	typedef xray::network::vector<timer_queue_base*>	timer_queues_t;
	
	struct timer_euque_predicate
	{
		bool operator()	(timer_queues_t::value_type const & left,
						 timer_queues_t::value_type const & right) const;
	};


	enum enum_completion_keys
	{
		eck_overlapped			= 0x00,
		eck_timers_rearrange,
		eck_stop
	}; // enum enum_completion_keys
	static const DWORD		max_timeout = 1000; // 1000 milliseconds 

	size_t					do_one						(bool blocking_mode,
														 boost::system::error_code& ec);
	void					post_timer_rearrange		();
	DWORD					get_timeout					();
	bool					all_timers_empty			();
	void					dispatch_timers				();

	long volatile				m_outstanding_operations;
	long volatile				m_stop_signal;
	long volatile				m_shutdown_signal;
	xbox_io_completion_queue	m_completion_queue;
	xray::threading::mutex		m_timer_queues_mutex;
	timer_queues_t				m_timer_queues;
	// thread id that waits for timers
	long volatile				m_timer_thread_id;
	bool						m_timer_is_interrupting;
}; // class xbox_iocp_service

} // namespace detail
} // namespace asio
} // namespace boost

#include "xbox_iocp_service_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>


#endif // #ifndef XBOX_IOCP_SERVICE_HPP_INCLUDED