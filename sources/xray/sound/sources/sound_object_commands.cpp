////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_object_commands.h>
#include "sound_world.h"

namespace xray {
namespace sound {

sound_order::sound_order		( ) :
	m_next_for_orders			( 0 ),
	m_next_for_postponed_orders	( 0 )
{
}

sound_order::~sound_order		( )
{
}

void sound_order::execute	( )
{
}

sound_response::sound_response	( ) :
	m_next						( 0 )
{
}

sound_response::~sound_response	( )
{
}

void sound_response::execute	( )
{
}

//sound_object_order::sound_object_order (
//		sound_object_impl* const owner
//	) :
//	m_owner			( owner ),
//	m_next			( 0 )
//{
//}
//
//void sound_object_order::execute	( )
//{
//}
//
//callback_order::callback_order	(
//		cb_event const& event,
//		sound_object_impl* const owner,
//		void* context
//	) :
//	super			( owner ),
//	m_event_type	( event ),
//	m_context		( context )
//{
//}
//
//void callback_order::execute		( )
//{
//	switch ( m_event_type )
//	{
//	case ev_stream_end:
//		m_owner->on_stream_end		( );
//		break;
//
//	case ev_buffer_end:
//		m_owner->on_buffer_end		( m_context );
//		break;
//
//	case ev_buffer_error:
//		m_owner->on_buffer_error	( m_context );
//		break;
//
//	default:
//		LOG_ERROR					( "unknown callback event: %d", (int)m_event_type );
//		NODEFAULT					( );
//	}
//}

listener_properties_order::listener_properties_order	(
		sound_world& world,
		sound_scene& scene,
		float4x4 const& inv_view_matrix
	) :
	m_world				( world ),
	m_scene				( scene ), 
	m_inv_view_matrix	( inv_view_matrix )
{
}

void listener_properties_order::execute	( )
{
	return m_world.set_listener_properties_impl	( m_scene, m_inv_view_matrix );
}

//playback_order::playback_order	(
//		playback_event const& event,
//		sound_object_impl* const owner
//	) :
//	super		( owner ),
//	m_event		( event )
//{
//}
//
//void playback_order::execute		( )
//{
//	switch ( m_event )
//	{
//	case ev_play:
//		m_owner->play_impl			( );
//		break;
//
//	case ev_stop:
//		m_owner->stop_impl			( );
//		break;
//
//	default:
//		NODEFAULT					( );
//	};
//}
//
//sound_position_order::sound_position_order	(
//		float3 const& position,
//		sound_object_impl* const owner
//	) :
//	super		( owner ),
//	m_position	( position )
//{
//}
//
//void sound_position_order::execute	( )
//{
//	m_owner->set_position_impl			( m_position );
//}

} // namespace sound
} // namespace xray
