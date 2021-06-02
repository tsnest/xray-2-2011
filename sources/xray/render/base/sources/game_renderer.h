////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RENDERER_H_INCLUDED
#define GAME_RENDERER_H_INCLUDED

#include <xray/render/base/game_renderer.h>

namespace xray {
namespace render {

struct platform;
class command_processor;
class base_world;

namespace debug {
	class debug_renderer;
} // namespace debug

namespace game {

class game_renderer :
	public xray::render::game::renderer,
	private boost::noncopyable
{
public:
					game_renderer				( render::base_world& world, platform& platform );
	virtual			~game_renderer				( );

	virtual	debug::renderer& debug				( );
	virtual	void	add_static					( resources::managed_resource_ptr const& collision,
												  resources::managed_resource_ptr const& geometry,
												  resources::managed_resource_ptr const& visuals );
	virtual	void	draw_static					( );
	virtual	void	draw_frame					( );

	virtual void	add_visual					( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected = false, bool system_object = false );
	virtual void	update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected = false );
	virtual void	remove_visual				( u32 id );
	virtual void	render_visuals				( );

	virtual	void	terrain_add_cell			( visual_ptr v );
	virtual	void	terrain_remove_cell			( visual_ptr v );

	virtual void	add_light					( u32 id, xray::render::light_props const& props );
	virtual void	update_light				( u32 id, xray::render::light_props const& props );
	virtual void	remove_light				( u32 id );

	virtual void	update_visual_vertex_buffer	( xray::render::visual_ptr const& object, vectora<buffer_fragment> const& fragments );
	virtual void	change_visual_shader		( xray::render::visual_ptr const& object, char const* shader, char const* texture );


private:
			void	draw_static_impl			( );
	virtual	void	set_command_push_thread_id	( );
	virtual	void set_command_processor_frame_id	( u32 frame_id );
	virtual	void	initialize_command_queue	( engine::command* null_command );
	
private:
	render::base_world&		m_world;
	platform&				m_platform;
	debug::debug_renderer*	m_debug;
}; // class game_renderer

} // namespace game
} // namespace render
} // namespace xray

#endif // #ifndef GAME_RENDERER_H_INCLUDED