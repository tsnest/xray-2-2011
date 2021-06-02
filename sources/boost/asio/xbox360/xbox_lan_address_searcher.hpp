////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX_LAN_ADDRESS_SEARCHER_HPP_INCLUDED
#define XBOX_LAN_ADDRESS_SEARCHER_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/xbox360/xsp_udp.hpp>
#include <boost/asio/xbox360/xsp_sessions_registry.hpp>
#include <boost/asio/xbox360/xsp_lan_addresses_sync_defines.hpp>


namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class xbox_lan_address_searcher :
	private boost::noncopyable
{
public:
	typedef full_secure_address					address_type;
	typedef xray::network::vector<address_type>	addresses_type;
	typedef addresses_type::const_iterator		const_iterator;
	typedef void (*handler_function_type)		(void* context,
												 boost::system::error_code const & ec,
												 address_type& new_address_found);

	inline xbox_lan_address_searcher		(boost::asio::io_service & ioservice);	
	
	inline void	search						(unsigned short udp_port,
											 unsigned int wait_time_ms,
											 handler_function_type handler,
											 void* handler_context);
private:
	typedef xsp::udp::socket					socket_type;
	typedef boost::array<char, 256>				receive_buffer_type;
	typedef boost::array<char, 256>				send_buffer_type;
	struct receiver_functor
	{
		receiver_functor		(xbox_lan_address_searcher* owner) : m_owner(owner) {};
		inline void	operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_searcher*	m_owner;
	}; // struct receiver_functor

	struct sender_functor
	{
		sender_functor			(xbox_lan_address_searcher* owner) : m_owner(owner) {};
		inline void operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_searcher*	m_owner;
	}; // struct sender_functor

	struct key_sender_functor
	{
		key_sender_functor		(xbox_lan_address_searcher* owner) : m_owner(owner) {};
		inline void operator()	(boost::system::error_code const & ec,
								 std::size_t bytes_transferred);
		xbox_lan_address_searcher*	m_owner;
	}; // struct key_sender_functor

	class error_category : public boost::system::error_category
	{
	public:
		virtual char const * name() const  {
			return "boost::asio::ip::xsp::xbox_lan_address_searcher"; }
		virtual inline xray::network::std_string message(int value) const;
	}; // class error_category

	inline void received_data		(boost::system::error_code const & ec,
									 std::size_t bytes_transferred);
	inline void data_sent			(boost::system::error_code const & ec,
									 std::size_t bytes_transferred);
	inline void mykey_sent			(boost::system::error_code const & ec,
									 std::size_t bytes_transferred);
	inline void expired_time		(boost::system::error_code const & ec);

	inline bool open_socket			(unsigned short udp_port);
	inline void send_request		();
	inline void process_response	(std::size_t bytes_transferred);
	inline void clear_handler		();
	

	boost::asio::io_service &	m_ioservice;
	sessions_registry &			m_sessions_registry;
	socket_type					m_searcher;
	socket_type::endpoint_type	m_broadcast_dest;
	handler_function_type		m_handler;
	void*						m_handler_context;
	boost::asio::deadline_timer	m_expire_timer;
	address_type				m_current_address;
	random_id					m_random_id;
	error_category				m_error;
	
	receive_buffer_type			m_receive_buffer;
	send_buffer_type			m_send_buffer;
}; // class xbox_lan_address_searcher

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xbox_lan_address_searcher_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XBOX_LAN_ADDRESS_SEARCHER_HPP_INCLUDED