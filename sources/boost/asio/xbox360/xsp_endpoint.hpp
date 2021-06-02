////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_ENDPOINT_HPP_INCLUDED
#define XSP_ENDPOINT_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/xray_defines.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

template <typename InternetProtocol>
class endpoint
{
public:
	enum enum_type
	{
		et_registered_ip	= 0x00,
		et_keyexchange_peer
	};
	static unsigned short const						default_port = 1001;
	typedef InternetProtocol						protocol_type;
	typedef boost::asio::detail::socket_addr_type	data_type;
	typedef boost::asio::detail::in4_addr_type		in_addr_type;
	
						endpoint	();
						endpoint	(boost::asio::ip::address const & address,
									 unsigned short const port = default_port);
						endpoint	(XNADDR const * xn_addr,
									 XNKID const * xn_session_id,
									 unsigned short const port = default_port);



						~endpoint	();
	protocol_type		protocol	() const { return protocol_type(); }
	data_type*			data		() { return &m_socket_addr; };
	data_type const *	data		() const { return &m_socket_addr; };
	std::size_t			size		() const { return sizeof(data_type); };
	void				resize		(std::size_t size);
	std::size_t			capacity	() const { return sizeof(data_type); };
	
	//static endpoint		localhost	(unsigned short const port = default_port);
private:
	data_type		m_socket_addr;
	enum_type		m_address_type;
};//class endpoint

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_endpoint_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_ENDPOINT_HPP_INCLUDED