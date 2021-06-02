////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_SERVICE_IMPL_HPP_INCLUDED
#define XSP_RESOLVER_SERVICE_IMPL_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/service_base.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

template <typename Protocol>
class resolver_service_impl :
	public boost::asio::detail::service_base<resolver_service_impl<Protocol> >
{
public:
	// The endpoint type.
  typedef typename Protocol::endpoint			endpoint_type;

  // The query type.
  typedef typename Protocol::resolver_query		query_type;

  // The iterator type.
  typedef typename Protocol::resolver_iterator iterator_type;
}; // class resolver_service_impl


} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_SERVICE_IMPL_HPP_INCLUDED