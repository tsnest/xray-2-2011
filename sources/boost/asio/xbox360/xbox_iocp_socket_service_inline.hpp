////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IOCP_SOCKET_SERVICE_INLINE_HPP_INCLUDED
#define XBOX_IOCP_SOCKET_SERVICE_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace detail {

template <typename Protocol>
xbox_iocp_socket_service<Protocol>::native_type::native_type(socket_type s) :
	m_socket(s)
{
}

template <typename Protocol>
xbox_iocp_socket_service<Protocol>::native_type::native_type(socket_type s,
															 const endpoint_type& ep) :
	m_socket(s),
	m_remote_endpoint(ep)
{
}

template <typename Protocol>
void xbox_iocp_socket_service<Protocol>::native_type::operator=(socket_type s)
{
	m_socket = s;
}

template <typename Protocol>
xbox_iocp_socket_service<Protocol>::native_type::operator socket_type() const
{
	return m_socket;
}

template <typename Protocol>
HANDLE xbox_iocp_socket_service<Protocol>::native_type::as_handle() const
{
	return reinterpret_cast<HANDLE>(m_socket);
}

template <typename Protocol>
bool xbox_iocp_socket_service<Protocol>::native_type::have_remote_endpoint() const
{
	return m_remote_endpoint;
}

template <typename Protocol>
typename xbox_iocp_socket_service<Protocol>::endpoint_type 
xbox_iocp_socket_service<Protocol>::native_type::remote_endpoint() const
{
	BOOST_ASSERT(m_remote_endpoint);
	return *m_remote_endpoint;
}

template <typename Protocol>
xbox_iocp_socket_service<Protocol>::implementation_type::implementation_type() :
	m_socket(invalid_socket),
	m_flags(0),
	m_next_ptr(NULL),
	m_prev_ptr(NULL)
{
}

////////////////////////////////////////////////////////////////////

template <typename BuffersSequence, int MaxCount>
struct wsabuf_buffers_sequence
{
	typedef typename BuffersSequence::const_iterator	src_const_iterator;
	typedef xray::fixed_vector<::WSABUF, MaxCount>		buffers_t;
	wsabuf_buffers_sequence(src_const_iterator from, src_const_iterator end)
	{
		m_summary_size = 0;
		for (int i = 0; (i < MaxCount) && (from < end); ++from, ++i)
		{
			::WSABUF								tmp_wsa_buff;
			typename BuffersSequence::value_type &	tmp_buff = *from;
			size_t									tmp_buff_size = 
				boost::asio::buffer_size(tmp_buff);
			tmp_buf.len			= static_cast<u_long>(tmp_buff_size);
			tmp_buf.buf			= boost::asio::buffer_cast<char*>(tmp_buff);
			m_summary_size		+= tmp_buff_size;
			m_buffers.push_back	(tmp_buf);
		}
	}

	LPWSABUF	buffers	()
	{
		return &m_buffers.front();
	}
	DWORD		count	()
	{
		return static_cast<DWORD>(m_buffers.size());
	}
	buffers_t		m_buffers;
	size_t			m_summary_size;
}; // struct wsabuf_const_buffers_sequence

template <typename BuffersSequence>
struct wsabuf_buffers_sequence<BuffersSequence, 1>
{
	typedef typename BuffersSequence::const_iterator	src_const_iterator;
	wsabuf_buffers_sequence(src_const_iterator from, src_const_iterator end)
	{
		m_summary_size		= 0;
		if (from != end)
		{
			typename BuffersSequence::value_type const & tmp_buff = *from;
			m_summary_size	= boost::asio::buffer_size(tmp_buff);
			m_buffer.len	= static_cast<u_long>(m_summary_size);
			// Windows API doesn't has ::WSABUF structure with const buf :(
			m_buffer.buf	= const_cast<char*>(
				boost::asio::buffer_cast<char const *>(tmp_buff));
			++from;
			BOOST_ASSERT(from == end);
		} else
		{
			m_buffer.len		= 0;
			m_buffer.buf		= NULL;
		}
	}

	LPWSABUF	buffers	()
	{
		return &m_buffer;
	}
	DWORD		count	()
	{
		return 1;
	}
	::WSABUF		m_buffer;
	size_t			m_summary_size;
}; // struct wsabuf_const_buffers_sequence




template <typename Protocol>
xbox_iocp_socket_service<Protocol>::xbox_iocp_socket_service(
	boost::asio::io_service& io_service) :
		service_base<xbox_iocp_socket_service<Protocol> >(io_service),
	m_iocp_service(boost::asio::use_service<xbox_iocp_service>(io_service))
{
}

