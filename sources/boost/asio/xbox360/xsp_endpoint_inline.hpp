////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_ENDPOINT_INLINE_HPP_INCLUDED
#define XSP_ENDPOINT_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {


template <typename InternetProtocol>
endpoint<InternetProtocol>::endpoint()
{
}

template <typename InternetProtocol>
endpoint<InternetProtocol>::endpoint(boost::asio::ip::address const & address,
									 unsigned short const port = default_port)
{
	using namespace detail;
	using namespace detail::socket_ops;
	BOOST_ASSERT(address.is_v4());
	sockaddr_in4_type*			in4_addr = 
		reinterpret_cast<sockaddr_in4_type*>(&m_socket_addr);
	m_address_type				= et_registered_ip;
	in4_addr->sin_family		= InternetProtocol().family();
	in4_addr->sin_port			= host_to_network_short(port);
	in4_addr->sin_addr.s_addr	= address.to_v4().to_ulong();
	memset(in4_addr->sin_zero, 0, sizeof(in4_addr->sin_zero));
}

template <typename InternetProtocol>
endpoint<InternetProtocol>::endpoint	(XNADDR const * xn_addr,
										 XNKID const * xn_session_id,
										 unsigned short const port)
{
	using namespace detail;
	using namespace detail::socket_ops;
	BOOST_STATIC_ASSERT(sizeof(SOCKADDR_IN) == sizeof(sockaddr_in4_type));
	sockaddr_in4_type*				in4_addr = 
		reinterpret_cast<sockaddr_in4_type*>(&m_socket_addr);

	m_address_type					= et_keyexchange_peer;
	in4_addr->sin_family			= InternetProtocol().family();
	in4_addr->sin_port				= host_to_network_short(port);
	int errcode = XNetXnAddrToInAddr(xn_addr, xn_session_id, &in4_addr->sin_addr);
	BOOST_ASSERT(errcode == 0);
	memset(in4_addr->sin_zero, 0, sizeof(in4_addr->sin_zero));
}

template <typename InternetProtocol>
endpoint<InternetProtocol>::~endpoint()
{
}

template <typename InternetProtocol>
void	endpoint<InternetProtocol>::resize(std::size_t size)
{
	BOOST_ASSERT(false && "not implemented");
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#endif // #ifndef XSP_ENDPOINT_INLINE_HPP_INCLUDED