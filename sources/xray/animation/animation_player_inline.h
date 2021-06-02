////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_PLAYER_INLINE_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_PLAYER_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline animation_player::animation_player			( ) :
	m_current_buffer			( &m_tree_buffers[1] ),
	m_mixing_tree				( float4x4() ),
	m_callbacks_buffer			( m_callbacks_buffer_raw, sizeof(m_callbacks_buffer_raw) ),
	m_first_subscribed_channel	( 0 ),
	m_in_tick					( 0 ),
	m_callbacks_are_actual		( true )
{
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_PLAYER_INLINE_H_INCLUDED