template <typename Protocol>
xbox_iocp_socket_service<Protocol>::~xbox_iocp_socket_service()
{
	shutdown_service();
}


template <typename Protocol>
void xbox_iocp_socket_service<Protocol>::shutdown_service()
{
	m_impl_list.for_each(
		std::bind1st(
			std::mem_fun(&xbox_iocp_socket_service::close_for_destruction),
			this));
	m_impl_list.clear	();			
}

template <typename Protocol>
void xbox_iocp_socket_service<Protocol>::construct(implementation_type& impl)
{
	impl.m_socket			= invalid_socket;
	impl.m_flags			= 0;
	m_impl_list.push_back	(&impl);
}

template <typename Protocol>
void xbox_iocp_socket_service<Protocol>::destroy(implementation_type& impl)
{
	close_for_destruction	(&impl);
	m_impl_list.erase		(&impl);
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::open(implementation_type& impl,
																   const protocol_type& protocol,
																   boost::system::error_code& ec)
{
	BOOST_ASSERT(protocol == m_protocol);
	if (is_open(impl))
	{
		ec = boost::asio::error::already_open;
		return ec;
	}

	socket_holder sock(socket_ops::socket(protocol.family(), protocol.type(),
          protocol.protocol(), ec));
    
	if (sock.get() == invalid_socket)
	{
		return ec;
	}

    impl.m_socket	= sock.release();
    impl.m_flags	= 0;
	ec.clear		();
	return ec;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::assign(implementation_type& impl,
																	 const protocol_type& protocol,
																	 const native_type& native_socket,
																	 boost::system::error_code& ec)
{
	BOOST_ASSERT(protocol == m_protocol);
	if (is_open(impl))
	{
		ec = boost::asio::error::already_open;
		return ec;
	}

	impl.m_socket	= native_socket;
	impl.m_flags	= 0;
	ec.clear		();
	return ec;
}

template <typename Protocol>
bool xbox_iocp_socket_service<Protocol>::is_open(const implementation_type& impl) const
{
	return impl.m_socket != invalid_socket;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::close(implementation_type& impl,
																	boost::system::error_code& ec)
{
	if (is_open(impl))
	{
		if (socket_ops::close(impl.m_socket, ec) == socket_error_retval)
		{
			return ec;
		}

		impl.m_socket	= invalid_socket;
		impl.m_flags	= 0;
	}
	ec.clear();
	return ec;
}

template <typename Protocol>
typename xbox_iocp_socket_service<Protocol>::native_type
xbox_iocp_socket_service<Protocol>::native(implementation_type& impl)
{
	return impl.m_socket;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::cancel(implementation_type& impl,
																	 boost::system::error_code& ec)
{
#pragma message (XRAY_TODO("implement cancel io functionality for timers"))
	int errorcode = WSACancelOverlappedIO(impl.m_socket);
	if (errorcode)
	{
		ec.assign(errorcode, boost::asio::error::get_system_category());
		return ec;
	}
	ec.clear();
	return ec;
}

template <typename Protocol>
	template <typename Option>
boost::system::error_code xbox_iocp_socket_service<Protocol>::set_option(implementation_type& impl,
																		 const Option& option,
																		 boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

    if ((option.level(m_protocol) == custom_socket_option_level) &&
		(option.name(m_protocol) == enable_connection_aborted_option))
    {
		if (option.size(m_protocol) != sizeof(int))
		{
			ec = boost::asio::error::invalid_argument;
		} else
		{
			if (*reinterpret_cast<const int*>(option.data(m_protocol)))
			{
				impl.m_flags |= implementation_type::enable_connection_aborted;
			} else
			{
				impl.m_flags &= ~implementation_type::enable_connection_aborted;
			}
			ec.clear();
		}
      return ec;
	}
    
	if ((option.level(m_protocol) == SOL_SOCKET)
		&& (option.name(m_protocol) == SO_LINGER))
	{
		const ::linger* linger_option = 
			reinterpret_cast<const ::linger*>(option.data(m_protocol));
		if ((linger_option->l_onoff != 0) && (linger_option->l_linger != 0))
		{
			impl.m_flags |= implementation_type::close_might_block;
		} else
		{
			impl.m_flags &= ~implementation_type::close_might_block;
		}
	}

	socket_ops::setsockopt(impl.m_socket,
		option.level(m_protocol), option.name(m_protocol),
		option.data(m_protocol), option.size(m_protocol), ec);

	return ec;    
}

template <typename Protocol>
	template <typename Option>
boost::system::error_code xbox_iocp_socket_service<Protocol>::get_option(const implementation_type& impl,
																		 Option& option,
																		 boost::system::error_code& ec) const
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

    if ((option.level(m_protocol) == custom_socket_option_level) &&
		(option.name(m_protocol) == enable_connection_aborted_option))
    {
		if (option.size(m_protocol) != sizeof(int))
		{
			ec = boost::asio::error::invalid_argument;
		} else
		{
			int* target = reinterpret_cast<int*>(option.data(m_protocol));
			if (impl.m_flags & implementation_type::enable_connection_aborted)
			{
				*target = 1;
			} else
			{
				*target = 0;
			}
			option.resize	(m_protocol, sizeof(int));
			ec.clear		();
		}
		return ec;
    }
    
	size_t size				= option.size(m_protocol);
	socket_ops::getsockopt	(impl.m_socket,
		option.level(m_protocol), option.name(m_protocol),
		option.data(m_protocol), &size, ec);
	
	if (!ec)
	{
		option.resize(m_protocol, size);
	}
	return ec;
}

template <typename Protocol>
	template <typename IO_Control_Command>
boost::system::error_code xbox_iocp_socket_service<Protocol>::io_control(implementation_type& impl,
																		 IO_Control_Command& command,
																		 boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

	socket_ops::ioctl(impl.m_socket, command.name(),
		static_cast<ioctl_arg_type*>(command.data()), ec);

	if (!ec && command.name() == static_cast<int>(FIONBIO))
	{
		if (*static_cast<ioctl_arg_type*>(command.data()))
		{
			impl.flags_ |= implementation_type::user_set_non_blocking;
		} else
		{
			impl.flags_ &= ~implementation_type::user_set_non_blocking;
		}
	}
	return ec;
}

template <typename Protocol>
bool xbox_iocp_socket_service<Protocol>::at_mark(const implementation_type& impl,
												 boost::system::error_code& ec) const
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return false;
	}

    ioctl_arg_type		value = 0;
    socket_ops::ioctl	(impl.m_socket, SIOCATMARK, &value, ec);
    return ec ? false : value != 0;
}

template <typename Protocol>
std::size_t xbox_iocp_socket_service<Protocol>::available(const implementation_type& impl,
														  boost::system::error_code& ec) const
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return 0;
	}

	ioctl_arg_type		value = 0;
	socket_ops::ioctl	(impl.m_socket, FIONREAD, &value, ec);
	return ec ? 0 : static_cast<std::size_t>(value);
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::bind(implementation_type& impl,
																   const endpoint_type& endpoint,
																   boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

    socket_ops::bind(impl.m_socket, endpoint.data(), endpoint.size(), ec);
    return ec;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::listen(implementation_type& impl,
																	 int backlog,
																	 boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

	socket_ops::listen(impl.m_socket, backlog, ec);
	return ec;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::shutdown(implementation_type& impl,
																	   socket_base::shutdown_type what,
																	   boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return ec;
	}

	socket_ops::shutdown(impl.m_socket, what, ec);
	return ec;
}

template <typename Protocol>
typename xbox_iocp_socket_service<Protocol>::endpoint_type
xbox_iocp_socket_service<Protocol>::local_endpoint(const implementation_type& impl,
												   boost::system::error_code& ec) const
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return endpoint_type();
	}

	endpoint_type			endpoint;
	std::size_t addr_len	= endpoint.capacity();
	if (socket_ops::getsockname(impl.m_socket, endpoint.data(), &addr_len, ec))
	{
		return endpoint_type();
	}
	endpoint.resize			(addr_len);
	return endpoint;
}

