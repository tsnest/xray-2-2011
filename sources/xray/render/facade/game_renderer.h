////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_GAME_RENDERER_H_INCLUDED
#define XRAY_RENDER_FACADE_GAME_RENDERER_H_INCLUDED

#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>
#include <xray/render/engine/vertex_colored.h>

namespace xray {

namespace particle	{
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;

	class particle_system;
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_ptr;

	struct preview_particle_emitter_info;
	enum enum_particle_render_mode;
	struct world;
} // namespace particle

namespace render {

struct light_props;

typedef math::rectangle< math::float2 >	viewport_type;

namespace engine {
	class world;
} // namespace engine

class world;

namespace debug {
	class renderer;
} // namespace debug

namespace ui {
	class renderer;
} // namespace debug

class scene_renderer;

class render_model_instance;
typedef	resources::resource_ptr<
	render_model_instance,
	resources::unmanaged_intrusive_base
> render_model_instance_ptr;

namespace game {

class XRAY_RENDER_API renderer : private core::noncopyable {
private:
	friend class render::world;
			renderer		( render::world& world, engine::world& engine_world );

public:
			~renderer		( );
	
	debug::renderer& debug	( ) const;
	ui::renderer& ui		( ) const;
	scene_renderer& scene	( ) const;

	void	draw_scene		(
				scene_ptr const& scene,
				scene_view_ptr const& scene_view,
				render_output_window_ptr const& render_output_window,
				viewport_type const& viewport
			);
	void	end_frame		( );
	
private:
	struct draw_scene_params;
			void	draw_scene_impl	( draw_scene_params const& params );

private:
	render::world&		m_world;
	engine::world&		m_render_engine_world;
	debug::renderer*	m_debug;
	ui::renderer*		m_ui;
	scene_renderer*		m_scene;
}; // class game_renderer

} // namespace game
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_GAME_RENDERER_H_INCLUDED