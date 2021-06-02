////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_INLINE_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline size_t		channels_self_memory_size( u32 channels_cnt )
{
	return channels_cnt * sizeof( event_channel );
}

template	<class ConfigValueType>
inline size_t	animation_event_channels::count_internal_memory_size( ConfigValueType const& config )
{
	size_t mem_size = channels_self_memory_size ( config.size() );
	
	typename ConfigValueType::const_iterator i = config.begin(), e = config.end();
	
	for ( ; i!=e; ++i )
		mem_size += event_channel::count_internal_memory_size( *i );
	return mem_size;

}

template	<class ConfigValueType>
inline void	animation_event_channels::create_in_place_internals ( ConfigValueType const& config, void* memory_buff )
{
	u32 const cnt = config.size();

	m_channels_count = cnt;

	if ( cnt == 0 )
		return;

	m_internal_memory_position = (u32)bytes_dist(  memory_buff, this );
	
	for ( u32 i = 0; i < m_channels_count; ++i )
	{
		new ( &channel( i ) ) event_channel;
	}
	
	memory_buff = get_shift_ptr( memory_buff, (u32)channels_self_memory_size( m_channels_count ) );

	for ( u32 i = 0; i < m_channels_count; ++i )
	{
		channel( i ).create_in_place_internals( config[ i ], memory_buff );
		
		memory_buff = get_shift_ptr( memory_buff, event_channel::count_internal_memory_size( config[i] ) );
	}
}

#ifndef	MASTER_GOLD

inline void	animation_event_channels::write( configs::lua_config_value &cfg )const
{
	if ( m_channels_count == u32(-1) )
		return;

	for ( u32 i = 0; i < m_channels_count; ++i )
	{
		configs::lua_config_value channel_cfg = cfg[i];
		channel( i ).write( channel_cfg );
	}
}

#endif
} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_INLINE_H_INCLUDED
