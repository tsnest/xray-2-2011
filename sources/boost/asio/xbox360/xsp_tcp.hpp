////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_TCP_HPP_INCLUDED
#define XSP_TCP_HPP_INCLUDED


#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/xbox360/xsp_endpoint.hpp>
#include <boost/asio/xbox360/xsp_resolver_service.hpp>
#include <boost/asio/xbox360/xsp_resolver_query.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator.hpp>
#include <boost/xray_defines.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

/// Encapsulates the flags needed for TCP over XSP.
/**
 * The boost::asio::ip::xsp::udp class contains flags necessary for 
 * XSP based UDP sockets on XBOX360.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol, InternetProtocol.
 */
class tcp
{
public:
	/// The type of a XSP endpoint.
	typedef endpoint<tcp>										endpoint;

	/// The tcp resolver type.
	typedef basic_resolver<tcp, xsp::resolver_service<tcp> >	resolver;

	/// The type of a resolver query.
	typedef xsp::resolver_query<tcp>							resolver_query;

	/// The type of a resolver iterator.
	typedef xsp::resolver_iterator<tcp>						resolver_iterator;

	/// The tcp socket type.
	typedef basic_stream_socket<tcp>							socket;

	/// The TCP acceptor type.
	typedef basic_socket_acceptor<tcp>						acceptor;


	/// Construct to represent the IPv4 XSP TCP protocol.
	static tcp v4()
	{
		return tcp();
	}
	static tcp v6()
	{
		BOOST_ASSERT(false && "not supported");
		return tcp();
	}

	/// Obtain an identifier for the type of the protocol.
	int type() const
	{
		return SOCK_STREAM;
	}

	/// Obtain an identifier for the protocol.
	int protocol() const
	{
		return IPPROTO_TCP;
	}

	/// Obtain an identifier for the protocol family.
	int family() const
	{
		return AF_INET;
	}

	// Default constructor
	tcp()
	{
	}
}; // class udp

/// Compare two protocols for equality.
inline bool operator==(const tcp& p1, const tcp& p2)
{
	return true;
}

/// Compare two protocols for inequality.
inline bool operator!=(const tcp& p1, const tcp& p2)
{
	return false;
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_TCP_HPP_INCLUDED