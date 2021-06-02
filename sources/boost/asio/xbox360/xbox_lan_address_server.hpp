////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_LAN_ADDRESS_SERVER_HPP_INCLUDED
#define XBOX_LAN_ADDRESS_SERVER_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/array.hpp>
#include <boost/asio/xbox360/xsp_lan_addresses_sync_defines.hpp>
#include <xray/network/sources/writers.h>
#include <xray/network/sources/readers.h>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class xbox_lan_address_server
{
public:
	inline	xbox_lan_address_server		(boost::asio::io_service& io_service);
	inline	~xbox_lan_address_server	();

	inline bool	start_passive_host			(unsigned short udp_port,
											 boost::system::error_code & ec);
	inline bool	start_active_host			(unsigned short udp_port,
											 boost::system::error_code & ec);
	inline void	shutdown					();
	inline void	on_receive					(xray::const_buffer const & buffer);
	inline void on_receive_key				(xray::const_buffer const & buffer);
	inline void on_receive_key_timeout		();
private:
	typedef xsp::udp::socket				socket_type;
	typedef boost::array<byte, 256>			receive_buffer_type;
	typedef boost::array<byte, 256>			send_buffer_type;
	
	inline void start_receive				();
	inline void wait_response_peer_key		();
	inline void send_response				(random_id const & peer_id);

	struct receiver_functor
	{
		receiver_functor		(xbox_lan_address_server* owner) : m_owner(owner) {};
		inline void	operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_server*	m_owner;
	}; // struct receiver_functor
	struct key_receiver_functor
	{
		key_receiver_functor	(xbox_lan_address_server* owner) : m_owner(owner) {};
		inline void	operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_server*	m_owner;
	}; // struct receiver_functor
	struct key_receiver_timeout
	{
		key_receiver_timeout	(xbox_lan_address_server* owner) : m_owner(owner) {};
		inline void	operator()	(boost::system::error_code const & ec);

		xbox_lan_address_server*	m_owner;
	}; // struct receiver_functor

	struct sender_functor
	{
		sender_functor			(xbox_lan_address_server* owner) : m_owner(owner) {};
		inline void operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_server*	m_owner;
	}; // struct sender_functor
	
	io_service &					m_ioservice;
	sessions_registry &				m_sessions_registry;
	socket_type						m_host;
	deadline_timer					m_keyreceive_expire_timer;
#pragma message ( XRAY_TODO( "implement smart accept preparing algorithm" ) )
	secure_inaddr::pointer_type		m_last_peer_addr;
	random_id						m_dialog_session;
	
	socket_type::endpoint_type		m_broadcast_dest;
	receive_buffer_type				m_recv_buffer_store;
	mutable_buffers_1				m_recv_buffer;
	send_buffer_type				m_send_buffer_store;
}; // class xbox_lan_address_server


} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xbox_lan_address_server_inline.hpp"

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XBOX_LAN_ADDRESS_SERVER_HPP_INCLUDED