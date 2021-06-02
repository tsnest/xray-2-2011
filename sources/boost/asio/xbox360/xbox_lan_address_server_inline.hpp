////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_LAN_ADDRESS_SERVER_INLINE_HPP_INCLUDED
#define XBOX_LAN_ADDRESS_SERVER_INLINE_HPP_INCLUDED

#include <xray/network/sources/writers.h>
#include <xray/network/sources/readers.h>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

inline xbox_lan_address_server::xbox_lan_address_server(
		boost::asio::io_service& io_service) :
	m_ioservice					(io_service),
	m_sessions_registry			(boost::asio::use_service<sessions_registry>(io_service)),
	m_host						(m_ioservice),
	m_keyreceive_expire_timer	(m_ioservice),
	m_broadcast_dest			(ip::address_v4::broadcast(), socket_type::endpoint_type::default_port),
	m_recv_buffer				(m_recv_buffer_store.data(), m_recv_buffer_store.size())
{
}

inline xbox_lan_address_server::~xbox_lan_address_server()
{
	if (m_host.is_open())
	{
		m_host.close();
	}
}

inline bool xbox_lan_address_server::start_passive_host(unsigned short udp_port,
														boost::system::error_code & ec)
{
	if (m_host.open(socket_type::protocol_type(), ec))
	{
		return false;
	}
	m_broadcast_dest = socket_type::endpoint_type(
		ip::address_v4::broadcast(),
		udp_port);
	return true;
}

inline bool xbox_lan_address_server::start_active_host(unsigned short udp_port,
													   boost::system::error_code & ec)
{
	if (m_host.open(socket_type::protocol_type(), ec))
	{
		return false;
	}
	if (m_host.bind(udp::endpoint(ip::address_v4::any(), udp_port), ec))
	{
		return false;
	}
	if (m_host.set_option(socket_type::broadcast(true), ec))
	{
		return false;
	}
	m_broadcast_dest	= socket_type::endpoint_type(
		ip::address_v4::broadcast(),
		udp_port);
	start_receive		();
	return true;
}

inline void xbox_lan_address_server::shutdown()
{
	m_host.close();
}


inline void xbox_lan_address_server::receiver_functor::operator () (
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	if (ec)
	{
		LOG_ERROR("xbox_lan_address_server: failed to receive data, errorcode = 0x%08x, (%s)",
			ec.value(),
			ec.message().c_str());
		return;
	}
	m_owner->on_receive(
		xray::const_buffer(m_owner->m_recv_buffer_store.data(), bytes_transferred));
}

inline void xbox_lan_address_server::key_receiver_functor::operator () (
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	if (ec)
	{
		LOG_ERROR("xbox_lan_address_server: failed to receive peer key, errorcode = 0x%08x, (%s)",
			ec.value(),
			ec.message().c_str());
		m_owner->start_receive();
		return;
	}
	m_owner->on_receive_key(
		xray::const_buffer(m_owner->m_recv_buffer_store.data(), bytes_transferred));
}

inline void xbox_lan_address_server::key_receiver_timeout::operator () (
	boost::system::error_code const & ec)
{
	BOOST_ASSERT(m_owner);
	if (ec)
	{
		LOG_ERROR("xbox_lan_address_server: receive new peer key error, errorcode = 0x%08x, (%s)",
			ec.value(),
			ec.message().c_str());
	}
	m_owner->on_receive_key_timeout();
}

inline void xbox_lan_address_server::sender_functor::operator() (
	boost::system::error_code const & ec, std::size_t bytes_transferred)
{
	BOOST_ASSERT(m_owner);
	if (ec)
	{
		LOG_ERROR("xbox_lan_address_server: failed to send response, errorcode = 0x%08x, (%s)",
			ec.value(),
			ec.message().c_str());
	}
	m_owner->wait_response_peer_key();
}

inline void xbox_lan_address_server::wait_response_peer_key()
{
	m_host.async_receive						(m_recv_buffer, key_receiver_functor(this));
	m_keyreceive_expire_timer.expires_from_now	(boost::posix_time::milliseconds(1000));
	m_keyreceive_expire_timer.async_wait		(key_receiver_timeout(this));
}

inline void xbox_lan_address_server::start_receive()
{
	m_host.async_receive(m_recv_buffer, receiver_functor(this));
}

inline void xbox_lan_address_server::send_response(random_id const & peer_id)
{
	secure_inaddr::pointer_type		address_ptr(
		m_sessions_registry.get_title_address());

	BOOST_ASSERT(address_ptr);

	xray::buffer_writer_device		tmp_wdevice(
		m_send_buffer_store.data(),
		m_send_buffer_store.size());
	xray::buffer_writer				tmp_writer(tmp_wdevice);

	XNetRandom(
		xray::pointer_cast<BYTE*>(m_dialog_session.m_data),
		sizeof(m_dialog_session.m_data));

	tmp_writer.w_char_array			(lan_search_response_string);
	peer_id.write					(tmp_writer);
	address_ptr->write				(tmp_writer);
	m_dialog_session.write			(tmp_writer);
	
	m_host.async_send_to(
		const_buffers_1(m_send_buffer_store.data(), tmp_writer.size()),
		m_broadcast_dest,
		sender_functor(this));
}

inline void xbox_lan_address_server::on_receive_key(xray::const_buffer const & buffer)
{
	typedef xray::reader<xray::buffer_reader_device, true> signalling_buffer_reader;
	
	xray::buffer_reader_device	tmp_rdevice(buffer);
	signalling_buffer_reader	tmp_reader(tmp_rdevice);

	secure_inaddr::pointer_type		address_ptr(
		m_sessions_registry.get_title_address());

	BOOST_ASSERT(address_ptr && *address_ptr);

	xray::fixed_string512	tmp_my_key_string;
	secure_inaddr::reader	tmp_peer_data;
	random_id				tmp_dialog_session;

	bool result =
		tmp_reader.r_string		(tmp_my_key_string) &&
		!xray::strings::compare	(tmp_my_key_string.c_str(), lan_my_key_string) &&
		tmp_dialog_session.read	(tmp_reader) &&
		tmp_peer_data.read		(tmp_reader);

	if (!result || (tmp_dialog_session != m_dialog_session))
	{
		m_host.async_receive(m_recv_buffer, key_receiver_functor(this));
		return;
	}

	m_last_peer_addr = m_sessions_registry.add_session(
		tmp_peer_data.m_peer_xnaddr,
		tmp_peer_data.m_peer_kid,
		&tmp_peer_data.m_peer_key);
	
	m_keyreceive_expire_timer.cancel();
	start_receive					();
}

inline void xbox_lan_address_server::on_receive_key_timeout()
{
	m_host.cancel();
	start_receive();
}

inline void xbox_lan_address_server::on_receive(xray::const_buffer const & buffer)
{
	typedef xray::reader<xray::buffer_reader_device, true> signalling_buffer_reader;

	xray::buffer_reader_device	tmp_rdevice(buffer);
	signalling_buffer_reader	tmp_reader(tmp_rdevice);

	xray::fixed_string512		tmp_request_string;
	random_id					tmp_peer_id;

	bool result =
		tmp_reader.r_string		(tmp_request_string) &&
		!xray::strings::compare	(
			tmp_request_string.c_str(), lan_search_request_string) &&
		tmp_peer_id.read		(tmp_reader);

	if (result)
	{
		send_response(tmp_peer_id);
	} else
	{
		start_receive();
	}
	//checking buffer to 
}


} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#endif // #ifndef XBOX_LAN_ADDRESS_SERVER_INLINE_HPP_INCLUDED