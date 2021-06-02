////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_QUERY_HPP_INCLUDED
#define XSP_RESOLVER_QUERY_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/ip/resolver_query_base.hpp>
#include <boost/xray_defines.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

/// An query to be passed to a resolver.
/**
 * The boost::asio::ip::xsp::resolver_query class template describes a query
 * that can be passed to a resolver.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
template <typename InternetProtocol>
class resolver_query
{
public:
	/// The protocol type associated with the endpoint query.
	typedef InternetProtocol protocol_type;

	/// Type of peer to resolve (XLSP based server or xbox360 console with
	/// synchronized security keys
	enum enum_resolve_type
	{
		ert_dns_xslp				=	0x00,
		ert_lan_search
	}; // enum enum_resolve_type

	/// Construct query for lan searching
	resolver_query(unsigned short udp_port) :
		m_lan_udp_port(udp_port),
		m_resolve_type(ert_lan_search)
	{
	}
	/// Construct with specified xlsp host name and service identifier
	resolver_query(xray::network::std_string const & host_name,
				 DWORD const service_id) :
		m_host_name(host_name),
		m_xlsp_service_id(service_id),
		m_resolve_type(ert_dns_xslp)
	{
	}

	/// Get the host name associated with the query.
	xray::network::std_string const &		host_name	() const
	{
		return m_host_name;
	}
 
	/// Get resolve type associated with the query.
	enum_resolve_type const					resolve_type() const
	{
		return m_resolve_type;
	}
	/// Get XLSP service identifier associated with the query
	DWORD const								service_id	() const
	{
		BOOST_ASSERT(m_resolve_type == ert_dns_xslp);
		return m_xlsp_service_id;
	}
	unsigned short const					udp_lan_port() const
	{
		BOOST_ASSERT(m_resolve_type == ert_lan_search);
		return m_lan_udp_port;
	}
private:
  resolver_query() {}
  xray::network::std_string		m_host_name;
  // XLSP service identifier (in case of ert_dns_xslp)
  union
  {
	DWORD							m_xlsp_service_id;
	unsigned short					m_lan_udp_port;
  };
  enum_resolve_type				m_resolve_type;
  
};

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_QUERY_HPP_INCLUDED