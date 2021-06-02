////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_INLINE_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

template <typename InternetProtocol>
resolver_iterator<InternetProtocol>::resolver_iterator() :
	m_impl(NULL)
{
}

template <typename InternetProtocol>
resolver_iterator<InternetProtocol>::resolver_iterator(
	XNDNS* dns_lookup,
	DWORD xlsp_service_id,
	boost::asio::io_service & io_service)
{
	m_impl.swap(XN_NEW(dns_iterator)(
		dns_lookup,
		xlsp_service_id,
		boost::asio::use_service<sessions_registry>(io_service)));
}

template <typename InternetProtocol>
resolver_iterator<InternetProtocol>::resolver_iterator(
	boost::asio::io_service & io_service)
{
	sessions_registry & reg_srv	= boost::asio::use_service<sessions_registry>(io_service);
	m_impl = implementation_ptr(XN_NEW(lan_iterator)(reg_srv));
}

template <typename InternetProtocol>
void resolver_iterator<InternetProtocol>::increment()
{
	BOOST_ASSERT(m_impl);
	m_impl->increment();
}

template <typename InternetProtocol>
bool resolver_iterator<InternetProtocol>::equal(
	const resolver_iterator& other) const
{
	BOOST_ASSERT(m_impl);
	if (!other.m_impl)
	{
		return m_impl->empty();
	}
	return (m_impl == other.m_impl) && (m_impl->equal(other.m_impl));
}

template <typename InternetProtocol>
secure_inaddr::pointer_type	const &
	resolver_iterator<InternetProtocol>::dereference() const
{
	BOOST_ASSERT(m_impl);
	return m_impl->dereference();
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#endif // #ifndef XSP_RESOLVER_ITERATOR_INLINE_HPP_INCLUDED