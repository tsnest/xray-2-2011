////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_LAN_ADDRESS_SEARCHER_INLINE_HPP_INCLUDED
#define XBOX_LAN_ADDRESS_SEARCHER_INLINE_HPP_INCLUDED

#include <xray/network/sources/writers.h>
#include <xray/network/sources/readers.h>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

inline xbox_lan_address_searcher::xbox_lan_address_searcher(boost::asio::io_service & ioservice) : 
	m_ioservice			(ioservice),
	m_sessions_registry	(boost::asio::use_service<sessions_registry>(ioservice)),
	m_searcher			(ioservice),
	m_broadcast_dest	(ip::address_v4::broadcast(), socket_type::endpoint_type::default_port),
	m_handler			(NULL),
	m_handler_context	(NULL),
	m_expire_timer		(ioservice)
{
}

inline void xbox_lan_address_searcher::search(unsigned short udp_port,
											  unsigned int wait_time_ms,
											  handler_function_type handler,
											  void* handler_context)
{
	BOOST_ASSERT(m_handler == NULL);
	if (m_handler)
	{
		m_expire_timer.cancel		();
		boost::system::error_code	tmp_ec(boost::asio::error::already_started, m_error);
		m_searcher.cancel			(tmp_ec);
	}
	m_handler							= handler;
	m_handler_context					= handler_context;
	if (!open_socket(udp_port)) // will invoke handle if any error occurs
	{
		return;
	}
	send_request						();
	m_expire_timer.expires_from_now		(boost::posix_time::milliseconds(wait_time_ms));
	m_expire_timer.async_wait			(
		std::bind1st(
			std::mem_fun(&xbox_lan_address_searcher::expired_time),
			this));
}

inline void	xbox_lan_address_searcher::receiver_functor::operator()(
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	m_owner->received_data(ec, bytes_transferred);
}

inline void xbox_lan_address_searcher::sender_functor::operator()(
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	m_owner->data_sent(ec, bytes_transferred);
}

inline void xbox_lan_address_searcher::key_sender_functor::operator()(
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	m_owner->mykey_sent(ec, bytes_transferred);
}

inline xray::network::std_string xbox_lan_address_searcher::error_category::message(
	int value) const
{
	switch (value)
	{
	case boost::asio::error::already_started:
		return "lan address searching operation already started";
	case boost::asio::error::timed_out:
		return "lan address search timed out";
	};// switch (value)
	return "unknown lan address search error";
}

inline void xbox_lan_address_searcher::received_data(boost::system::error_code const & ec,
													 std::size_t bytes_transferred)
{
	if (ec)
	{
		if (ec.value() == WSAECANCELLED)
		{
			m_handler(m_handler_context,
				boost::system::error_code(
					boost::asio::error::timed_out,
					m_error),
				m_current_address);
		} else
		{
			m_handler(m_handler_context, ec, m_current_address);
		}
		clear_handler();
		return;
	}
	
	process_response(bytes_transferred);
	m_searcher.async_receive(
		boost::asio::mutable_buffers_1(
			m_receive_buffer.data(), m_receive_buffer.size()),
		receiver_functor(this));
}

inline void xbox_lan_address_searcher::mykey_sent(boost::system::error_code const & ec,
												  std::size_t bytes_transferred)
{
	if (ec)
	{
		LOG_WARNING("failed to send my key to host, errorcode = 0x%08x (%s)",
			ec.value(),
			ec.message().c_str());
	}
	m_handler(m_handler_context, boost::system::error_code(), m_current_address);
}

inline void xbox_lan_address_searcher::data_sent(boost::system::error_code const & ec,
												 std::size_t bytes_transferred)
{
	if (ec)
	{
		m_handler(m_handler_context, ec, m_current_address);
		clear_handler();
		return;
	}

	m_searcher.async_receive(
		boost::asio::mutable_buffers_1(
			m_receive_buffer.data(), m_receive_buffer.size()),
		receiver_functor(this));
	
}

