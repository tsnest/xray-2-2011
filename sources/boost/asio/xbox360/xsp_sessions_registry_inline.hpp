////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_SESSIONS_REGISTRY_INLINE_HPP_INCLUDED
#define XSP_SESSIONS_REGISTRY_INLINE_HPP_INCLUDED

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

inline sinaddr_intrusive_base::sinaddr_intrusive_base(sessions_registry* owner) : 
	m_reference_count(0),
	m_owner(owner)
{
}

inline void sinaddr_intrusive_base::destroy(secure_inaddr* object)
{
	m_owner->delete_session(object);
}

inline secure_inaddr::secure_inaddr(sessions_registry* owner,
							 in_addr_type const & secure_addr) :
	sinaddr_intrusive_base(owner)
{
	m_last_secure_in_addr	= secure_addr;
	int errcode				= XNetInAddrToXnAddr(m_last_secure_in_addr,
		&m_xnaddr, &m_xnkid);
	m_valid					= (errcode == 0);
}

inline secure_inaddr::secure_inaddr(sessions_registry* owner,
									XNADDR const & xn_addr,
									XNKID const & xn_kid,
									XNKEY const * xn_key) :
	sinaddr_intrusive_base(owner),
	m_xnaddr(xn_addr),
	m_xnkid(xn_kid)
{
	if (xn_key)
	{
		m_xnkey.reset(*xn_key);
	}
	reinit();
}

inline bool secure_inaddr::reinit()
{
	if (!m_valid)
		return false;

	BOOST_STATIC_ASSERT(sizeof(m_last_secure_in_addr) == sizeof(IN_ADDR));
	int errcode = XNetXnAddrToInAddr(&m_xnaddr, &m_xnkid, &m_last_secure_in_addr);
	m_valid		= (errcode == 0);
	return m_valid;
}

inline sessions_registry::sessions_registry(boost::asio::io_service& io_service) :
	boost::asio::detail::service_base<sessions_registry>(io_service)
{
}

inline sessions_registry::~sessions_registry()
{
	BOOST_ASSERT(m_sessions.empty());
}

inline void sessions_registry::delete_session(secure_inaddr* session)
{
	sessions_type::iterator tmp_iter = std::find(
		m_sessions.begin(),
		m_sessions.end(),
		session);
	BOOST_ASSERT(tmp_iter != m_sessions.end());
	BOOST_ASSERT(session);
	int const errcode	= XNetUnregisterKey(session->session_id());
	(void)errcode;
	BOOST_ASSERT(errcode == 0);
	XN_DELETE(session);
	m_sessions.erase(tmp_iter);
}
	
inline secure_inaddr::pointer_type	sessions_registry::add_session(
		secure_inaddr::in_addr_type const & secure_addr)
{
	secure_inaddr* new_session = XN_NEW(secure_inaddr)(this, secure_addr);
	BOOST_ASSERT(*new_session);
	if (!(*new_session))
	{
		XN_DELETE(new_session);
		return secure_inaddr::pointer_type();
	}
	m_sessions.push_back(new_session);
	return secure_inaddr::pointer_type(new_session);
}

inline secure_inaddr::pointer_type	sessions_registry::add_session(
		XNADDR const & xn_addr,
		XNKID const & xn_kid,
		XNKEY const * xn_key)
{
	secure_inaddr* new_session = XN_NEW(secure_inaddr)(
		this, xn_addr, xn_kid, xn_key);
		 
	BOOST_ASSERT(new_session);
	if (!(*new_session))
	{
		XN_DELETE(new_session);
		return secure_inaddr::pointer_type();
	}
	m_sessions.push_back(new_session);
	return secure_inaddr::pointer_type(new_session);
}

inline secure_inaddr::pointer_type	sessions_registry::init_title_address()
{
	BOOST_ASSERT(!m_title_address);
	XNKID	tmp_id;
	XNADDR	tmp_addr;
	XNKEY	tmp_key;
	int errorcode = XNetCreateKey(&tmp_id, &tmp_key);
	BOOST_ASSERT(errorcode == 0);
	if (errorcode)
	{
		LOG_ERROR("failed to create security key (XNetCreateKey), errorcode = 0x%08x, lasterror = 0x%08x",
			errorcode,
			::GetLastError());
		return secure_inaddr::pointer_type();
	}
	errorcode = XNetRegisterKey(&tmp_id, &tmp_key);
	if (errorcode)
	{
		LOG_ERROR("failed to register security key (XNetRegisterKey), errorcode = 0x%08x, lasterror = 0x%08x",
			errorcode,
			::GetLastError());
		return secure_inaddr::pointer_type();
	}

	while (XNetGetTitleXnAddr(&tmp_addr) == XNET_GET_XNADDR_PENDING)
	{
		LOG_WARNING("title xnaddr is not ready yet");
		Sleep(50);
	}
	secure_inaddr::pointer_type	result = add_session(tmp_addr, tmp_id, &tmp_key);
	BOOST_ASSERT(result);
	if (!result)
	{
		LOG_ERROR("failed to create node in session registry");
		XNetUnregisterKey(&tmp_id);
		return secure_inaddr::pointer_type();
	}
	return result;	
}

inline secure_inaddr::pointer_type	sessions_registry::get_title_address()
{
	if (!m_title_address)
	{
		m_title_address = init_title_address();
	}
	return m_title_address;
}


} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#endif // #ifndef XSP_SESSIONS_REGISTRY_INLINE_HPP_INCLUDED