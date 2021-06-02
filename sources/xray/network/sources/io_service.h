////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef IO_SERVICE_H_INCLUDED
#define IO_SERVICE_H_INCLUDED

#include "asio_handler_hook.h"
#include "asio_types_multi.h"
#include "async_handler_host.h"

namespace xray {
namespace network {
namespace lowlevel {

typedef async_handler_host<std_allocator, xray::threading::mutex> async_host;

class io_service :
	private boost::noncopyable
{
public:
				io_service		();
				~io_service		();

	void		tick			();
private:
	void		test_timer_func			(error_code const ec);
	void		sent_result				(error_code const ec, std::size_t bytes_transferred);
	void		receive_result			(error_code const ec, std::size_t bytes_transferred);


	asio_io_service						m_asio_service;
	async_host							m_dispatch_host;
	io_service_work						m_asio_work;
	deadline_timer						m_test_timer;
	datagram_socket						m_udp_test;
	datagram_endpoint					m_udp_sender;
	boost::array<char, 256>				m_receive_buffer;
	boost::array<char, 256>				m_send_buffer;
};// class io_service

} // namespace lowlevel
} // namespace network
} // namespace xray

#include "io_service_inline.h"

#endif // #ifndef IO_SERVICE_H_INCLUDED