template <typename Protocol>
typename xbox_iocp_socket_service<Protocol>::endpoint_type
xbox_iocp_socket_service<Protocol>::remote_endpoint(const implementation_type& impl,
													boost::system::error_code& ec) const
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return endpoint_type();
	}

    if (impl.m_socket.have_remote_endpoint())
    {
		ec.clear();
		return impl.m_socket.remote_endpoint();
    }
   
	endpoint_type	endpoint;
	std::size_t		addr_len = endpoint.capacity();
	if (socket_ops::getpeername(impl.m_socket, endpoint.data(), &addr_len, ec))
	{
		return endpoint_type();
	}

	endpoint.resize	(addr_len);
	return endpoint;
}

template <typename Protocol>
boost::system::error_code xbox_iocp_socket_service<Protocol>::connect(implementation_type& impl,
																	  const endpoint_type& peer_endpoint,
																	  boost::system::error_code& ec)
{
	if (!is_open(impl))
    {
		ec = boost::asio::error::bad_descriptor;
		return ec;
    }

	if (!socket_ops::connect(impl.m_socket, 
			static_cast<socket_addr_type const*>(peer_endpoint.data()),
			peer_endpoint.size(),
			ec))
	{
		ec.clear();
	}
	return ec;	
}


template <typename Protocol>
boost::system::error_code const & xbox_iocp_socket_service<Protocol>::enable_nonblock_io(
	implementation_type & impl, boost::system::error_code& ec)
{
	unsigned long nbio_flag = 1;
	socket_ops::ioctl(impl.m_socket,
		FIONBIO, static_cast<ioctl_arg_type*>(&nbio_flag), ec);
	return ec;
}

