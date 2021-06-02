////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_SERVICE_INLINE_HPP_INCLUDED
#define XSP_RESOLVER_SERVICE_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

template <typename Protocol>
resolver_service<Protocol>::~resolver_service()
{
	if (m_lan_address_searcher)
	{
		XN_DELETE(m_lan_address_searcher);
	}
}

template <typename Protocol>
typename resolver_service<Protocol>::iterator_type
	resolver_service<Protocol>::resolve(
		implementation_type& impl,
		const query_type& query,
		boost::system::error_code & ec)
{
	if (query.resolve_type() == query_type::ert_dns_xslp)
	{
		XNDNS*	lookup_result = NULL;
		int errcode = XNetDnsLookup(query.host_name().c_str(),
			impl.get_handle(),
			&lookup_result);

		if (errcode)
		{
			ec.assign(errcode, boost::asio::error::addrinfo_category);
			return iterator_type();
		}

		while (lookup_result->iStatus == WSAEINPROGRESS) 
		{
			impl.wait(xray::threading::event::wait_time_infinite);
		}
		if (lookup_result->iStatus)
		{
			if (lookup_result)
			{
				XNetDnsRelease(lookup_result);
			}
			ec = boost::asio::error::host_not_found;
			return iterator_type();
		}
		return iterator_type::create(
			lookup_result, query.service_id(),
			m_sessions_registry);
	} else if (query.resolve_type() == query_type::ert_peer_session_name)
	{

	}
	return iterator_type();
}

template <typename Protocol>
	template <typename Handler>
void resolver_service<Protocol>::async_resolve(implementation_type& impl,
											   const query_type& query,
											   Handler handler)
{
	using namespace detail;
	typedef async_resolver<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef raw_handler_ptr<alloc_traits>				pointer_allocator;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	pointer_allocator		handler_memory(handler);
	handler_contructor		handler_object(handler_memory, this, handler);

	if (query.resolve_type() == query_type::ert_lan_search)
	{
		if (!m_lan_address_searcher)
		{
			m_lan_address_searcher = XN_NEW(xbox_lan_address_searcher)(
				get_io_service());
		}
		
		m_lan_address_searcher->search(
			query.udp_lan_port(),
			3000,
			&value_type::lan_resolved,
			static_cast<void*>(handler_object.release()));
	} else
	{
		//not implemented
	}
}

template <typename Protocol>
	template <typename Handler>
resolver_service<Protocol>::async_resolver<Handler>::async_resolver(
		resolver_service* owner, Handler handler) :
	m_owner(owner),
	m_handler(handler),
	m_result(iterator_type(m_owner->get_io_service()))
{
}

template <typename Protocol>
	template <typename Handler>
void resolver_service<Protocol>::async_resolver<Handler>::lan_resolved(
	void* this_ptr,
	boost::system::error_code const & ec,
	full_secure_address& new_one)
{
	using namespace detail;
	typedef async_resolver<Handler>						value_type;
	typedef handler_alloc_traits<Handler, value_type>	alloc_traits;
	typedef handler_ptr<alloc_traits>					handler_contructor;

	BOOST_ASSERT(this_ptr);
	async_resolver* me = static_cast<async_resolver*>(this_ptr);
	if (!ec)
	{
		lan_iterator* lanaddrs_iter = xray::static_cast_checked<lan_iterator*>(
			me->m_result.get_impl().get());
		BOOST_ASSERT(lanaddrs_iter);
		lanaddrs_iter->push_back(new_one);
		return;
	}

	handler_contructor			ptr(me->m_handler, me);
	Handler						tmp_handler(me->m_handler);
	iterator_type				tmp_result = me->m_result;
	ptr.reset();
	
	if (ec.value() == boost::asio::error::timed_out)
	{
		boost_asio_handler_invoke_helpers::invoke(
			bind_handler(
				tmp_handler,
				boost::system::error_code(),
				tmp_result),
			&tmp_handler);
	} else
	{
		boost_asio_handler_invoke_helpers::invoke(
			bind_handler(tmp_handler, ec, tmp_result), &tmp_handler);
	}
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#endif // #ifndef XSP_RESOLVER_SERVICE_INLINE_H_INCLUDED