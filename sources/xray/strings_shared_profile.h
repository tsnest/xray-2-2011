////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STRINGS_SHARED_PROFILE_H_INCLUDED
#define XRAY_STRINGS_SHARED_PROFILE_H_INCLUDED

#include <xray/intrusive_ptr.h>

namespace xray {
namespace strings {
namespace shared {

class profile;

XRAY_CORE_API void remove	( profile* profile );

namespace detail {

struct intrusive_base {
private:
	friend class threading::multi_threading_policy;
	friend class threading::single_threading_policy;
	friend class xray::strings::shared::profile;
	threading::atomic32_type	m_reference_count;

public:

	inline			intrusive_base	( ) : m_reference_count( 0 )
	{
	}

	inline	void	destroy			( profile* const object )
	{
		shared::remove	( object );
	}
}; // struct intrusive_base

} // namespace detail

class profile : public detail::intrusive_base {
public:
	typedef intrusive_ptr<
		profile,
		detail::intrusive_base,
		threading::simple_lock
	>				profile_ptr;

public:
	static	profile*	create					( threading::mutex& mutex, pcstr value, profile const& temp );
	static	void		create_temp				( pcstr value, profile & result );
	static	void		destroy					( threading::mutex& mutex, profile *string );
	static	u32			create_checksum			( pcvoid begin, pcvoid end );

public:
	inline	threading::atomic32_type const& reference_count( ) const;
	inline	u32 const&	length					( ) const;
	inline	u32 const&	length_no_check			( ) const;
	inline	u32 const&	checksum				( ) const;
	inline	u32 const&	checksum_no_check		( ) const;
	inline	pcstr		value					( ) const;

private:
	enum {
		max_length	= 4096, //256,
	};

public:
	profile*		next_in_hashset;

private:
	u32				m_length;
	u32				m_checksum;
#if defined(DEBUG) && !defined(__GNUC__)
#	pragma warning( push )
#	pragma warning( disable:4200 )
	char			m_value[];
#	pragma warning( pop )
#endif // #if defined(DEBUG) && !defined(__GNUC__)
};

} // namespace shared
} // namespace strings
} // namespace xray

#include <xray/strings_shared_profile_inline.h>

#endif // #ifndef XRAY_STRINGS_SHARED_PROFILE_H_INCLUDED