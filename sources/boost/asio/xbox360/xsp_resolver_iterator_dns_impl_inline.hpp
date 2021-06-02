////////////////////////////////////////////////////////////////////////////
//	Created		: 01.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_RESOLVER_ITERATOR_DNS_IMPL_INLINE_HPP_INCLUDED
#define XSP_RESOLVER_ITERATOR_DNS_IMPL_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

inline dns_iterator::dns_iterator	(XNDNS * dns_lookup_result,
									 DWORD const service_id,
									 sessions_registry & sess_reg) :
	resolver_iterator_base_impl	(sess_reg),
	m_dns_lookup_result			(dns_lookup_result),
	m_xlsp_service_id			(service_id),
	m_index						(0)
{
	BOOST_ASSERT(dns_lookup_result);
	search_valid_xlsp(m_index);	
}

inline dns_iterator::~dns_iterator()
{
	clear();
}

inline void dns_iterator::increment()
{
	search_valid_xlsp(m_index + 1);
}

inline secure_inaddr::pointer_type const & dns_iterator::dereference()
{
	return m_current_dereferred;
}

inline bool dns_iterator::equal(pointer_type const & other)
{
	using namespace xray;
	dns_iterator* other_ptr = static_cast_checked<dns_iterator*>(
		other.c_ptr());
	BOOST_ASSERT(other_ptr);
	return ((m_dns_lookup_result == other_ptr->m_dns_lookup_result) &&
		(m_index == other_ptr->m_index));
}

inline bool dns_iterator::empty()
{
	return (m_dns_lookup_result == NULL);
}

inline bool	dns_iterator::search_valid_xlsp(int from)
{
	BOOST_ASSERT(m_dns_lookup_result);
	int count = m_dns_lookup_result->cina;
	for (m_index = from; m_index < count; ++m_index)
	{
		IN_ADDR tmp_addr;
		int errcode = XNetServerToInAddr(m_dns_lookup_result->aina[m_index],
			m_xlsp_service_id, &tmp_addr);
		if (errcode)
			continue;
		
		m_current_dereferred = m_sessions_registry.add_session(tmp_addr);
		if (!m_current_dereferred || !(*m_current_dereferred))
			continue;

		return true;
	}
	clear();
	return false;	
}

inline void dns_iterator::clear()
{
	if (m_dns_lookup_result)
	{
		XNetDnsRelease		(m_dns_lookup_result);
		m_dns_lookup_result	= NULL;
	}
	m_index						= 0;
	m_current_dereferred		= NULL;
}

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost


#endif // #ifndef XSP_RESOLVER_ITERATOR_DNS_IMPL_INLINE_HPP_INCLUDED