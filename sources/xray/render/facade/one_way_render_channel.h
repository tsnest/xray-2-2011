////////////////////////////////////////////////////////////////////////////
//	Created		: 04.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_H_INCLUDED
#define XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_H_INCLUDED

#include <xray/one_way_threads_channel.h>
#include <xray/render/facade/base_command.h>
#include <xray/render/api.h>
#include <xray/intrusive_spsc_queue.h>

template class XRAY_RENDER_API
	xray::intrusive_list<
		xray::render::base_command,
		xray::render::base_command*,
		&xray::render::base_command::deferred_next,
		xray::threading::single_threading_policy
	>;

namespace xray {
namespace render {

class XRAY_RENDER_API one_way_render_channel : private core::noncopyable {
private:
	typedef intrusive_spsc_queue< base_command, base_command, &base_command::next >				render_commands_queue_type;
	typedef one_way_threads_channel< render_commands_queue_type, render_commands_queue_type	>	one_way_threads_channel_type;
	typedef one_way_threads_channel_type::pointer_type	pointer_type;

public:
	inline					one_way_render_channel		(
								memory::base_allocator& owner_allocator
							);
	// owner functionality
	// must be called from the owner thread ONLY
	inline	void			owner_initialize			( );
	inline	void			owner_finalize				( );
	inline	void			owner_push_back				( pointer_type value );
	inline	void			owner_delete_processed_items( );

	// user functionality
	// must be called from the render thread ONLY
			bool			render_process_commands		( bool wait_for_command_if_queue_is_empty );
			void			render_on_draw_scene		( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view, bool use_depth );
			void			render_on_draw_scene2		( render::scene_ptr * scene, render::scene_view_ptr * scene_view, bool use_depth );
			void			render_on_end_frame			( );

private:
	inline	void			render_initialize			( );
	inline	bool			render_is_queue_empty		( ) const;
			void			process_next_frame_commands	( );

	template < typename Scene_OR_SceneViewType >
	inline	void			move_commands_from_list		( Scene_OR_SceneViewType const& scene_or_scene_view );

private:
	struct null_render_command : base_command {
		virtual	void		execute						( ) { }
	}; // struct null_render_command

private:
	typedef intrusive_list<
		base_command,
		base_command*,
		&base_command::deferred_next,
		threading::single_threading_policy
	> delayed_commands_queue_type;

private:
	one_way_threads_channel_type	m_channel;
	threading::event				m_wait_form_command_event;
	delayed_commands_queue_type		m_next_frame_commands_queue;
	scene_ptr						m_scenes;
	scene_view_ptr					m_scene_views;
	u32								m_current_frame_id;
	bool							m_process_next_frame_commands;
}; // class one_way_render_channel

} // namespace render
} // namespace xray

#include <xray/render/facade/one_way_render_channel_inline.h>

#endif // #ifndef XRAY_RENDER_BASE_ONE_WAY_RENDER_CHANNEL_H_INCLUDED