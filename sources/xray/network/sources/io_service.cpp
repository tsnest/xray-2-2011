////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "io_service.h"
#include "writers.h"
#include "readers.h"

namespace xray {
namespace network {
namespace lowlevel {

static port_type const	test_port = 1001;

io_service::io_service() :
	m_asio_work			(m_asio_service),
	m_test_timer		(m_asio_service),
	m_udp_test			(m_asio_service)
{
	error_code							tmp_ec;
	datagram_endpoint					tmp_broadcast(ip_address_v4::broadcast(), test_port);
	datagram_endpoint					tmp_me(ip_address_v4::any(), test_port);
	m_udp_test.open						(tmp_me.protocol());
	m_udp_test.bind						(tmp_me);
	if (m_udp_test.set_option(datagram_socket::broadcast(true), tmp_ec))
	{
		LOG_ERROR("failed to set broadcast option to socket, errorcode = %d (%s)",
			tmp_ec.value(),
			tmp_ec.message().c_str());
		return;
	}

	m_test_timer.expires_from_now	(boost::posix_time::seconds(3));
	m_test_timer.async_wait			(m_dispatch_host.wrap(
		&io_service::test_timer_func, this));

	boost::asio::mutable_buffers_1	tmp_buffs(m_receive_buffer.data(), m_receive_buffer.size());
	m_udp_test.async_receive_from(tmp_buffs,
		m_udp_sender, m_dispatch_host.wrap(&io_service::receive_result, this));
}

io_service::~io_service()
{
	m_asio_service.stop();
}

void io_service::sent_result(error_code const ec, std::size_t bytes_transferred)
{
	using namespace boost;
	static size_t bytes_sent = 0;
	if (ec)
	{
		LOG_ERROR("failed to send data, errorcode = %d (%s)",
			ec.value(),
			ec.message().c_str());
	} else
	{
		bytes_sent	+= bytes_transferred;
		LOG_INFO("%u bytes sent...", bytes_sent);
	}
	m_test_timer.expires_from_now	(boost::posix_time::seconds(3));
	m_test_timer.async_wait			(m_dispatch_host.wrap(
		&io_service::test_timer_func, this));
}

void io_service::receive_result(error_code const ec,
								std::size_t bytes_transferred)
{
	using namespace boost;
	typedef reader<buffer_reader_device, true> buffer_signalling_reader;
	
	if (ec)
	{
		LOG_ERROR("failed to receive data, errorcode = %d (%s)",
			ec.value(),
			ec.message().c_str());
		return;
	}

	fixed_string512				tmp_message_dest;
	buffer_reader_device		tmp_rdevice(
		pointer_cast<pcbyte>(
			static_cast<char const*>(m_receive_buffer.data())),
		(u32)bytes_transferred);
	buffer_signalling_reader	tmp_reader(tmp_rdevice);
	
	if (tmp_reader.r_string(tmp_message_dest))
	{
		LOG_INFO("received message: %s", tmp_message_dest.c_str());
	} else
	{
		LOG_INFO("received %u bytes of binary data", bytes_transferred);
	}

	asio::mutable_buffers_1		tmp_buffs(m_receive_buffer.data(), m_receive_buffer.size());
	m_udp_test.async_receive_from(tmp_buffs,
		m_udp_sender, m_dispatch_host.wrap(&io_service::receive_result, this));
}

void io_service::test_timer_func(boost::system::error_code const ec)
{
	using namespace boost;
	LOG_INFO("timer expired, errorcode = %d", ec.value());
	
	datagram_endpoint				tmp_broadcast(ip_address_v4::broadcast(), test_port);
	buffer_writer_device			tmp_wdevice(
		pointer_cast<buffer_writer_device::pointer_type>(m_send_buffer.data()),
		(u32)m_send_buffer.size());
	buffer_writer					tmp_writer(tmp_wdevice);

	tmp_writer.w_char_array			("hello all");

	asio::const_buffers_1			tmp_buff(
		m_send_buffer.data(),
		tmp_writer.size());
	m_udp_test.async_send_to		(
		tmp_buff,
		tmp_broadcast,
		m_dispatch_host.wrap(&io_service::sent_result, this));
}

void io_service::tick()
{
	m_asio_service.run_one					();
	m_dispatch_host.invoke_ready_callbacks	();
}

} // namespace lowlevel
} // namespace network
} // namespace xray