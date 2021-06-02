////////////////////////////////////////////////////////////////////////////
//	Created		: 01.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_BASE_IMPL_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_BASE_IMPL_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>
#include <boost/asio/xbox360/xsp_sessions_registry.hpp>
#include <boost/xray_defines.hpp>
#include <xray/intrusive_ptr.h>


namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class resolver_iterator_base_impl
{
public:
	typedef xray::intrusive_ptr<
		resolver_iterator_base_impl,
		resolver_iterator_base_impl,
		xray::threading::multi_threading_policy_base>	pointer_type;

	inline		resolver_iterator_base_impl	(sessions_registry & sess_reg) :
		m_reference_count(0),
		m_sessions_registry(sess_reg)
	{
	};
	
	inline void destroy(resolver_iterator_base_impl* in_addrs)
	{
		XN_DELETE(in_addrs);
	}
	virtual	inline ~resolver_iterator_base_impl()
	{
	}
	virtual void								increment		() = 0;
	virtual secure_inaddr::pointer_type const &	dereference		() = 0;
	virtual bool								equal			(pointer_type const & other) = 0;
	virtual	bool								empty			() = 0;
	
	long					m_reference_count;
protected:
	sessions_registry &		m_sessions_registry;
}; // class xsp_resolver_iterator_base_impl

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_RESOLVER_ITERATOR_BASE_IMPL_HPP_INCLUDED