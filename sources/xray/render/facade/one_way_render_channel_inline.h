////////////////////////////////////////////////////////////////////////////
//	Created		: 04.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_INLINE_H_INCLUDED
#define XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline one_way_render_channel::one_way_render_channel			(
		memory::base_allocator& owner_allocator
	) :
	m_channel					( owner_allocator ),
	m_current_frame_id			( 0 ),
	m_process_next_frame_commands ( false )
{
	// since we are sure these channels will be created in render thread only
	// we may initialize in constructor
	render_initialize			( );
}

inline void one_way_render_channel::owner_initialize			( )
{
	m_channel.owner_initialize	(
		XRAY_NEW_IMPL( m_channel.owner_allocator(), null_render_command ),
		XRAY_NEW_IMPL( m_channel.owner_allocator(), null_render_command )
	);
}

inline void one_way_render_channel::owner_finalize				( )
{
	m_channel.owner_finalize	( m_next_frame_commands_queue );
}

inline void one_way_render_channel::owner_push_back				( pointer_type const command )
{
	bool const empty			= m_channel.user_is_queue_empty();
	m_channel.owner_push_back	( command );
	if ( empty )
		m_wait_form_command_event.set	( true );
}

inline void one_way_render_channel::owner_delete_processed_items( )
{
	m_channel.owner_delete_processed_items	( );
}

inline void one_way_render_channel::render_initialize			( )
{
	m_channel.user_initialize	( );
}

inline bool one_way_render_channel::render_is_queue_empty		( ) const
{
	return						m_channel.user_is_queue_empty( );
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_INLINE_H_INCLUDED