template <typename Protocol>
boost::system::error_code const & xbox_iocp_socket_service<Protocol>::disable_nonblock_io(
	implementation_type & impl, boost::system::error_code& ec)
{
	unsigned long nbio_flag = 0;
	socket_ops::ioctl(impl.m_socket,
		FIONBIO, static_cast<ioctl_arg_type*>(&nbio_flag), ec);
	return ec;
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::async_connect(implementation_type& impl,
													   const endpoint_type& peer_endpoint,
													   Handler handler)
{
	typedef connect_operation<Handler>					value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	boost::system::error_code	tmp_ec;
	if (!(impl.m_flags & implementation_type::user_set_non_blocking))
	{
		if (enable_nonblock_io(impl, tmp_ec))
		{
			m_iocp_service.post(bind_handler(handler, tmp_ec));
			return;
		}
	}

	connect(impl, peer_endpoint, tmp_ec);
		
	if ((tmp_ec.value() == 0) || (tmp_ec.value() != WSAEWOULDBLOCK))
	{
		boost::system::error_code tmp_dec;
		if (!(impl.m_flags & implementation_type::user_set_non_blocking))
		{
			disable_nonblock_io(impl, tmp_dec);
		}
		m_iocp_service.post(bind_handler(handler, tmp_ec));
		return;
	}

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr, m_iocp_service, handler);

	operation*			operation_ptr		= handler_ptr.get();
	operation_event	&	ovevent				= m_iocp_service.start_operation(
		impl.m_socket, operation_ptr);

	int errorcode = ::WSAEventSelect		(impl.m_socket,
		operation_ptr->overlapped_ptr()->hEvent,
		FD_CONNECT);
	
	if (errorcode)
	{
		DWORD last_error = ::WSAGetLastError();
		handler_ptr.get()->set_result(last_error);
		ovevent.set(true);
	}
	handler_ptr.release();
}

template <typename Protocol>		
	template <typename MutableBufferSequence>
size_t xbox_iocp_socket_service<Protocol>::receive(implementation_type& impl,
												   const MutableBufferSequence& buffers,
												   socket_base::message_flags flags,
												   boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return 0;
	}

	wsabuf_buffers_sequence<MutableBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	// Receive some data.
	DWORD	bytes_transferred	= 0;
	DWORD	recv_flags			= flags;

	int		result				= ::WSARecv(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred, &recv_flags, 0, 0);

	if (result != 0)
	{
		DWORD last_error = ::WSAGetLastError();
		ec = boost::system::error_code(last_error,
		  boost::asio::error::get_system_category());
		return 0;
	}

	if ((bytes_transferred == 0) && (m_protocol.type() == SOCK_STREAM))
	{
		ec = boost::asio::error::eof;
		return 0;
	}

	ec.clear();
	return bytes_transferred;
}

template <typename Protocol>
	template <typename MutableBufferSequence>
size_t	xbox_iocp_socket_service<Protocol>::receive_from(implementation_type& impl,
														 const MutableBufferSequence& buffers,
														 endpoint_type& sender_endpoint,
														 socket_base::message_flags flags,
														 boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return 0;
	}

	wsabuf_buffers_sequence<MutableBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	// Receive some data.
	DWORD	bytes_transferred	= 0;
	DWORD	recv_flags			= flags;

	int		endpoint_size = static_cast<int>(sender_endpoint.capacity());
    int		result = ::WSARecvFrom(impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred,
        &recv_flags,
		sender_endpoint.data(), &endpoint_size,
		0, 0);

	if (result != 0)
	{
		DWORD last_error = ::WSAGetLastError();
		ec = boost::system::error_code(last_error,
			boost::asio::error::get_system_category());
		return 0;
	}

	if ((bytes_transferred == 0) && (m_protocol.type() == SOCK_STREAM))
	{
		ec = boost::asio::error::eof;
		return 0;
	}
	
	sender_endpoint.resize(static_cast<std::size_t>(endpoint_size));
	ec.clear();
	return bytes_transferred;
}

template <typename Protocol>
	template <typename MutableBufferSequence, typename Handler>
