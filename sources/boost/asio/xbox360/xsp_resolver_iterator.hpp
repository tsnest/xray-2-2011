////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef resolver_iterator_HPP_INCLUDED
#define resolver_iterator_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/detail/push_options.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/asio/detail/pop_options.hpp>

#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator_dns_impl.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator_lan_impl.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

/// An iterator over the entries produced by a resolver.
/**
 * The boost::asio::ip::resolver_iterator class template is used to define
 * iterators over the results returned by a resolver.
 *
 * The iterator's value_type, obtained when the iterator is dereferenced, is:
 * @code const resolver_iterator<InternetProtocol> @endcode
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
template <typename InternetProtocol>
class resolver_iterator :
	public boost::iterator_facade<
		resolver_iterator<InternetProtocol>,
		const secure_inaddr::pointer_type,
        boost::forward_traversal_tag>
{
public:
	typedef resolver_iterator_base_impl::pointer_type	implementation_ptr;

	/// Default constructor creates an end iterator.
	resolver_iterator					();
	resolver_iterator					(XNDNS* dns_lookup,
										 DWORD xlsp_serviceo_id,
										 boost::asio::io_service & io_service);
	resolver_iterator					(boost::asio::io_service & io_service);

	implementation_ptr		get_impl	() { return m_impl; }
private:
	friend class boost::iterator_core_access;
	
	void	increment						();
	bool	equal							(resolver_iterator const & other) const;
	secure_inaddr::pointer_type	const &		dereference() const;
	
	implementation_ptr						m_impl;
}; // class resolver_iterator

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_resolver_iterator_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef resolver_iterator_HPP_INCLUDED