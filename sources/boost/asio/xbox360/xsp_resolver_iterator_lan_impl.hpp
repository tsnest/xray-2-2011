////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_LAN_IMPL_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_LAN_IMPL_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/xbox360/xsp_resolver_iterator_base_impl.hpp>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class lan_iterator : public resolver_iterator_base_impl
{
public:
	typedef full_secure_address						address_type;
	typedef xray::network::list<address_type>		addresses_list_type;
	typedef addresses_list_type::const_iterator		const_iterator;

	inline				lan_iterator	(sessions_registry & sess_reg);
	virtual inline		~lan_iterator	();
	
	virtual inline void									increment			();
	virtual inline secure_inaddr::pointer_type const &	dereference			();
	virtual inline bool									equal				(pointer_type const & other);
	virtual	inline bool									empty				();

	inline void									push_back			(address_type const & addr);
private:
	inline secure_inaddr::pointer_type const &	search_valid_lan	(const_iterator const & from);
	addresses_list_type							m_list;
	const_iterator								m_current_addr;
	secure_inaddr::pointer_type					m_current_dereferred;
}; // class xsp_resolver_iterator_lan_impl

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_resolver_iterator_lan_impl_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_ITERATOR_LAN_IMPL_HPP_INCLUDED