void xbox_iocp_socket_service<Protocol>::async_receive(implementation_type& impl,
													   const MutableBufferSequence& buffers,
													   socket_base::message_flags flags,
													   Handler handler)
{

	// Allocate and construct an operation to wrap the handler.
	typedef io_operation<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	if (!is_open(impl))
	{
		m_iocp_service.post(
			bind_handler(handler, boost::asio::error::bad_descriptor, 0));
		return;
	}

	wsabuf_buffers_sequence<MutableBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	BOOST_ASSERT(wsa_buffs.m_summary_size > 0);

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr,
		m_iocp_service,
		handler);

	DWORD				bytes_transferred	= 0;
    DWORD				recv_flags			= flags;
	
	operation*			operation_ptr		= handler_ptr.get();
	m_iocp_service.start_operation			(impl.m_socket, operation_ptr);
	
	int					result = ::WSARecv(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred, &recv_flags,
		operation_ptr->overlapped_ptr(), 0);

	if (!result)
	{
		handler_ptr.get()->set_result(0, bytes_transferred);
	} else
	{
		DWORD last_error = ::WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			handler_ptr.get()->set_result(last_error, 0);
			//ovevent.set(true);
		}
	}
	handler_ptr.release();
}

template <typename Protocol>
	template <typename MutableBufferSequence, typename Handler>
void xbox_iocp_socket_service<Protocol>::async_receive_from(implementation_type& impl,
															const MutableBufferSequence& buffers,
															endpoint_type& sender_endpoint,
															socket_base::message_flags flags,
															Handler handler)
{
	// Allocate and construct an operation to wrap the handler.
	typedef io_operation<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	if (!is_open(impl))
	{
		m_iocp_service.post(
			bind_handler(handler, boost::asio::error::bad_descriptor, 0));
		return;
	}

	wsabuf_buffers_sequence<MutableBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	BOOST_ASSERT(wsa_buffs.m_summary_size > 0);

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr,
		m_iocp_service,
		handler);

	DWORD				bytes_transferred	= 0;
    DWORD				recv_flags			= flags;
	int					endpoint_size = static_cast<int>(sender_endpoint.capacity());
	
	operation*			operation_ptr		= handler_ptr.get();
	m_iocp_service.start_operation			(impl.m_socket, operation_ptr);
	
    int					result = ::WSARecvFrom(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred, &recv_flags,
		sender_endpoint.data(), &endpoint_size,
		operation_ptr->overlapped_ptr(), 0);

	if (!result)
	{
		handler_ptr.get()->set_result(0, bytes_transferred);
	} else
	{
		DWORD last_error = ::WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			handler_ptr.get()->set_result(last_error, 0);
			//ovevent.set(true);
		}
	}
	handler_ptr.release();
}

template <typename Protocol>
	template <typename ConstBufferSequence>
size_t xbox_iocp_socket_service<Protocol>::send(implementation_type& impl,
												const ConstBufferSequence& buffers,
												socket_base::message_flags flags,
												boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return 0;
	}

	wsabuf_buffers_sequence<ConstBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

    // A request to receive 0 bytes on a stream socket is a no-op.
    if ((m_protocol.type() == SOCK_STREAM) && (wsa_buffs.m_summary_size == 0))
    {
		ec.clear();
		return 0;
    }

    // Send the data.
    DWORD	bytes_transferred	= 0;
    int		result = ::WSASend(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
        &bytes_transferred, 0, 0, 0);

    if (result != 0)
    {
		DWORD last_error = ::WSAGetLastError();
		ec = boost::system::error_code(last_error,
			boost::asio::error::get_system_category());
		return 0;
    }

    ec.clear();
    return bytes_transferred;
}

template <typename Protocol>
	template <typename ConstBufferSequence>
size_t xbox_iocp_socket_service<Protocol>::send_to(implementation_type& impl,
												   const ConstBufferSequence& buffers,
												   const endpoint_type& destination,
												   socket_base::message_flags flags,
												   boost::system::error_code& ec)
{
	if (!is_open(impl))
	{
		ec = boost::asio::error::bad_descriptor;
		return 0;
	}

	wsabuf_buffers_sequence<ConstBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

    // A request to receive 0 bytes on a stream socket is a no-op.
    if ((m_protocol.type() == SOCK_STREAM) && (wsa_buffs.m_summary_size == 0))
    {
		ec.clear();
		return 0;
    }

    // Send the data.
    DWORD	bytes_transferred	= 0;
	int		result = ::WSASendTo(impl.m_socket,
		wsa_buffs.buffesr(), wsa_buffs.count(), &bytes_transferred, 0,
		destination.data(), static_cast<int>(destination.size()), 0, 0);

    if (result != 0)
    {
		DWORD last_error = ::WSAGetLastError();
		ec = boost::system::error_code(last_error,
			boost::asio::error::get_system_category());
		return 0;
    }

    ec.clear();
    return bytes_transferred;
}

