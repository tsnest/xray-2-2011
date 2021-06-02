////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_EVENT_CHANNEL_INLINE_H_INCLUDED
#define XRAY_ANIMATION_EVENT_CHANNEL_INLINE_H_INCLUDED

namespace xray {
namespace animation {

template	<class ConfigValueType>
inline u32		event_channel::count_internal_memory_size( ConfigValueType const& config  )
{
	return (u32)time_channel_type::count_internal_memory_size( config["knots"].size() );
}

template	<class ConfigValueType>
inline void	event_channel::create_in_place_internals ( ConfigValueType const& config, void* memory_buff )
{
	const u32 cnt = config["knots"].size();
	
	m_time_channel.create_in_place( memory_buff, cnt );

	strings::copy( m_name, static_cast<pcstr>( config["name"] ) );

	m_type		= config["type"];

	for ( u32 i = 0; i < cnt ; ++i )
		m_time_channel.knot( i ) = config["knots"][i];

	const u32 domains_count	= m_time_channel.domains_count( );
	R_ASSERT(domains_count!=u32(-1));

	for ( u32 i = 0; i < domains_count ; ++i )
		m_time_channel.domain( i ).data = u8(-1);

	if ( !config.value_exists("domains") )
		return;

	for ( u32 i = 0; i < domains_count ; ++i )
		m_time_channel.domain( i ).data = config["domains"][i];

}

#ifndef	MASTER_GOLD
inline void	event_channel::write( configs::lua_config_value	&cfg )const
{
	const u32 cnt = knots_count();

	cfg["name"] = m_name;
	cfg["type"]	= m_type;

	for ( u32 i = 0; i< cnt; ++i )
		cfg["knots"][i] = m_time_channel.knot( i );
	
	const u32 domains_count	= m_time_channel.domains_count( );
	for ( u32 i = 0; i < domains_count ; ++i )
		cfg["domains"][i] = m_time_channel.domain( i ).data;
}
#endif

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_EVENT_CHANNEL_INLINE_H_INCLUDED