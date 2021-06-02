////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_IOCP_SOCKET_SERVICE_HPP_INCLUDED
#define XBOX_IOCP_SOCKET_SERVICE_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/detail/socket_types.hpp> // Must come before posix_time.

#include <boost/asio/detail/push_options.hpp>
#include <cstddef>
#include <boost/config.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <xray/intrusive_double_linked_list.h>
#include <boost/asio/detail/pop_options.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include <boost/asio/detail/noncopyable.hpp>
#include <boost/asio/detail/service_base.hpp>
#include <boost/asio/detail/signal_blocker.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/socket_holder.hpp>
#include <boost/asio/detail/task_io_service.hpp>
#include <boost/asio/detail/timer_queue.hpp>
#include <boost/asio/xbox360/xbox_iocp_service.hpp>

namespace boost {
namespace asio {
namespace detail {

template <typename Protocol>
class xbox_iocp_socket_service :
	public service_base<xbox_iocp_socket_service<Protocol> >
{
public:
	// The protocol type.
	typedef Protocol									protocol_type;
	// The endpoint type.
	typedef typename Protocol::endpoint					endpoint_type;
	// The operation type.
	typedef boost::asio::detail::operation				operation;
	// The operation completion event type
	typedef xbox_iocp_service::overlapped_event_type	operation_event;

	// The native type of a socket.
	class native_type
	{
	public:
						native_type				(socket_type s);
						native_type				(socket_type s, const endpoint_type& ep);
		void			operator=				(socket_type s);
						operator socket_type	() const;
		HANDLE			as_handle				() const;
		bool			have_remote_endpoint	() const;
		endpoint_type	remote_endpoint			() const;
		operator HANDLE	() const				{ return as_handle(); }
	private:
		friend class xbox_iocp_socket_service;

		socket_type							m_socket;
		boost::optional<endpoint_type>		m_remote_endpoint;
	}; // class native_type

	// The implementation type of the socket.
	class implementation_type
	{
	public:
		// Default constructor.
		implementation_type();
	private:
		// Only this service will have access to the internal values.
		friend class xbox_iocp_socket_service;
		enum
		{
			enable_connection_aborted	= 1 << 0, // User wants connection_aborted errors.
			close_might_block			= 1 << 1, // User set linger option for blocking close.
			user_set_non_blocking		= 1 << 2, // The user wants a non-blocking socket.
		};

		// The native socket representation.
		native_type							m_socket;
		// Flags indicating the current state of the socket.
		unsigned int 						m_flags;
		// Next pointer in single linked list
		implementation_type*				m_next_ptr;
		// Previous pointer in single linked list
		implementation_type*				m_prev_ptr;
	}; // class implementation_type

	enum { max_buffers = 1 };

	// Constructor
	xbox_iocp_socket_service	(boost::asio::io_service& io_service);
	// Destructor
	~xbox_iocp_socket_service	();
	// Destroy all user-defined handler objects owned by the service.
	void						shutdown_service	();
	// Creates a new socket implementation.
	void						construct			(implementation_type& impl);
	// Cancels any async operation and close the socket.
	void						destroy				(implementation_type& impl);
	// Open a new socket implementation.
	boost::system::error_code	open				(implementation_type& impl,
													 const protocol_type& protocol,
													 boost::system::error_code& ec);
	// Assign a native socket to a socket implementation.
	boost::system::error_code	assign				(implementation_type& impl,
													 const protocol_type& protocol,
													 const native_type& native_socket,
													 boost::system::error_code& ec);
	// Determine whether the socket is open.
	bool						is_open				(const implementation_type& impl) const;

	// Destroy a socket implementation.
	boost::system::error_code	close				(implementation_type& impl,
													 boost::system::error_code& ec);
	// Get the native socket representation.
	native_type					native				(implementation_type& impl);

	// Cancel all operations associated with the socket.
	boost::system::error_code	cancel				(implementation_type& impl,
													 boost::system::error_code& ec);

	// Set a socket option.
	template <typename Option>
	boost::system::error_code	set_option			(implementation_type& impl,
													 const Option& option,
													 boost::system::error_code& ec);
	// Set a socket option.
	template <typename Option>
	boost::system::error_code	get_option			(const implementation_type& impl,
													 Option& option,
													 boost::system::error_code& ec) const;
	// Perform an IO control command on the socket.
	template <typename IO_Control_Command>
	boost::system::error_code	io_control			(implementation_type& impl,
													 IO_Control_Command& command,
													 boost::system::error_code& ec);
	// Determine whether the socket is at the out-of-band data mark.
	bool						at_mark				(const implementation_type& impl,
													 boost::system::error_code& ec) const;
	// Determine the number of bytes available for reading.
	std::size_t					available			(const implementation_type& impl,
													 boost::system::error_code& ec) const;
	// Bind the socket to the specified local endpoint.
	boost::system::error_code	bind				(implementation_type& impl,
													 const endpoint_type& endpoint,
													 boost::system::error_code& ec);
	/// Disable sends or receives on the socket.
	boost::system::error_code	shutdown			(implementation_type& impl,
													 socket_base::shutdown_type what,
													 boost::system::error_code& ec);

	// Get the local endpoint.
	endpoint_type				local_endpoint		(const implementation_type& impl,
													 boost::system::error_code& ec) const;
	// Get the remote endpoint.
	endpoint_type				remote_endpoint		(const implementation_type& impl,
													 boost::system::error_code& ec) const;

	// Connect the socket to the specified endpoint.
	boost::system::error_code	connect				(implementation_type& impl,
													 const endpoint_type& peer_endpoint,
													 boost::system::error_code& ec);
	// Start an asynchronous connect.
	template <typename Handler>
	void						async_connect		(implementation_type& impl,
													 const endpoint_type& peer_endpoint,
													 Handler handler);


	// Receive some data from the peer. Returns the number of bytes received.
	template <typename MutableBufferSequence>
	size_t	receive				(implementation_type& impl,
								 const MutableBufferSequence& buffers,
								 socket_base::message_flags flags,
								 boost::system::error_code& ec);
	// Start an asynchronous receive. The buffer for the data being received
	// must be valid for the lifetime of the asynchronous operation.
	template <typename MutableBufferSequence, typename Handler>
	void	async_receive		(implementation_type& impl,
								 const MutableBufferSequence& buffers,
								 socket_base::message_flags flags,
								 Handler handler);
	// Receive a datagram with the endpoint of the sender. Returns the number of
	// bytes received.
	template <typename MutableBufferSequence>
	size_t	receive_from		(implementation_type& impl,
								 const MutableBufferSequence& buffers,
								 endpoint_type& sender_endpoint,
								 socket_base::message_flags flags,
								 boost::system::error_code& ec);
	// Start an asynchronous receive. The buffer for the data being received and
	// the sender_endpoint object must both be valid for the lifetime of the
	// asynchronous operation.
	template <typename MutableBufferSequence, typename Handler>
	void	async_receive_from	(implementation_type& impl,
								 const MutableBufferSequence& buffers,
								 endpoint_type& sender_endp,
								 socket_base::message_flags flags,
								 Handler handler);
	// Send the given data to the peer. Returns the number of bytes sent.
	template <typename ConstBufferSequence>
	size_t	send				(implementation_type& impl,
								 const ConstBufferSequence& buffers,
								 socket_base::message_flags flags,
								 boost::system::error_code& ec);
	// Start an asynchronous send. The data being sent must be valid for the
	// lifetime of the asynchronous operation.
	template <typename ConstBufferSequence, typename Handler>
	void	async_send			(implementation_type& impl,
								 const ConstBufferSequence& buffers,
								 socket_base::message_flags flags,
								 Handler handler);	
	// Send a datagram to the specified endpoint. Returns the number of bytes
	// sent.
	template <typename ConstBufferSequence>
	size_t	send_to				(implementation_type& impl,
								 const ConstBufferSequence& buffers,
								 const endpoint_type& destination,
								 socket_base::message_flags flags,
								 boost::system::error_code& ec);
	// Start an asynchronous send. The data being sent must be valid for the
	// lifetime of the asynchronous operation.
	template <typename ConstBufferSequence, typename Handler>
	void	async_send_to		(implementation_type& impl,
								 const ConstBufferSequence& buffers,
								 const endpoint_type& destination,
								 socket_base::message_flags flags,
								 Handler handler);

	// Place the socket into the state where it will listen for new connections.
	boost::system::error_code	listen			(implementation_type& impl,
												 int backlog,
												 boost::system::error_code& ec);
	// Accept a new connection.
	template <typename Socket>
	boost::system::error_code	accept			(implementation_type& impl,
												 Socket& peer,
												 endpoint_type* peer_endpoint,
												 boost::system::error_code& ec);
	// Start an asynchronous accept. The peer and peer_endpoint objects
	// must be valid until the accept's handler is invoked.
	template <typename Socket, typename Handler>
	void						async_accept	(implementation_type& impl,
												 Socket& peer,
												 endpoint_type* peer_endpoint,
												 Handler handler);
private:
	void	close_for_destruction				(implementation_type* impl);
	static boost::system::error_code const &	enable_nonblock_io(implementation_type & impl,
																   boost::system::error_code& ec);
	static boost::system::error_code const &	disable_nonblock_io(implementation_type & impl,
																	boost::system::error_code& ec);

	
	// Classes to store overlapped handles for async operations

	template <typename Handler>
	class io_operation  : public operation
	{
	public:	
						io_operation	(xbox_iocp_service & iocp_service,
										 Handler const & handler);
				void	set_result		(DWORD last_error, size_t bytes_transferred);
	private:
		virtual void	do_completion	();
		virtual void	destroy			();
		virtual	void	fetch_result	(HANDLE file_handle, boost::system::error_code & ec);

		DWORD							m_last_error;
		size_t							m_bytes_transferred;
		xbox_iocp_service &				m_io_service;
		Handler							m_handler;
		bool							m_results_ready;
	}; // class io_operation

	template <typename Handler>
	class connect_operation : public operation
	{
	public:
				connect_operation	(xbox_iocp_service & iocp_service,
									 Handler const & handler);
		void	set_result			(DWORD last_error);
	private:
		virtual void	do_completion	();
		virtual void	destroy			();
		virtual	void	fetch_result	(HANDLE file_handle, boost::system::error_code & ec);

		DWORD							m_last_error;
		xbox_iocp_service &				m_io_service;
		Handler							m_handler;
		bool							m_results_ready;
	}; // class connect_operation

	template <typename Handler, typename SocketType>
	class accept_operation : public operation
	{
	public:
				accept_operation	(xbox_iocp_service & iocp_service,
									 SocketType & peer,
									 Handler const & handler);
		void	set_result			(DWORD last_error);
	private:
		virtual void	do_completion	();
		virtual void	destroy			();
		virtual	void	fetch_result	(HANDLE file_handle, boost::system::error_code & ec);

		DWORD							m_last_error;
		xbox_iocp_service &				m_io_service;
		SocketType &					m_peer;
		Handler							m_handler;
		bool							m_results_ready;
	}; // class accept_operation
	
	typedef xray::intrusive_double_linked_list<
		implementation_type,
		implementation_type *,
		&implementation_type::m_prev_ptr,
		&implementation_type::m_next_ptr,
		xray::threading::mutex>	implementation_list_t;

	Protocol							m_protocol;
	implementation_list_t				m_impl_list;
	xbox_iocp_service &					m_iocp_service;
	
}; // class xbox_iocp_socket_service

} // namespace detail
} // namespace asio
} // namespace boost

#include "xbox_iocp_socket_service_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>


#endif // #ifndef XBOX_IOCP_SOCKET_SERVICE_HPP_INCLUDED