template <typename Protocol>
	template <typename ConstBufferSequence, typename Handler>
void xbox_iocp_socket_service<Protocol>::async_send(implementation_type& impl,
													const ConstBufferSequence& buffers,
													socket_base::message_flags flags,
													Handler handler)
{
	typedef io_operation<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	if (!is_open(impl))
	{
		m_iocp_service.post(
			bind_handler(handler, boost::asio::error::bad_descriptor, 0));
		return;
	}
	
	wsabuf_buffers_sequence<MutableBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	// A request to receive 0 bytes on a stream socket is a no-op.
    if ((m_protocol.type() == SOCK_STREAM) && (wsa_buffs.m_summary_size == 0))
    {
		m_iocp_service.post(
			bind_handler(handler, boost::system::error_code(), 0));
		return;
    }

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr, m_iocp_service, handler);
	DWORD				bytes_transferred	= 0;

	operation*			operation_ptr		= handler_ptr.get();
	m_iocp_service.start_operation			(impl.m_socket, operation_ptr);
		
	int					result = ::WSASend(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred, 0, operation_ptr->overlapped_ptr(), 0);

	if (!result)
	{
		handler_ptr.get()->set_result(0, bytes_transferred);
	} else
	{
		DWORD last_error = ::WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			handler_ptr.get()->set_result(last_error, 0);
			//ovevent.set(true);
		}
	}
	handler_ptr.release();
}

template <typename Protocol>
	template <typename ConstBufferSequence, typename Handler>
void xbox_iocp_socket_service<Protocol>::async_send_to(implementation_type& impl,
													   const ConstBufferSequence& buffers,
													   const endpoint_type& destination,
													   socket_base::message_flags flags,
													   Handler handler)
{
	typedef io_operation<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;
	typedef event_type									event_type;

	if (!is_open(impl))
	{
		m_iocp_service.post(
			bind_handler(handler, boost::asio::error::bad_descriptor, 0));
		return;
	}
	
	wsabuf_buffers_sequence<ConstBufferSequence, max_buffers>	wsa_buffs(
		buffers.begin(), buffers.end());

	// A request to receive 0 bytes on a stream socket is a no-op.
    if ((m_protocol.type() == SOCK_STREAM) && (wsa_buffs.m_summary_size == 0))
    {
		m_iocp_service.post(
			bind_handler(handler, boost::system::error_code(), 0));
		return;
    }

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr, m_iocp_service, handler);
	DWORD				bytes_transferred	= 0;
	operation*			operation_ptr		= handler_ptr.get();
	m_iocp_service.start_operation			(impl.m_socket, operation_ptr);
	
	int					result = ::WSASendTo(
		impl.m_socket,
		wsa_buffs.buffers(), wsa_buffs.count(),
		&bytes_transferred, 0,
		destination.data(), static_cast<int>(destination.size()),
		operation_ptr->overlapped_ptr(), 0);

	if (!result)
	{
		handler_ptr.get()->set_result(0, bytes_transferred);
	} else
	{
		DWORD last_error = ::WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			handler_ptr.get()->set_result(last_error, 0);
			//ovevent.set(true);
		}
	}
	handler_ptr.release();
}

template <typename Protocol>
	template <typename Socket>
boost::system::error_code xbox_iocp_socket_service<Protocol>::accept(implementation_type& impl,
																	 Socket& peer,
																	 endpoint_type* peer_endpoint,
																	 boost::system::error_code& ec)
{
	if (!is_open(impl))
    {
		ec = boost::asio::error::bad_descriptor;
		return ec;
    }
    if (peer.is_open())
    {
		ec = boost::asio::error::already_open;
		return ec;
    }

	std::size_t	endpoint_size = 0;
	if (peer_endpoint)
	{
		endpoint_size = peer_endpoint->capacity();
	}
	
	peer.assign(Protocol(),
		socket_ops::accept(impl.m_socket,
			peer_endpoint->data(),
			&endpoint_size,
			ec));
	
	BOOST_ASSERT(!ec || (peer.native() == invalid_socket));
	return ec;
}

template <typename Protocol>
	template <typename Socket, typename Handler>
