////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XSP_SESSIONS_REGISTRY_HPP_INCLUDED
#define XSP_SESSIONS_REGISTRY_HPP_INCLUDED

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/service_base.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/ip/address.hpp>

#include <xray/intrusive_ptr.h>
#include <xray/network/sources/writers.h>
#include <xray/network/sources/readers.h>

namespace boost {
namespace asio {
namespace ip {
namespace xsp {

class sessions_registry;
class secure_inaddr;

class sinaddr_intrusive_base
{
public:
	inline			sinaddr_intrusive_base	(sessions_registry* owner);
	inline	void	destroy					(secure_inaddr* object);

	long volatile		m_reference_count;
private:
	sessions_registry*	m_owner;
}; // class sinaddr_intrusive_base

class secure_inaddr : public sinaddr_intrusive_base
{
public:
	typedef boost::asio::detail::in4_addr_type		in_addr_type;
	typedef xray::intrusive_ptr<secure_inaddr, sinaddr_intrusive_base,
		xray::threading::multi_threading_policy_base>	pointer_type;

	inline						secure_inaddr	(sessions_registry* owner,
												 in_addr_type const & secure_addr);
	inline						secure_inaddr	(sessions_registry* owner,
												 XNADDR const & xn_addr,
												 XNKID const & xn_kid,
												 XNKEY const * xn_key);
	inline in_addr_type const &	in_addr			() const { return m_last_secure_in_addr; };
	inline bool					reinit			();
	XNKID const *				session_id		() const { return &m_xnkid; }
	XNADDR const *				xn_address		() const { return &m_xnaddr; }
	XNKEY const *				xn_secret_key	() const { return m_xnkey ? &(*m_xnkey) : NULL; }
	operator					bool			() const { return m_valid; }
	bool						operator!		() const { return !m_valid; }

	template <typename Writer>
	void				write(Writer & writer) const
	{
		BOOST_ASSERT(m_xnkey);
		writer.write	((pvoid)&*m_xnkey,	sizeof(XNKEY));
		writer.write	((pvoid)&m_xnkid,	sizeof(XNKID));
		writer.write	((pvoid)&m_xnaddr,	sizeof(XNADDR));
	}
	struct reader
	{
		XNKEY		m_peer_key;
		XNKID		m_peer_kid;
		XNADDR		m_peer_xnaddr;

		template <typename Reader>
		xray::signalling_bool		read(Reader & reader)
		{
			xray::mutable_buffer	key_dest	((pvoid)&m_peer_key, sizeof(m_peer_key));
			xray::mutable_buffer	kid_dest	((pvoid)&m_peer_kid, sizeof(m_peer_kid));
			xray::mutable_buffer	peer_addr	((pvoid)&m_peer_xnaddr, sizeof(m_peer_xnaddr));

			return (
				reader.read(key_dest,	sizeof(m_peer_key)) &&
				reader.read(kid_dest,	sizeof(kid_dest)) &&
				reader.read(peer_addr,	sizeof(peer_addr))) ? true : false;
		}
	}; // struct reader
private:
	in_addr_type			m_last_secure_in_addr;
	XNADDR					m_xnaddr;
	XNKID					m_xnkid;
	boost::optional<XNKEY>	m_xnkey;
	secure_inaddr*			m_next;
	secure_inaddr*			m_prev;
	bool					m_valid;
}; // class secure_inaddr

struct full_secure_address
{
	XNKID	m_session_id;
	XNKEY	m_secret_key;
	XNADDR	m_xnaddr;
}; // full_secure_address

class sessions_registry :
	public boost::asio::detail::service_base<sessions_registry>
{
public:
	inline			sessions_registry		(boost::asio::io_service& io_service);
	inline			~sessions_registry		();

	inline secure_inaddr::pointer_type	get_title_address	();

	inline void							delete_session		(secure_inaddr* session);
	inline secure_inaddr::pointer_type	add_session			(
		secure_inaddr::in_addr_type const & secure_addr);
	inline secure_inaddr::pointer_type	add_session			(
		XNADDR const & xn_addr, XNKID const & xn_kid, XNKEY const * xn_key);
private:
	inline secure_inaddr::pointer_type	init_title_address	();
	virtual void						shutdown_service	()
	{
		while (!m_sessions.empty())
		{
			delete_session			(m_sessions.front());
		}
		if (m_title_address)
		{
			XNetUnregisterKey		(m_title_address->session_id());
			secure_inaddr* todel	= m_title_address.c_ptr();
			XN_DELETE				(todel);
		}
	}

	typedef xray::network::vector<secure_inaddr*>	sessions_type;	
	
	sessions_type				m_sessions;
	secure_inaddr::pointer_type	m_title_address;
	
}; // class sessions_registry

} // namespace xsp
} // namespace ip
} // namespace asio
} // namespace boost

#include "xsp_sessions_registry_inline.hpp"
#include <boost/asio/detail/pop_options.hpp>

#endif // #ifndef XSP_SESSIONS_REGISTRY_HPP_INCLUDED