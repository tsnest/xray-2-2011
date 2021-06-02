////////////////////////////////////////////////////////////////////////////
//	Created		: 01.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_DNS_IMPL_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_DNS_IMPL_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator_base_impl.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class dns_iterator : public resolver_iterator_base_impl
{
public:
	inline				dns_iterator	(XNDNS * dns_lookup_result,
										 DWORD const service_id,
										 sessions_registry & sess_reg);
	virtual inline		~dns_iterator	();
	
	virtual inline void									increment			();
	virtual inline secure_inaddr::pointer_type const &	dereference			();
	virtual inline bool									equal				(pointer_type const & other);
	virtual	inline bool									empty				();

private:
	inline			bool						search_valid_xlsp	(int from);
	inline			void						clear				();
	
	XNDNS *							m_dns_lookup_result;
	DWORD							m_xlsp_service_id;
	int								m_index;
	secure_inaddr::pointer_type		m_current_dereferred;
}; // class dns_in_addrs

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_resolver_iterator_dns_impl_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_ITERATOR_DNS_IMPL_HPP_INCLUDED