void xbox_iocp_socket_service<Protocol>::async_accept(implementation_type& impl,
													  Socket& peer,
													  endpoint_type* peer_endpoint,
													  Handler handler)
{
	typedef accept_operation<Handler, Socket>			value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	boost::system::error_code	tmp_ec;
	if (!(impl.m_flags & implementation_type::user_set_non_blocking))
	{
		if (enable_nonblock_io(impl, tmp_ec))
		{
			m_iocp_service.post(bind_handler(handler, tmp_ec));
			return;
		}
	}

	accept(impl, peer, peer_endpoint, tmp_ec);
		
	if ((tmp_ec.value() == 0) || (tmp_ec.value() != WSAEWOULDBLOCK))
	{
		boost::system::error_code tmp_dec;
		if (!(impl.m_flags & implementation_type::user_set_non_blocking))
		{
			disable_nonblock_io(impl, tmp_dec);
		}
		m_iocp_service.post(bind_handler(handler, tmp_ec));
		return;
	}

	pointer_allocator	raw_ptr(handler);
	handler_contructor	handler_ptr(raw_ptr, m_iocp_service, peer, handler);

	operation*			operation_ptr		= handler_ptr.get();
	operation_event	&	ovevent				= m_iocp_service.start_operation(
		impl.m_socket, operation_ptr);
	
	int errorcode = ::WSAEventSelect		(impl.m_socket,
		operation_ptr->overlapped_ptr()->hEvent,
		FD_ACCEPT);
	
	if (errorcode)
	{
		DWORD last_error = ::WSAGetLastError();
		handler_ptr.get()->set_result(last_error);
		ovevent.set(true);
	}

	handler_ptr.release();
}

template <typename Protocol>
void xbox_iocp_socket_service<Protocol>::close_for_destruction(implementation_type* impl)
{
	BOOST_ASSERT(impl);
	boost::system::error_code ec;
	cancel	(*impl, ec);
	BOOST_ASSERT(!ec);
	close	(*impl, ec);
}

///////////////////////////////////////////////////////////////////////////////

template <typename Protocol>
	template <typename Handler>
