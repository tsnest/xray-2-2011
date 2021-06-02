////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_SERVICE_HPP_INCLUDED
#define XSP_RESOLVER_SERVICE_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/service_base.hpp>
#include <boost/asio/detail/resolver_service.hpp>
#include <boost/asio/xbox360/xsp_sessions_registry.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class xbox_lan_address_searcher;

/// Service implementation for a xbox360 resolver.
template <typename Protocol>
class resolver_service
  : public boost::asio::detail::service_base<
      resolver_service<Protocol> >
{
public:
	/// The protocol type.
	typedef Protocol									protocol_type;

	/// The endpoint type.
	typedef typename Protocol::endpoint					endpoint_type;

	/// The query type.
	typedef typename Protocol::resolver_query			query_type;

	/// The iterator type.
	typedef typename Protocol::resolver_iterator		iterator_type;

	typedef xray::threading::event						implementation_type;

	/// Construct a new resolver service for the specified io_service.
	explicit resolver_service(boost::asio::io_service& io_service) :
		boost::asio::detail::service_base<resolver_service<Protocol> >(io_service),
		m_sessions_registry(boost::asio::use_service<sessions_registry>(io_service)),
		m_lan_address_searcher(NULL)
	{
	}
	~resolver_service();

	/// Destroy all user-defined handler objects owned by the service.
	void shutdown_service()
	{
	}

	/// Construct a new resolver implementation.
	void construct(implementation_type& impl)
	{
		impl.set(false);
	}

	/// Destroy a resolver implementation.
	void destroy(implementation_type& impl)
	{
	}

	/// Cancel pending asynchronous operations.
	void cancel(implementation_type& impl)
	{
	}

	/// Resolve a query to a list of entries.
	iterator_type resolve(implementation_type& impl, const query_type& query,
		boost::system::error_code& ec);

	/// Asynchronously resolve a query to a list of entries.
	template <typename Handler>
	void async_resolve(implementation_type& impl, const query_type& query,
		Handler handler);

	/// Resolve an endpoint to a list of entries.
	iterator_type resolve(implementation_type& impl,
	const endpoint_type& endpoint, boost::system::error_code& ec)
	{
	//return service_impl_.resolve(impl, endpoint, ec);
	return iterator_type();
	}

	/// Asynchronously resolve an endpoint to a list of entries.
	template <typename ResolveHandler>
	void async_resolve(implementation_type& impl, const endpoint_type& endpoint,
	ResolveHandler handler)
	{
	//return service_impl_.async_resolve(impl, endpoint, handler);
	}

	template <typename Handler>
	class async_resolver : boost::noncopyable
	{
	public:
					async_resolver	(resolver_service* owner, Handler handler);		
		static void lan_resolved	(void* this_ptr,
									 boost::system::error_code const & ec,
									 full_secure_address& new_one);
	private:
		resolver_service*		m_owner;
		Handler					m_handler;
		iterator_type			m_result;
	}; // struct async_resolver
private:
	sessions_registry &			m_sessions_registry;
	xbox_lan_address_searcher*	m_lan_address_searcher;
}; // class resolver_service

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_resolver_service_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_SERVICE_HPP_INCLUDED