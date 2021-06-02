////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_WORLD_H_INCLUDED
#define XRAY_RENDER_WORLD_H_INCLUDED

#include <xray/render/facade/common_types.h>
#include <xray/render/facade/one_way_render_channel.h>
#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>

namespace xray {

#ifdef _MSC_VER
template class XRAY_RENDER_API intrusive_ptr<
	resources::managed_resource, 
	resources::managed_intrusive_base, 
	threading::mutex
>;
#endif // #ifdef _MSC_VER

namespace render {

namespace extended	{ class renderer; }
namespace engine	{ class renderer; }
namespace game		{ class renderer; }
namespace ui		{ class renderer; }
namespace editor	{ class renderer; }

namespace engine {
	class world;
} // namespace engine

class engine_renderer;

class XRAY_RENDER_API world {
public:
									world					(
										memory::base_allocator& logic_allocator,
										memory::base_allocator* editor_allocator
									);
									~world					( );
			void					clear_resources			( );
			void					tick					( );

			void				enable_logic_thread_safety	( bool value );

	inline	engine::world&			engine_world			( ) { return *m_render_engine_world; }

			engine::renderer&		engine_renderer			( );
			game::renderer&			game_renderer			( );
			editor::renderer&		editor_renderer			( );

	inline	one_way_render_channel& logic_channel			( ) { return m_logic_channel; }
			void					end_frame_logic			( );

	inline	one_way_render_channel& editor_channel			( ) { return m_editor_channel; }
			void					end_frame_editor		( );

private:
									world					( world const& other );
			world&					operator =				( world const& other );

private:
			void					end_frame				( );

private:
	one_way_render_channel		m_logic_channel;
	one_way_render_channel		m_editor_channel;
	timing::timer				m_timer;
	engine::world*				m_render_engine_world;
	render::engine::renderer*	m_engine_renderer;
	render::game::renderer*		m_game_renderer;
	render::editor::renderer*	m_editor_renderer;
	u32							m_last_frame_time_in_ms;
	threading::atomic32_type	m_is_logic_enabled;
	threading::atomic32_type	m_is_logic_frame_ended;
	bool						m_is_editor_frame_ended;
	bool						m_is_editor;
}; // class world

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_WORLD_H_INCLUDED