xbox_iocp_socket_service<Protocol>::io_operation<Handler>::io_operation(
		xbox_iocp_service & iocp_service,
		Handler const & handler) :
	operation(iocp_service),
	m_last_error(0),
	m_bytes_transferred(0),
	m_io_service(iocp_service),
	m_handler(handler),
	m_results_ready(false)
{
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::io_operation<Handler>::do_completion()
{
	typedef handler_alloc_traits<Handler, io_operation>		alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>						handler_ptr_t;
	
	BOOST_ASSERT(m_results_ready);
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);

	boost::system::error_code	ec(m_last_error,
		boost::asio::error::get_system_category());

	size_t				bytes_transferred = m_bytes_transferred;
	
	ptr_raii.reset();

	boost_asio_handler_invoke_helpers::invoke(
		detail::bind_handler(tmp_handler, ec, bytes_transferred), &tmp_handler);
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::io_operation<Handler>::destroy()
{
	typedef handler_alloc_traits<Handler, io_operation>		alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>						handler_ptr_t;
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);
	tmp_handler;

	ptr_raii.reset();
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::io_operation<Handler>::fetch_result(
	HANDLE file_handle, boost::system::error_code & ec)
{
	if (m_results_ready)
	{
		ec	= boost::system::error_code(m_last_error, error::system_category);
		return;
	}
	DWORD	bytes_transferred = 0;

	if (::GetOverlappedResult(file_handle,
		overlapped_ptr(),
		&bytes_transferred, TRUE))
	{
		m_last_error		= 0;
		m_bytes_transferred = static_cast<size_t>(bytes_transferred);
		ec.clear			();
	} else
	{
		m_last_error		= ::GetLastError();
		m_bytes_transferred	= 0;
		ec	= boost::system::error_code(m_last_error, error::system_category);
	}
	m_results_ready			= true;
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::io_operation<Handler>::set_result(
	DWORD last_error, size_t bytes_transferred)
{
	m_last_error			= last_error;
	m_bytes_transferred		= bytes_transferred; 
	m_results_ready			= true;
}

template <typename Protocol>
	template <typename Handler>
xbox_iocp_socket_service<Protocol>::connect_operation<Handler>::connect_operation(
		xbox_iocp_service & iocp_service, Handler const & handler) :
	operation		(iocp_service),
	m_last_error	(0),
	m_io_service	(iocp_service),
	m_handler		(handler),
	m_results_ready	(false)
{
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::connect_operation<Handler>::set_result(DWORD last_error)
{
	m_last_error	= last_error;
	m_results_ready	= true;
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::connect_operation<Handler>::do_completion()
{
	typedef handler_alloc_traits<Handler, connect_operation>	alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>							handler_ptr_t;
	
	BOOST_ASSERT(m_results_ready);
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);

	boost::system::error_code	ec(m_last_error,
		boost::asio::error::system_category);
	
	ptr_raii.reset();

	boost_asio_handler_invoke_helpers::invoke(
		detail::bind_handler(tmp_handler, ec), &tmp_handler);
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::connect_operation<Handler>::destroy()
{
	typedef handler_alloc_traits<Handler, connect_operation>	alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>							handler_ptr_t;
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);
	tmp_handler;

	ptr_raii.reset();
}

template <typename Protocol>
	template <typename Handler>
void xbox_iocp_socket_service<Protocol>::connect_operation<Handler>::fetch_result(HANDLE file_handle, boost::system::error_code & ec)
{
	fd_set	write_set;
	fd_set	except_set;
	FD_ZERO	(&write_set);
	FD_ZERO	(&except_set);
	FD_SET	((SOCKET)file_handle, &write_set);
	FD_SET	((SOCKET)file_handle, &except_set);

	::timeval	tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 0;
	
	int errorcode = ::select(1, NULL, &write_set, &except_set, &tv);
	
	BOOST_ASSERT(errorcode != 0);

	if (errorcode == socket_error_retval)
	{
		m_last_error		= ::WSAGetLastError();
		ec	= boost::system::error_code(m_last_error, error::system_category);
	} else if (!FD_ISSET((SOCKET)file_handle, &write_set) ||
				FD_ISSET((SOCKET)file_handle, &except_set))
	{
		m_last_error = error::timed_out;
	} else
	{
		m_last_error = 0;
	}
	m_results_ready = true;
}

template <typename Protocol>
	template <typename Handler, typename SocketType>
xbox_iocp_socket_service<Protocol>::accept_operation<Handler, SocketType>::accept_operation(
		xbox_iocp_service & iocp_service, SocketType & peer, Handler const & handler) :
	operation		(iocp_service),
	m_last_error	(0),
	m_io_service	(iocp_service),
	m_peer			(peer),
	m_handler		(handler),
	m_results_ready	(false)
{
}

template <typename Protocol>
	template <typename Handler, typename SocketType>
void xbox_iocp_socket_service<Protocol>::accept_operation<Handler, SocketType>::set_result(DWORD last_error)
{
	m_last_error	= last_error;
	m_results_ready	= true;
}

template <typename Protocol>
	template <typename Handler, typename SocketType>
void xbox_iocp_socket_service<Protocol>::accept_operation<Handler, SocketType>::do_completion()
{
	typedef handler_alloc_traits<Handler, accept_operation>		alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>							handler_ptr_t;
	
	BOOST_ASSERT(m_results_ready);
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);

	boost::system::error_code	ec(m_last_error, error::system_category);
	
	ptr_raii.reset();

	boost_asio_handler_invoke_helpers::invoke(
		detail::bind_handler(tmp_handler, ec), &tmp_handler);
}

template <typename Protocol>
	template <typename Handler, typename SocketType>
void xbox_iocp_socket_service<Protocol>::accept_operation<Handler, SocketType>::destroy()
{
	typedef handler_alloc_traits<Handler, accept_operation>		alloc_traits_t;
	typedef handler_ptr<alloc_traits_t>							handler_ptr_t;
	
	handler_ptr_t		ptr_raii	(m_handler, this);
	Handler				tmp_handler	(m_handler);
	tmp_handler;

	//int errorcode = ::WSAEventSelect((SOCKET)file_handle, this->hEvent, 0);
	
	ptr_raii.reset();
}

template <typename Protocol>
	template <typename Handler, typename SocketType>
void xbox_iocp_socket_service<Protocol>::accept_operation<Handler, SocketType>::fetch_result(HANDLE file_handle, boost::system::error_code & ec)
{
	fd_set	read_set;
	fd_set	except_set;
	FD_ZERO	(&read_set);
	FD_ZERO	(&except_set);
	FD_SET	((SOCKET)file_handle, &read_set);
	FD_SET	((SOCKET)file_handle, &except_set);

	::timeval	tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 0;
	
	int errorcode = ::select(1, &read_set, NULL, &except_set, &tv);
	
	BOOST_ASSERT(errorcode != 0);

	if (errorcode == socket_error_retval)
	{
		m_last_error		= ::WSAGetLastError();
		ec	= boost::system::error_code(m_last_error,
			boost::asio::error::system_category);
	} else if (!FD_ISSET((SOCKET)file_handle, &read_set) ||
				FD_ISSET((SOCKET)file_handle, &except_set))
	{
		m_last_error		= error::fd_set_failure;
	} else
	{
		m_last_error		= 0;
	}
	errorcode = ::WSAEventSelect((SOCKET)file_handle, overlapped_ptr()->hEvent, 0);
	BOOST_ASSERT(errorcode == 0);

	m_results_ready = true;
}


} // namespace detail
} // namespace asio
} // namespace boost


#endif // #ifndef XBOX_IOCP_SOCKET_SERVICE_INLINE_HPP_INCLUDED