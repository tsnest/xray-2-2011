////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_H_INCLUDED

#include <xray/animation/event_channel.h>

namespace xray {
namespace animation {

//static const u32	max_event_channels = 10;

class XRAY_ANIMATION_API animation_event_channels {

public:
	animation_event_channels();
	~animation_event_channels();

public:
template	<class ConfigValueType>
static	size_t	count_internal_memory_size( ConfigValueType const& config );

public:
template	<class ConfigValueType>
void create_in_place_internals ( ConfigValueType const& config, void* memory_buff );

public:
		void				create_in_place_internals ( animation_event_channels const& event_channels, void* memory_buff );

#ifndef	MASTER_GOLD
public:
		void				write( configs::lua_config_value &cfg )const;
#endif

public:
		size_t				internal_memory_size	()const;
		void				copy_internals			( void* mem_buff )const;

public:
inline	u32						channels_count		( )	const		{ return m_channels_count; }		
		event_channel			&channel			( u32 id )	;
		event_channel	const	&channel			( u32 id ) const ;
		u32						get_channel_id		( pcstr name ) const ;
		
private:
	u32					m_channels_count;
	u32					m_internal_memory_position;

}; // class animation_event_channels

} // namespace animation
} // namespace xray

#include <xray/animation/animation_event_channels_inline.h>

#endif // #ifndef XRAY_ANIMATION_ANIMATION_EVENT_CHANNELS_H_INCLUDED