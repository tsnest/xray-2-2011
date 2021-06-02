////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_EVENT_CHANNEL_H_INCLUDED
#define XRAY_ANIMATION_EVENT_CHANNEL_H_INCLUDED

#include <xray/animation/time_channel.h>

namespace xray {
namespace animation {

class event_channel {
public:
	struct domain_data {
		u8 data;
	}; // struct domain_data

public:
	template < class ConfigValueType >
	static inline u32			count_internal_memory_size	( ConfigValueType const& config  );

	template < class ConfigValueType >
	inline	void				create_in_place_internals	( ConfigValueType const& config, void* memory_buff );

#ifndef	MASTER_GOLD
public:
	inline 	void				write						( configs::lua_config_value	&cfg )const;
#endif // #ifndef MASTER_GOLD

public:
			u32					internal_memory_size		( )const;
	inline	u32					domain_id					( float t, u32& current_domain  ) const	{ return m_time_channel.domain( t, current_domain  ); }
	inline	domain_data const&	domain						( u32 id ) const						{ return m_time_channel.domain( id ); }
	inline	u32					domains_count				( ) const								{ return m_time_channel.domains_count(); }

	inline	float				knot						( u32 id ) const						{ return m_time_channel.knot( id ); }
	inline	float const*		knots						( )	const								{ return m_time_channel.knots( ); }
	inline	u32					knots_count					( )	const								{ return m_time_channel.knots_count( ); }

	inline	pcstr				name						( ) const								{ return m_name; }

private:
	typedef time_channel< domain_data >	time_channel_type;
	
	string16					m_name;
	time_channel<domain_data>	m_time_channel;
	u8							m_type;
}; // class event_channel

} // namespace animation
} // namespace xray

#include <xray/animation/event_channel_inline.h>

#endif // #ifndef XRAY_ANIMATION_EVENT_CHANNEL_H_INCLUDED