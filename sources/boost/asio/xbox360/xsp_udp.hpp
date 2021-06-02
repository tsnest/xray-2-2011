////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_UDP_HPP_INCLUDED
#define XSP_UDP_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_datagram_socket.hpp>
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

/// Encapsulates the flags needed for UDP over XSP.
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
class udp
{
public:
  /// The type of a XSP endpoint.
  typedef endpoint<udp>										endpoint;

  /// The UDP resolver type.
  typedef basic_resolver<udp, xsp::resolver_service<udp> >	resolver;

  /// The type of a resolver query.
  typedef xsp::resolver_query<udp>							resolver_query;
  
  /// The type of a resolver iterator.
  typedef xsp::resolver_iterator<udp>						resolver_iterator;

  /// The UDP socket type.
  typedef basic_datagram_socket<udp>						socket;


  /// Construct to represent the IPv4 XSP UDP protocol.
  static udp v4()
  {
    return udp();
  }
  static udp v6()
  {
	BOOST_ASSERT(false && "not supported");
	return udp();
  }

  /// Obtain an identifier for the type of the protocol.
  int type() const
  {
	return SOCK_DGRAM;
  }

  /// Obtain an identifier for the protocol.
  int protocol() const
  {
    return IPPROTO_UDP;
  }

  /// Obtain an identifier for the protocol family.
  int family() const
  {
    return AF_INET;
  }

  // Default constructor
  udp()
  {
  }
}; // class udp

/// Compare two protocols for equality.
inline bool operator==(const udp& p1, const udp& p2)
{
  return true;
}

/// Compare two protocols for inequality.
inline bool operator!=(const udp& p1, const udp& p2)
{
  return false;
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_UDP_HPP_INCLUDED