inline void xbox_lan_address_searcher::expired_time(
	boost::system::error_code const & ec)
{
	BOOST_ASSERT(!ec);
	
	boost::system::error_code tmp_err;
	if (m_searcher.cancel(tmp_err))
	{
		m_handler(m_handler_context,
			boost::system::error_code(
				boost::asio::error::timed_out,
				m_error),
			m_current_address);
		clear_handler();
	}
}

inline bool xbox_lan_address_searcher::open_socket(unsigned short udp_port)
{
	using namespace boost::asio::detail; // for bind_handler
	if (m_searcher.is_open())
	{
		return true;
	}
	boost::system::error_code	ec;
	if (m_searcher.open(socket_type::protocol_type(), ec))
	{
		m_ioservice.post(
			bind_handler(m_handler, m_handler_context, ec, m_current_address));
		clear_handler();
		return false;
	}
	if (m_searcher.bind(udp::endpoint(ip::address_v4::any(), udp_port), ec))
	{
		m_ioservice.post(
			bind_handler(m_handler, m_handler_context, ec, m_current_address));
		m_searcher.close();
		clear_handler();
		return false;
	}
	if (m_searcher.set_option(socket_type::broadcast(true), ec))
	{
		m_ioservice.post(
			bind_handler(m_handler, m_handler_context, ec, m_current_address));
		m_searcher.close();
		clear_handler();
		return false;
	}
	m_broadcast_dest	= socket_type::endpoint_type(
		ip::address_v4::broadcast(),
		udp_port);
	return true;
}

inline void xbox_lan_address_searcher::send_request()
{
	xray::mutable_buffer		tmp_mbuffer	(m_send_buffer.data(), m_send_buffer.size());
	xray::buffer_writer_device	tmp_wdevice	(tmp_mbuffer);
	xray::buffer_writer			tmp_writer	(tmp_wdevice);
	
	XNetRandom				(xray::pointer_cast<BYTE*>(m_random_id.m_data),
		sizeof(m_random_id.m_data));

	tmp_writer.w_char_array	(lan_search_request_string);
	m_random_id.write		(tmp_writer);
	
	m_searcher.async_send_to(
		const_buffers_1(m_send_buffer.data(), tmp_writer.size()),
		m_broadcast_dest,
		sender_functor(this));
}

inline void xbox_lan_address_searcher::process_response(std::size_t bytes_transferred)
{
	typedef xray::reader<xray::buffer_reader_device, true>	signalling_buffer_reader;
	
	xray::buffer_reader_device	tmp_rdevice(
		xray::const_buffer(m_receive_buffer.data(), bytes_transferred));
	signalling_buffer_reader	tmp_reader(tmp_rdevice);

	random_id				dest_id;
	random_id				sender_id;
	xray::fixed_string512	tmp_str;
	secure_inaddr::reader	tmp_host_addr;

	bool result =
		tmp_reader.r_string		(tmp_str) &&
		!xray::strings::compare	(tmp_str.c_str(), lan_search_response_string) &&
		dest_id.read			(tmp_reader) &&
		tmp_host_addr.read		(tmp_reader) &&
		sender_id.read			(tmp_reader);
		
	if (!result || (dest_id != m_random_id))
	{
		return;
	}

	secure_inaddr::pointer_type		address_ptr(
		m_sessions_registry.get_title_address());

	BOOST_ASSERT(address_ptr && *address_ptr);

	xray::mutable_buffer		tmp_mbuffer	(m_send_buffer.data(), m_send_buffer.size());
	xray::buffer_writer_device	tmp_wdevice	(tmp_mbuffer);
	xray::buffer_writer			tmp_writer	(tmp_wdevice);

	tmp_writer.w_char_array		(lan_my_key_string);
	sender_id.write				(tmp_writer);
	address_ptr->write			(tmp_writer);
	
	m_searcher.async_send_to(
		const_buffers_1(m_send_buffer.data(), tmp_writer.size()),
		m_broadcast_dest,
		key_sender_functor(this));
}

inline void xbox_lan_address_searcher::clear_handler()
{
	m_handler			= NULL;
	m_handler_context	= NULL;
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost


#endif // #ifndef XBOX_LAN_ADDRESS_SEARCHER_INLINE_HPP_INCLUDED