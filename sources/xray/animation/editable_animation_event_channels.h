////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_EDITABLE_ANIMATION_EVENT_CHANNELS_H_INCLUDED
#define XRAY_ANIMATION_EDITABLE_ANIMATION_EVENT_CHANNELS_H_INCLUDED

#include <xray/configs_lua_config.h>
#include <xray/animation/animation_event_channels.h>

namespace xray {
namespace animation {

class editable_animation_event_channels : private boost::noncopyable {

public:
		editable_animation_event_channels( animation_event_channels const& source );
		~editable_animation_event_channels( );

		configs::lua_config_value		config_value	( ) const;
		animation_event_channels const&	channels		( );
		void							save			( pcstr path ) const;

private:
	void				create_channels_from_config();	

private:
	animation_event_channels			*m_event_channels;
	configs::lua_config_ptr				m_config;
	pvoid								m_event_channels_memory;
	mutable bool						m_dirty;
}; // class editable_animation_event_channels

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_EDITABLE_ANIMATION_EVENT_CHANNELS_H_INCLUDED