////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_LAN_IMPL_INLINE_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_LAN_IMPL_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

inline lan_iterator::lan_iterator(sessions_registry & sess_reg) :
	resolver_iterator_base_impl(sess_reg),
	m_current_addr(m_list.begin())
{
}

inline lan_iterator::~lan_iterator()
{
}
	
inline void	lan_iterator::increment()
{
	BOOST_ASSERT(m_current_addr != m_list.end());
	if (m_current_addr == m_list.end())
		return;
	++m_current_addr;
}

inline secure_inaddr::pointer_type const & lan_iterator::dereference()
{
	return search_valid_lan(m_current_addr);
}

inline bool	lan_iterator::equal(pointer_type const & other)
{
	using namespace xray;
	lan_iterator* other_ptr = static_cast_checked<lan_iterator*>(
		other.c_ptr());
	BOOST_ASSERT(other_ptr);
	return m_current_addr == other_ptr->m_current_addr;
}

inline bool lan_iterator::empty()
{
	return m_current_addr == m_list.end();
}

inline void	lan_iterator::push_back(address_type const & addr)
{
	m_list.push_back	(addr);
	m_current_addr		= m_list.begin();
}

inline secure_inaddr::pointer_type const & lan_iterator::search_valid_lan(
	const_iterator const & from)
{
	m_current_dereferred		= NULL;
	const_iterator end_iter		= m_list.end();
	BOOST_ASSERT(from != end_iter);

	if (from == end_iter)
	{
		return m_current_dereferred;
	}

	for (m_current_addr = from; m_current_addr != end_iter; ++m_current_addr)
	{
		int errorcode = XNetRegisterKey(&m_current_addr->m_session_id,
			&m_current_addr->m_secret_key);
		
		if (errorcode)
			continue;

		m_current_dereferred = m_sessions_registry.add_session(
			m_current_addr->m_xnaddr,
			m_current_addr->m_session_id,
			&m_current_addr->m_secret_key);
		
		if (!m_current_dereferred || !(*m_current_dereferred))
		{
			XNetUnregisterKey(&m_current_addr->m_session_id);
			continue;
		}

		return m_current_dereferred;
	}
	
	m_list.clear					();
	m_current_addr = m_list.begin	();
	m_current_dereferred			= NULL;
	return m_current_dereferred;
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost


#endif // #ifndef XSP_RESOLVER_ITERATOR_LAN_IMPL_INLINE_HPP_INCLUDED